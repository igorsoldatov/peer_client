#include "peer_client.h"
#include <boost/lexical_cast.hpp>

int main(int argc, char ** argv)
{
    if (argc > 3) {
        try {
            Port = boost::lexical_cast<int>(argv[1]);
            Seed = argv[2];
            SeedPort = boost::lexical_cast<int>(argv[3]);
        }
        catch (const boost::bad_lexical_cast& err)
        {
            cerr << err.what();
        }
    }

    peer_client* chatInstance = &peer_client::get_instance(Port, Seed, SeedPort);

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
