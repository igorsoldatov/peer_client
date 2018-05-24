#include "node.h"
#include "msg_builder.h"
#include "msg_formats.h"

node* node::Handler::_node = 0;

void node::handler_sys::handle(cc_string data, size_t size, node_type ntype)
{
    message_system* msg_sys = (message_system*)data;

    string action;
    action.assign(msg_sys->_action);
    string peer_id;
    peer_id.assign(msg_sys->_peer_id);

    cout << "Got system message..." << endl;
}

void node::handler_text::handle(cc_string data, size_t size, node_type ntype)
{
    message_data* msg_data = (message_data*)data;

    string peer_id;
    peer_id.assign(msg_data->_peer_id);

    if (ntype == node_type::WAN) {
        tcp_endpoint remote_endpoint = _node->_tcp_socket.remote_endpoint();

        string peer_ip = remote_endpoint.address().to_string();
        uint16 peer_port = msg_data->_port;
        peer_id = peer_ip + ":" + to_string(peer_port);
    }

    cout << peer_id << " > ";
    cout.write(msg_data->_body.payload, msg_data->_length);
    cout << endl;

    for (auto peer : _node->_peers_map)
    {
        if (peer_id == peer.second->get_id())
            continue;
        string tmp(msg_data->_body.payload);
        tmp.resize(msg_data->_length);

        _node->send_to(*peer.second, msg_builder::text(tmp, _node->_this_peer.get_id(), _node->_port));
    }
}

void node::handler_peer_data::handle(cc_string data, size_t size, node_type ntype)
{
    message_peer_data* msg_peer_data = (message_peer_data*)data;

    string peer_id;
    peer_id.assign(msg_peer_data->_id);
    string peer_ip;
    peer_ip.assign(msg_peer_data->_id);
    uint16 peer_port = 0;

    if (ntype == node_type::WAN) {
        tcp_endpoint remote_endpoint = _node->_tcp_socket.remote_endpoint();
        peer_port = msg_peer_data->_port;
        peer_ip = remote_endpoint.address().to_string();
        peer_id = peer_ip + ":" + to_string(peer_port);
    }

    peers::iterator it = _node->_peers_map.find(peer_id);

    if (it == _node->_peers_map.end())
    {
        cout << "Found peer: " << peer_id << ", adding to peers map" << endl;

        peer* p = new peer(peer_id);
        p->set_ip(peer_ip);
        p->set_port(peer_port);
        p->set_type(ntype);
        _node->_peers_map[peer_id] = p;

        _node->send_to(*p, msg_builder::peer_data(_node->_this_peer.get_id(), _node->_port));
    }
}