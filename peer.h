#ifndef PEER_H
#define PEER_H

#include "utils.h"

class peer
{
public:
    peer(const string& peer_id);
    peer(const string& peer_ip, uint16 peer_port);
    peer();
    ~peer() { }

    void set_id(const string& id);
    void set_ip(const string& ip);
    void set_port(const uint16 port);

    const string& get_id() const { return _id; }
    const string& get_ip() const { return _ip; }
    uint16 get_port() const { return _port; }
private:
    string _ip;
    uint16 _port;
    string _id;
};

#endif // PEER_H
