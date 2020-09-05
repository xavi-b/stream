#include "server.h"

#include <spdlog/spdlog.h>

namespace ST::Network
{

using boost::asio::ip::udp;

Server::Server(boost::asio::io_service& ioService, unsigned short portNum)
    : socket_(ioService, udp::endpoint(udp::v4(), portNum)), aliveTimer_(ioService)
{
    receive();
    keepAlive();
}

void Server::receive()
{
    auto connection = std::make_shared<Connection>();

    socket_.async_receive_from(boost::asio::buffer(connection->buffer()),
                               connection->endpoint(),
                               boost::bind(&Server::handleReceive,
                                           this,
                                           connection,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));

    // TODO timer sendStream
}

void Server::keepAlive()
{
    aliveTimer_.expires_from_now(boost::posix_time::seconds(5));
    aliveTimer_.async_wait(boost::bind(&Server::handleKeepAlive, this));
}

void Server::handleKeepAlive()
{
    spdlog::debug("handleKeepAlive");

    for (auto const& c : connections_)
    {
        boost::shared_ptr<std::string> message(new std::string("alive"));

        socket_.async_send_to(boost::asio::buffer(*message),
                              c->endpoint(),
                              boost::bind(&Server::handleSend,
                                          this,
                                          c,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }

    keepAlive();
}

void Server::handleReceive(shared_connection                connection,
                           boost::system::error_code const& error,
                           std::size_t                      bytesTransferred)
{
    spdlog::debug("handleReceive");

    if (!error || error == boost::asio::error::message_size)
    {
        std::string stringData = std::string(connection->buffer().data(), bytesTransferred);

        spdlog::debug("Received from {}: {}", connection->endpoint().address().to_string(), stringData);

        addConnection(connection);

        if (stringData.rfind("getStreams", 0) == 0)
        {
            std::string uuid = stringData.substr(strlen("getStreams"));
            spdlog::debug("getStreams uuid: {}", uuid);
            connection->uuid() = boost::lexical_cast<boost::uuids::uuid>(uuid);

            boost::shared_ptr<std::string> message = boost::make_shared<std::string>("getStreams");

            for (auto const& c : connections_)
            {
                if (c == connection)
                    continue;

                (*message) += boost::lexical_cast<std::string>(c->uuid()) + '|';
            }

            socket_.async_send_to(boost::asio::buffer(*message),
                                  connection->endpoint(),
                                  boost::bind(&Server::handleSend,
                                              this,
                                              connection,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
        }
        else if (stringData.rfind("selectStream", 0) == 0)
        {
            // TODO selectStream
        }
        else // stream
        {
            // TODO if error == boost::asio::error::message_size
            {
            }

            for (auto const& c : connections_)
            {
                if (c == connection)
                    continue;

                // TODO if selected broadcast
                {
                    // TODO buffer size
                    socket_.async_send_to(boost::asio::buffer(connection->buffer()),
                                          c->endpoint(),
                                          boost::bind(&Server::handleSend,
                                                      this,
                                                      c,
                                                      boost::asio::placeholders::error,
                                                      boost::asio::placeholders::bytes_transferred));
                }
            }
        }

        receive();
    }
    else
    {
        removeConnection(connection);
    }
}

void Server::handleSend(shared_connection                connection,
                        const boost::system::error_code& error,
                        std::size_t /*bytesTransferred*/)
{
    spdlog::debug("handleSend");

    if (error)
        removeConnection(connection);
}

void Server::addConnection(shared_connection connection)
{
    if (connections_.find(connection) == connections_.end())
    {
        connections_.insert(connection);
    }
}

void Server::removeConnection(shared_connection connection)
{
    if (connections_.find(connection) == connections_.end())
    {
        connections_.insert(connection);
    }
}

} // namespace ST::Network