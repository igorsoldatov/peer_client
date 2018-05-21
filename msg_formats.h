#ifndef MESSAGE_FORMATS_H
#define MESSAGE_FORMATS_H

#include "utils.h"

enum message_type
{
    M_SYS,
    M_DATA,
    M_PEER_DATA,
    FIRST = M_SYS,
    LAST = M_PEER_DATA
};

#pragma pack(push, 1)

// system message
struct message_system
{
    uint8 _code;
    char _peer_id[PEER_ID_SIZE + 1];
    char _action[1];
};

#define SZ_MESSAGE_SYS (sizeof(uint8) + sizeof(char[PEER_ID_SIZE + 1]))

struct message_peer_data
{
    uint8 _code;
    uint16 _port;
    char _id[PEER_ID_SIZE + 1];
};

#define SZ_MESSAGE_PEERDATA (sizeof(uint8) + sizeof(uint16) + sizeof(uint32) + sizeof(char[PEER_ID_SIZE + 1]))
#define VERSION_SIZE 11

// text message
struct message_data
{
    struct header
    {
        /// Protocol Description
        /// header:
        ///     version
        ///     gid
        ///     id
        ///     check_sum
        ///     check_sum_body // чем отличается от check_sum ?
        ///     size
        /// body:
        ///   payload
        char version[VERSION_SIZE]; ///xxx.xxx.xxx
        uint32 gid;
        uint32 id;
        uint32 check_sum;
        uint32 check_sum_body;
        uint32 size;
    };

    struct body
    {
        char payload[1];
    };

    uint8 _code;
    uint16 _port;
    uint32 _length;
    char _peer_id[PEER_ID_SIZE + 1];
    header _header;
    body _body;
};

#define SZ_MESSAGE_HEADER (sizeof(char[VERSION_SIZE + 1]) + sizeof(uint32) + sizeof(uint32) + sizeof(uint32) + sizeof(uint32) + sizeof(uint32))
#define SZ_MESSAGE_TEXT (sizeof(uint8) + sizeof(uint16) + sizeof(uint32) + sizeof(char[PEER_ID_SIZE + 1]) + SZ_MESSAGE_HEADER)

#pragma pack(pop)

#endif // MESSAGE_FORMATS_H
