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

        if (stringData.rfind("getStreams", 0) == 0)
        {
            spdlog::debug("Received from {}: {}", connection->endpoint().address().to_string(), stringData);

            std::string uuid = stringData.substr(strlen("getStreams"));
            spdlog::debug("getStreams uuid: {}", uuid);
            connection->uuid() = boost::lexical_cast<boost::uuids::uuid>(uuid);

            addConnection(connection);

            boost::shared_ptr<std::string> message = boost::make_shared<std::string>("getStreams");

            for (auto const& c : connections_)
            {
                if (c->uuid() == connection->uuid())
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
            spdlog::debug("Received from {}: {}", connection->endpoint().address().to_string(), stringData);
            // TODO selectStream
        }
        else // stream
        {
            spdlog::debug("Received stream from {}, size {}", connection->endpoint().address().to_string(), bytesTransferred);
            if (error == boost::asio::error::message_size)
            {
                spdlog::warn("boost::asio::error::message_size");
                // TODO
            }

            for (auto const& c : connections_)
            {
                if (c->uuid() == connection->uuid())
                    continue;

                // TODO if selected broadcast
                {
                    // TODO buffer size
                    socket_.async_send_to(boost::asio::buffer(connection->buffer().data(), bytesTransferred),
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
    auto it = std::find_if(connections_.begin(), connections_.end(), [&connection](auto const& c) {
        return c->uuid() == connection->uuid();
    });
    if (it == connections_.end())
    {
        spdlog::debug("addConnection {}", boost::lexical_cast<std::string>(connection->uuid()));
        connections_.insert(connection);
    }
}

void Server::removeConnection(shared_connection connection)
{
    auto it = std::find_if(connections_.begin(), connections_.end(), [&connection](auto const& c) {
        return c->uuid() == connection->uuid();
    });
    if (it != connections_.end())
        connections_.erase(it);
}

} // namespace ST::Network