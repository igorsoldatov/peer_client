#include "peer.h"

peer::peer(const string& peer_id)
{
    if (!peer_id.empty())
        _id = peer_id;
}

peer::peer(const string& peer_ip, uint16 peer_port)
{
    _ip = peer_ip;
    _port = peer_port;
    _id = peer_ip + ":" + to_string(peer_port);
}

void peer::set_id(const string& id)
{
    if (id.empty())
        return;
    else
        _id = id;
}

void peer::set_ip(const string& ip)
{
    if (ip.empty())
        return;
    else
        _ip = ip;
}

void peer::set_port(const uint16 port)
{
    if (port < 0)
        return;
    else
        _port = port;
}