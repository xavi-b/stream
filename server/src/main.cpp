#include <iostream>
#include "server.h"

int main(int, char **)
{
    try
    {
        boost::asio::io_service ioService;
        ST::Network::Server server(ioService, 50000);
        ioService.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}