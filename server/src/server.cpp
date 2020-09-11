#include "server.h"

#include <spdlog/spdlog.h>

namespace ST::Network
{

using boost::asio::ip::udp;

Server::Server(boost::asio::io_service& ioService, unsigned short portNum)
    : ioService_(ioService), socket_(ioService, udp::endpoint(udp::v4(), portNum)), aliveTimer_(ioService)
{
    receive();
    keepAlive();
}

void Server::receive()
{
    auto endpoint = std::make_shared<boost::asio::ip::udp::endpoint>();

    boost::asio::streambuf::mutable_buffers_type mutableBuffer = receivingBuffer_.prepare(65536);

    socket_.async_receive_from(boost::asio::buffer(mutableBuffer),
                               *endpoint,
                               boost::bind(&Server::handleReceive,
                                           this,
                                           endpoint,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
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
                              c.first,
                              boost::bind(&Server::handleSend,
                                          this,
                                          std::make_shared<boost::asio::ip::udp::endpoint>(c.first),
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }

    keepAlive();
}

void Server::handleReceive(shared_endpoint                  endpoint,
                           boost::system::error_code const& error,
                           std::size_t                      bytesTransferred)
{
    spdlog::debug("handleReceive");

    if (!error || error == boost::asio::error::message_size)
    {
        shared_connection connection = addConnection(endpoint);

        std::string stringData = std::string((const char*)receivingBuffer_.data().data(), bytesTransferred);

        if (stringData.rfind("getStreams", 0) == 0)
        {
            spdlog::debug("Received from {}: {}", endpoint->address().to_string(), stringData);

            std::string uuid = stringData.substr(strlen("getStreams"));
            spdlog::debug("getStreams uuid: {}", uuid);
            connection->uuid() = boost::lexical_cast<boost::uuids::uuid>(uuid);

            boost::shared_ptr<std::string> message = boost::make_shared<std::string>("getStreams");

            for (auto const& c : connections_)
            {
                spdlog::debug("stream: {}", (std::string)(*c.second));

                if (c.first == *endpoint)
                    continue;

                if (!c.second->streaming())
                    continue;

                (*message) += boost::lexical_cast<std::string>(c.second->uuid()) + '|';
            }

            socket_.async_send_to(boost::asio::buffer(*message),
                                  *endpoint,
                                  boost::bind(&Server::handleSend,
                                              this,
                                              endpoint,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
        }
        else if (stringData.rfind("selectStream", 0) == 0)
        {
            spdlog::debug("Received from {}: {}", endpoint->address().to_string(), stringData);
            std::string stream = stringData.substr(strlen("selectStream"));

            connection->setSelectedStream(stream);
        }
        else // stream
        {
            spdlog::debug("Received stream from {}, size {}", endpoint->address().to_string(), bytesTransferred);

            connection->setStreaming(true);

            if (error == boost::asio::error::message_size)
            {
                spdlog::warn("boost::asio::error::message_size");
                // TODO
            }

            for (auto const& c : connections_)
            {
                spdlog::debug("stream: {}", (std::string)(*c.second));

                if (c.first == *endpoint)
                    continue;

                if (boost::lexical_cast<std::string>(connection->uuid()) == c.second->selectedStream())
                {
                    // TODO buffer size
                    socket_.async_send_to(boost::asio::buffer(receivingBuffer_.data().data(), bytesTransferred),
                                          c.first,
                                          boost::bind(&Server::handleSend,
                                                      this,
                                                      std::make_shared<boost::asio::ip::udp::endpoint>(c.first),
                                                      boost::asio::placeholders::error,
                                                      boost::asio::placeholders::bytes_transferred));
                }
            }
        }

        receivingBuffer_.consume(bytesTransferred);
        receive();
    }
    else
    {
        removeConnection(endpoint);
    }
}

void Server::handleSend(shared_endpoint endpoint, const boost::system::error_code& error, std::size_t bytesTransferred)
{
    spdlog::debug("handleSend {}", bytesTransferred);

    if (error)
        removeConnection(endpoint);
}

Server::shared_connection Server::addConnection(shared_endpoint endpoint)
{
    auto ret = connections_.insert({*endpoint, std::make_shared<Connection>(ioService_)});
    return ret.first->second;
}

void Server::removeConnection(shared_endpoint endpoint)
{
    connections_.erase(*endpoint);
}

} // namespace ST::Network