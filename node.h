#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "utils.h"
#include "peer.h"

#include <mutex>

class node
{
public:
    virtual ~node();
    static node& get_instance(uint16 port, const string& socket_name, uint8 seed_type, const string& seed);

    int loop();

private:
    // singleton class
    static unique_ptr<node> _instance;
    static once_flag _onceFlag;
    node(uint16 port, const string& socket_name, uint8 seed_type, const string& seed);
    node(const node& src);
    node& operator=(const node& rval);

    typedef map<string, peer*> peers;

    // Handlers

    class Handler
    {
    public:
        virtual ~Handler() {}
        virtual void handle(cc_string data, size_t size, node_type ntype) = 0;
        static void setChatInstance(node* node) { _node = node; }
    protected:
        static node* _node;
    };
    typedef vector< Handler* > Handlers;

    class handler_sys : public Handler {
    public:
        void handle(cc_string data, size_t size, node_type ntype);
    };

    class handler_text : public Handler {
    public:
        void handle(cc_string data, size_t size, node_type ntype);
    };

    class handler_peer_data : public Handler {
    public:
        void handle(cc_string data, size_t size, node_type ntype);
    };

    boost::asio::io_service _io_service;

    tcp_socket _tcp_socket;
    tcp_acceptor _tcp_acceptor;
    unix_socket _local_socket;
    unix_acceptor _local_acceptor;
    uint16 _port;
    volatile uint8 _run_threads;
    boost::array<char, 64 * 1024> _data;
    Handlers _handlers;
    peer _this_peer;
    peers _peers_map;

    void boost_service_thread();

    // async
    void handle_receive_from(const ErrorCode& error, size_t bytes_recvd);
    void start_accept_wan();
    void handle_accept_wan(const boost::system::error_code& error);
    void handle_receive_from_local(const ErrorCode& error, size_t bytes_recvd);
    void start_accept_local();
    void handle_accept_local(const boost::system::error_code& error);

    // parsing

    string endpoint_to_string(const tcp_endpoint& ep);
    void parse_user_input(const string& data);

    // senders

    void send_peer_data_msg(const string&id, const uint16 port);
    void send_text(const peer& p, const string& message);
    void send_to(const peer& p, const string& m);
};

#endif // CHAT_CLIENT_H
