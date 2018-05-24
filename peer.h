#ifndef PEER_H
#define PEER_H

#include <boost/lexical_cast.hpp>
#include "utils.h"

enum node_type
{
    LOCAL,
    WAN
};

class peer
{
public:
    peer(const string& peer_id);
    peer(uint8 seed_type, const string& seed);
    peer();
    ~peer() { }

    void set_id(const string& id);
    void set_ip(const string& ip);
    void set_port(const uint16 port);
    void set_type(node_type type);
    void set_local_name(const string& name);

    const string& get_id() const { return _id; }
    const string& get_ip() const { return _ip; }
    uint16 get_port() const { return _port; }
    node_type get_type() const { return _type; }
    string get_local_name() const { return _local_name; }

    pair<string, uint16> parse_ip_port(const string& ip_port);
private:
    node_type _type;
    string _ip;
    uint16 _port;
    string _local_name;
    string _id;
};

#endif // PEER_H
