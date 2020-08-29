#include "server.h"

namespace ST
{

using boost::asio::ip::udp;

Server::Server(boost::asio::io_service& io_service, unsigned short port_num)
    : socket_(io_service, udp::endpoint(udp::v4(), port_num))
{
    receive();
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

void Server::handle_receive(shared_connection                connection,
                            boost::system::error_code const& error,
                            std::size_t /*bytes_transferred*/)
{
    if (!error || error == boost::asio::error::message_size)
    {
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

} // namespace ST