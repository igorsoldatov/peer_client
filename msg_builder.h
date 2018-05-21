#ifndef MESSAGE_BUILDER_H
#define MESSAGE_BUILDER_H

#include "utils.h"

class msg_builder
{
public:
    static string system(cc_string action, const string& peer_id);
    static string peer_data(const string& id, const uint16 port);
    static string text(const string& msg, const string& peer_id, const uint16 port);
};

#endif // MESSAGE_BUILDER_H
