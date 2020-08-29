#include <iostream>
#include "server.h"

int main(int, char **)
{
    try
    {
        boost::asio::io_service io_service;
        ST::Server server(io_service, 50000);
        io_service.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}