#include "server.h"

#include <exception>
#include <spdlog/spdlog.h>

int main(int, char **)
{
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("DEBUG");
#endif

    try
    {
        boost::asio::io_service ioService;
        ST::Network::Server server(ioService, 50000);
        ioService.run();
    }
    catch (std::exception &e)
    {
        spdlog::critical(e.what());
    }

    return 0;
}