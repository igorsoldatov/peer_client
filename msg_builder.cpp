#include "msg_builder.h"
#include "msg_formats.h"
#include "utils.h"

std::string msg_builder::system(cc_string action, const string& peer_id)
{
    string raw;
    size_t rawLen = strlen(action) + 1;
    raw.resize(rawLen + SZ_MESSAGE_SYS, 0);

    message_system* msg_sys = (message_system*)raw.data();

    msg_sys->_code = M_SYS;
    memcpy(msg_sys->_action, action, rawLen);
    memcpy(msg_sys->_peer_id, peer_id.c_str(), PEER_ID_SIZE + 1);

    return raw;
}

std::string msg_builder::peer_data(const string& id, const uint16 port)
{
    string raw;
    raw.resize(SZ_MESSAGE_PEERDATA, 0);

    message_peer_data* msgpeer_data = (message_peer_data*)raw.data();

    msgpeer_data->_code = M_PEER_DATA;
    msgpeer_data->_port = port;
    memcpy(msgpeer_data->_id, id.c_str(), PEER_ID_SIZE + 1);

    return raw;
}

std::string msg_builder::text(const string& msg, const string& peer_id, const uint16 port)
{
    string ver = "001.000.000";
    string raw;
    size_t rawLen = msg.length() * sizeof(char);
    raw.resize(rawLen + SZ_MESSAGE_TEXT, 0);

    message_data* msg_data = (message_data*)raw.data();

    // header
    memcpy(msg_data->_header.version, ver.data(), VERSION_SIZE);
    msg_data->_header.gid = 111111;
    msg_data->_header.id = 111111;
    msg_data->_header.check_sum = 111111;
    msg_data->_header.check_sum_body = 111111;
    msg_data->_header.size = msg.length();

    // body
    memcpy(msg_data->_body.payload, msg.data(), rawLen);

    msg_data->_code = M_DATA;
    msg_data->_port = port;
    msg_data->_length = msg.length();
    memcpy(msg_data->_peer_id, peer_id.c_str(), PEER_ID_SIZE + 1);

    return raw;
}