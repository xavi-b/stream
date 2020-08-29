#include "server.h"

#include <iostream>

namespace ST::Network
{

using boost::asio::ip::udp;

Server::Server(boost::asio::io_service& io_service, unsigned short port_num)
    : socket_(io_service, udp::endpoint(udp::v4(), port_num)), alive_timer_(io_service)
{
    receive();
    keepAlive();
}

void Server::receive()
{
    auto connection = std::make_shared<Connection>();

    socket_.async_receive_from(boost::asio::buffer(connection->buffer()),
                               connection->endpoint(),
                               boost::bind(&Server::handle_receive,
                                           this,
                                           connection,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
}

void Server::keepAlive()
{
    alive_timer_.expires_from_now(boost::posix_time::seconds(5));
    alive_timer_.async_wait(boost::bind(&Server::handle_keepAlive, this));
}

void Server::handle_keepAlive()
{
    std::cout << "handle_keepAlive\n";
    for (auto const& c : connections_)
    {
        boost::shared_ptr<std::string> message(new std::string("alive"));

        socket_.async_send_to(boost::asio::buffer(*message),
                              c->endpoint(),
                              boost::bind(&Server::handle_send,
                                          this,
                                          c,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }

    keepAlive();
}

void Server::handle_receive(shared_connection                connection,
                            boost::system::error_code const& error,
                            std::size_t                      bytes_transferred)
{
    std::cout << "handle_receive\n";
    if (!error || error == boost::asio::error::message_size)
    {
        std::cout << "Received from " << connection->endpoint().address().to_string() << ": "
                  << std::string(connection->buffer().data(), bytes_transferred) << "\n";

        addConnection(connection);

        // TODO if getStreams
        {
            boost::shared_ptr<std::string> message(new std::string("test"));

            socket_.async_send_to(boost::asio::buffer(*message),
                                  connection->endpoint(),
                                  boost::bind(&Server::handle_send,
                                              this,
                                              connection,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
        }

        // TODO if selectStream
        {
            // TODO select stream
        }

        // TODO if sendStream
        {
            for (auto const& c : connections_)
            {
                if (c == connection)
                    continue;

                // TODO if selected broadcast
                {
                    // TODO broadcast received data
                    boost::shared_ptr<std::string> message(new std::string("broadcast selected channel"));

                    socket_.async_send_to(boost::asio::buffer(*message),
                                          c->endpoint(),
                                          boost::bind(&Server::handle_send,
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

void Server::handle_send(shared_connection                connection,
                         const boost::system::error_code& error,
                         std::size_t /*bytes_transferred*/)
{
    std::cout << "handle_send\n";
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