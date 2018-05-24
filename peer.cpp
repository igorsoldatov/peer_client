#include "peer.h"

peer::peer()
{
}

peer::peer(const string& peer_id)
{
    if (!peer_id.empty())
        _id = peer_id;
}

peer::peer(uint8 seed_type, const string& seed)
{
    _type = (node_type)seed_type;
    if (seed_type == node_type::LOCAL)
    {
        _ip = "/tmp/" + seed;
        _port = 0;
        _id = _ip;
    }
    else if (seed_type == node_type::WAN)
    {
        auto ip_port = parse_ip_port(seed);
        _ip = ip_port.first;
        _port = ip_port.second;
        _id = seed;
    } else{
        cerr << "unknown seed type" << endl;
    }
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

void peer::set_type(node_type type)
{
    _type = type;
}

void peer::set_local_name(const string &name)
{
    if (name.empty())
        return;
    else
        _local_name = name;
}

pair<string, uint16> peer::parse_ip_port(const string& ip_port)
{
    string ip;
    string port;

    // try to find first space-symbol
    wstring::size_type t = ip_port.find_first_of(":");

    if (t == wstring::npos)
        return pair<string, uint16>("", 0);

    ip = string(ip_port.begin(), ip_port.begin() + t);
    port = ip_port.substr(t + 1);

    return pair<string, uint16>(ip, boost::lexical_cast<int>(port));
}