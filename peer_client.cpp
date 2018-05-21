#include <cctype>
#include <exception>

#include <boost/bind.hpp>

#include "peer_client.h"
#include "msg_formats.h"
#include "msg_builder.h"

unique_ptr<peer_client> peer_client::_instance;
once_flag peer_client::_onceFlag;

peer_client::peer_client(uint16 port, string seed_ip, uint16 seedport) : _send_socket(_io_service)
    , _recv_socket(_io_service)
    , _acceptor(_io_service, tcp_endpoint(Ipv4Address::any(), port))
    , _run_threads(1)
    , _port(port)
{
    cout << "p2p client started" << endl;

    // create handlers
    Handler::setChatInstance(this);
    _handlers.resize(LAST + 1);
    _handlers[M_SYS] = new handler_sys;
    _handlers[M_DATA] = new handler_text;
    _handlers[M_PEER_DATA] = new handler_peer_data;

    peer* seed = new peer(seed_ip, seedport);
    __peers_map[seed->get_id()] = seed;

    start_accept();

    /*
    In this thread io_service is ran
    We can't read user's input without it
    */
    ThreadsMap.insert(pair<string, shared_ptr<Thread>>(BOOST_SERVICE_THREAD,
        shared_ptr<Thread>(new Thread(boost::bind(&peer_client::boost_service_thread, this)))));

}

peer_client::~peer_client()
{
    _recv_socket.close();
    _send_socket.close();
    _io_service.stop();
    
    if (ThreadsMap[BOOST_SERVICE_THREAD].get())
        ThreadsMap[BOOST_SERVICE_THREAD]->join();
    
    ThreadsMap.clear();

    // Delete all handlers

    for (Handlers::iterator it = _handlers.begin(); it != _handlers.end(); ++it)
        delete (*it);

    // Delete all peers

    for (peers::iterator it = __peers_map.begin(); it != __peers_map.end(); ++it)
        delete((*it).second);
}

peer_client& peer_client::get_instance(uint16 port, string seed, uint16 seedport)
{
    call_once(_onceFlag, [=] {
        _instance.reset(new peer_client(port, seed, seedport));
    }
    );    
    return *_instance.get();
}

// Thread function: BOOST_SERVICE_THREAD

void peer_client::boost_service_thread()
{
    ErrorCode ec;

    while (_run_threads)
    {
        _io_service.run(ec);
        _io_service.reset();
    }

    if (ec)
        cout << ec.message();
}

// async reading handler (udp socket)
void peer_client::handle_receive_from(const ErrorCode& err, size_t size)
{
    if (err)
        return;

    // parse received packet
    message_system * pmsys = (message_system*)_data.data();
    if (pmsys->_code <= LAST)
    {
        (_handlers[pmsys->_code])->handle(_data.data(), size);
    }
}

void peer_client::handle_accept(const boost::system::error_code& error)
{
    if (!error)
    {
        size_t max_length = 1024;
        _recv_socket.async_read_some(boost::asio::buffer(_data, max_length),
                                boost::bind(&peer_client::handle_receive_from, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        _recv_socket = tcp_socket(_io_service);
    }

    start_accept();
}

void peer_client::start_accept()
{
    _acceptor.async_accept(_recv_socket,
                           boost::bind(&peer_client::handle_accept, this,
                                       boost::asio::placeholders::error));
}

// try to understand user's input
void peer_client::parse_user_input(const string& data)
{
    string tmp(data);

    // parse string
    if (tmp.size() > 1)
    {
        string peerCommand = tmp.substr(0, 5);
        if (peerCommand == "peers")
        {
            for (auto peer : __peers_map)
            {
                cout << peer.second->get_ip() << ":" << peer.second->get_port();
                cout << " id: " << peer.second->get_id() << endl;
            }
            return;
        }
    }

    if (tmp.empty())
        return;

    // send on broadcast address
    for (auto peer : __peers_map)
    {
        tcp_endpoint peerEndpoint(Ipv4Address::from_string(peer.second->get_ip()), peer.second->get_port());
        send_text(peerEndpoint, tmp);
    }
}

string peer_client::endpoint_to_string(const tcp_endpoint& ep)
{
    string ip_addr = ep.address().to_string();
    uint16 port = ep.port();
    string s_endpoint = ip_addr + ":" + to_string(port);
    return s_endpoint;
}

// Main loop of application

int peer_client::loop()
{
    try
    {
        cout << "Sending broadcast message (type & enter):" << endl;

        send_peer_data_msg("some_id", _port);

        for (string line;;)
        {
            getline(cin, line);
            // delete spaces to the right
            if (line.empty() == false)
            {
                string::iterator p;
                for (p = line.end() - 1; p != line.begin() && *p == L' '; --p);

                if (*p != L' ')
                    p++;

                line.erase(p, line.end());
            }

            if (line.empty())
                continue;

            try
            {
                // parse string
                parse_user_input(line);
            }
            catch (const logic_error& e) {
                cout << "ERR: " << e.what() << endl;
            }
        }
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}

// system message

void peer_client::send_system_msg(const tcp_endpoint& endpoint, cc_string action)
{
    send_to(endpoint, msg_builder::system(action, "some_id"));
}

void peer_client::send_peer_data_msg(const string&id, const uint16 port)
{
    for (auto peer : __peers_map)
    {
        tcp_endpoint peer_ep(Ipv4Address::from_string(peer.second->get_ip()), peer.second->get_port());
        send_to(peer_ep, msg_builder::peer_data(id, port));
    }
}

// text message

void peer_client::send_text(const tcp_endpoint& endpoint, const string& msg)
{
    send_to(endpoint, msg_builder::text(msg, "some_id", _port));
}

void peer_client::send_to(const tcp_endpoint& ep, const string& m)
{
    try {
        _send_socket.connect(ep);
        _send_socket.write_some(boost::asio::buffer(m));
        _send_socket.close();
    }
    catch (const exception& err)
    {
        string s_ep = endpoint_to_string(ep);
        cerr << s_ep << " " << err.what() << endl;
    }
}
