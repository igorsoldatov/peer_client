#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "utils.h"
#include "peer.h"

#include <mutex>

class peer_client
{
public:
    virtual ~peer_client();
    static peer_client& get_instance(uint16 port, string seed, uint16 seedport);

    int loop();

private:
    // singleton class
    static unique_ptr<peer_client> _instance;
    static once_flag _onceFlag;
    peer_client(uint16 port, string seed, uint16 seedport);
    peer_client(const peer_client& src);
    peer_client& operator=(const peer_client& rval);

    typedef map<string, peer*> peers;

    // Handlers

    class Handler
    {
    public:
        virtual ~Handler() {}
        virtual void handle(cc_string data, size_t size) = 0;
        static void setChatInstance(peer_client* peer_client) { _peer_client = peer_client; }
    protected:
        static peer_client* _peer_client;
    };
    typedef vector< Handler* > Handlers;

    class handler_sys : public Handler {
    public:
        void handle(cc_string data, size_t size);
    };

    class handler_text : public Handler {
    public:
        void handle(cc_string data, size_t size);
    };

    class handler_peer_data : public Handler {
    public:
        void handle(cc_string data, size_t size);
    };

    boost::asio::io_service _io_service;

    tcp_socket _send_socket;
    tcp_socket _recv_socket;
    tcp_acceptor _acceptor;
    uint16 _port;
    volatile uint8 _run_threads;
    boost::array<char, 64 * 1024> _data;
    Handlers _handlers;
    peers __peers_map;

    void boost_service_thread();

    // async
    void start_accept();
    void handle_receive_from(const ErrorCode& error, size_t bytes_recvd);
    void handle_accept(const boost::system::error_code& error);

    // parsing

    string endpoint_to_string(const tcp_endpoint& ep);
    void parse_user_input(const string& data);

    // senders

    void send_system_msg(const tcp_endpoint& endpoint, cc_string action);
    void send_peer_data_msg(const string&id, const uint16 port);
    void send_text(const tcp_endpoint& endpoint, const string& message);
    void send_to(const tcp_endpoint& endpoint, const string& m);
};

#endif // CHAT_CLIENT_H
