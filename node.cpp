#include <cctype>
#include <exception>

#include <boost/bind.hpp>

#include "node.h"
#include "msg_formats.h"
#include "msg_builder.h"

unique_ptr<node> node::_instance;
once_flag node::_onceFlag;

node::node(uint16 port, const string& socket_name, uint8 seed_type, const string& seed) : _tcp_socket(_io_service)
    , _tcp_acceptor(_io_service, tcp_endpoint(Ipv4Address::any(), port))
    , _local_socket(_io_service)
    , _local_acceptor(_io_service)
    , _run_threads(1)
    , _port(port)
{
    ::unlink(socket_name.c_str());
    _local_acceptor = unix_acceptor(_io_service, socket_name);
    cout << "p2p client started" << endl;

    // create handlers
    Handler::setChatInstance(this);
    _handlers.resize(LAST + 1);
    _handlers[M_SYS] = new handler_sys;
    _handlers[M_DATA] = new handler_text;
    _handlers[M_PEER_DATA] = new handler_peer_data;

    _this_peer.set_local_name(socket_name);
    _this_peer.set_port(port);
    _this_peer.set_ip(socket_name);
    _this_peer.set_id(socket_name);

    peer* _seed = new peer(seed_type, seed);
    _peers_map[_seed->get_id()] = _seed;

    start_accept_wan();
    start_accept_local();

    /*
    In this thread io_service is ran
    We can't read user's input without it
    */
    ThreadsMap.insert(pair<string, shared_ptr<Thread>>(BOOST_SERVICE_THREAD,
        shared_ptr<Thread>(new Thread(boost::bind(&node::boost_service_thread, this)))));

}

node::~node()
{
    _tcp_socket.close();
    _io_service.stop();
    
    if (ThreadsMap[BOOST_SERVICE_THREAD].get())
        ThreadsMap[BOOST_SERVICE_THREAD]->join();
    
    ThreadsMap.clear();

    // Delete all handlers

    for (Handlers::iterator it = _handlers.begin(); it != _handlers.end(); ++it)
        delete (*it);

    // Delete all peers

    for (peers::iterator it = _peers_map.begin(); it != _peers_map.end(); ++it)
        delete((*it).second);
}

node& node::get_instance(uint16 port, const string& socket_name, uint8 seed_type, const string& seed)
{
    call_once(_onceFlag, [=] {
        _instance.reset(new node(port, socket_name, seed_type, seed));
    }
    );    
    return *_instance.get();
}

// Thread function: BOOST_SERVICE_THREAD

void node::boost_service_thread()
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
void node::handle_receive_from(const ErrorCode& err, size_t size)
{
    if (err)
        return;

    // parse received packet
    message_system * pmsys = (message_system*)_data.data();
    if (pmsys->_code <= LAST)
    {
        (_handlers[pmsys->_code])->handle(_data.data(), size, node_type::WAN);
    }
}

void node::start_accept_wan()
{
    _tcp_acceptor.async_accept(_tcp_socket,
                               boost::bind(&node::handle_accept_wan, this,
                                           boost::asio::placeholders::error));
}

void node::handle_accept_wan(const boost::system::error_code& error)
{
    if (!error)
    {
        size_t max_length = 1024;
        _tcp_socket.async_read_some(boost::asio::buffer(_data, max_length),
                                boost::bind(&node::handle_receive_from, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        _tcp_socket = tcp_socket(_io_service);
    }

    start_accept_wan();
}

void node::handle_receive_from_local(const ErrorCode& err, size_t size)
{
    if (err)
        return;

    // parse received packet
    message_system * pmsys = (message_system*)_data.data();
    if (pmsys->_code <= LAST)
    {
        (_handlers[pmsys->_code])->handle(_data.data(), size, node_type::LOCAL);
    }
}

void node::start_accept_local()
{
    _local_acceptor.async_accept(_local_socket,
                               boost::bind(&node::handle_accept_local, this,
                                           boost::asio::placeholders::error));
}

void node::handle_accept_local(const boost::system::error_code& error)
{
    if (!error)
    {
        size_t max_length = 1024;
        _local_socket.async_read_some(boost::asio::buffer(_data, max_length),
                                    boost::bind(&node::handle_receive_from_local, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        _local_socket = unix_socket(_io_service);
    }

    start_accept_local();
}

// try to understand user's input
void node::parse_user_input(const string& data)
{
    string tmp(data);

    // parse string
    if (tmp.size() > 1)
    {
        string peerCommand = tmp.substr(0, 5);
        if (peerCommand == "peers")
        {
            for (auto peer : _peers_map)
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
    for (auto peer : _peers_map)
    {
        send_text(*peer.second, tmp);
    }
}

string node::endpoint_to_string(const tcp_endpoint& ep)
{
    string ip_addr = ep.address().to_string();
    uint16 port = ep.port();
    string s_endpoint = ip_addr + ":" + to_string(port);
    return s_endpoint;
}

// Main loop of application

int node::loop()
{
    try
    {
        cout << "Sending broadcast message (type & enter):" << endl;

        send_peer_data_msg(_this_peer.get_local_name(), _port);

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

void node::send_peer_data_msg(const string&id, const uint16 port)
{
    for (auto peer : _peers_map)
    {
        send_to(*peer.second, msg_builder::peer_data(id, port));
    }
}

// text message

void node::send_text(const peer& p, const string& msg)
{
    send_to(p, msg_builder::text(msg, _this_peer.get_id(), _port));
}

void node::send_to(const peer& p, const string& m)
{
    try {
        if (p.get_type() == node_type::LOCAL)
        {
            unix_endpoint ep(p.get_ip().c_str());
            unix_socket send_socket(_io_service);
            send_socket.connect(ep);
            send_socket.write_some(boost::asio::buffer(m));
            send_socket.close();
        }
        else if (p.get_type() == node_type::WAN)
        {
            tcp_endpoint ep(Ipv4Address::from_string(p.get_ip()), p.get_port());
            tcp_socket send_socket(_io_service);
            send_socket.connect(ep);
            send_socket.write_some(boost::asio::buffer(m));
            send_socket.close();
        }
    }
    catch (const exception& err)
    {
        cerr << "Couldn't send message to " << p.get_id() << " " << err.what() << endl;
    }
}
