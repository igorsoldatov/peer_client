#include "node.h"
#include <boost/lexical_cast.hpp>

int main(int argc, char ** argv)
{
    uint16 port = 8080;
    string socket_name = "this_unix_socket";
    uint8 seed_type = 0;
    string seed = "seed_unix_socket";

    if (argc > 4) {
        try {
            port = boost::lexical_cast<int>(argv[1]);
            socket_name = argv[2];
            seed_type = boost::lexical_cast<int>(argv[3]); // 0 - local socket, 1 - WAN
            seed = argv[4]; // local socket name or ip address with port
        }
        catch (const boost::bad_lexical_cast& err)
        {
            cerr << err.what();
        }
    }

    socket_name = "/tmp/" + socket_name;
    node* chatInstance = &node::get_instance(port, socket_name, seed_type, seed);

    try
    {
        return chatInstance->loop();
    }
    catch (logic_error& err)
    {
        cerr << err.what() << endl;
        return 1;
    }
}
