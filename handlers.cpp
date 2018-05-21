#include "peer_client.h"
#include "msg_builder.h"
#include "msg_formats.h"

peer_client* peer_client::Handler::_peer_client = 0;

void peer_client::handler_sys::handle(cc_string data, size_t size)
{
    message_system* msg_sys = (message_system*)data;

    string action;
    action.assign(msg_sys->_action);
    string peer_id;
    peer_id.assign(msg_sys->_peer_id);

    cout << "Got system message..." << endl;
}

void peer_client::handler_text::handle(cc_string data, size_t size)
{
    message_data* msg_data = (message_data*)data;

    tcp_endpoint remote_endpoint = _peer_client->_recv_socket.remote_endpoint();

    string peer_ip = remote_endpoint.address().to_string();
    uint16 peer_port = msg_data->_port;
    string peer_id = peer_ip + ":" + to_string(peer_port);

    cout << peer_id << " > ";
    cout.write(msg_data->_body.payload, msg_data->_length);
    cout << endl;

    for (auto peer : _peer_client->__peers_map)
    {
        if (peer_id == peer.second->get_id())
            continue;
        string tmp(msg_data->_body.payload);
        tmp.resize(msg_data->_length);
        tcp_endpoint peerEndpoint(Ipv4Address::from_string(peer.second->get_ip()), peer.second->get_port());
        _peer_client->send_to(peerEndpoint, msg_builder::text(tmp, "some_id", _peer_client->_port));
    }
}

void peer_client::handler_peer_data::handle(cc_string data, size_t size)
{
    message_peer_data* msgpeer_data = (message_peer_data*)data;

    tcp_endpoint remote_endpoint = _peer_client->_recv_socket.remote_endpoint();
    uint16 peer_port = msgpeer_data->_port;
    string peer_ip = remote_endpoint.address().to_string();
    string peer_id = peer_ip + ":" + to_string(peer_port);

    peers::iterator it = _peer_client->__peers_map.find(peer_id);

    if (it == _peer_client->__peers_map.end())
    {
        cout << "Found peer: " << peer_id << ", adding to peers map" << endl;

        peer* p = new peer(peer_id);
        p->set_ip(peer_ip);
        p->set_port(peer_port);
        _peer_client->__peers_map[peer_id] = p;

        remote_endpoint.port(peer_port);
        _peer_client->send_to(remote_endpoint, msg_builder::peer_data("some_id", _peer_client->_port));
    }
}