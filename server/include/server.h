#ifndef SERVER_H
#define SERVER_H

#include "connection.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <unordered_set>

namespace ST
{

class Server
{
    using shared_connection = std::shared_ptr<Connection>;

public:
    Server(boost::asio::io_service& io_service, unsigned short port_num);

private:
    void receive();

    void handle_receive(shared_connection                connection,
                        boost::system::error_code const& error,
                        std::size_t                      bytes_transferred);

    void handle_send(shared_connection                connection,
                     boost::system::error_code const& error,
                     std::size_t                      bytes_transferred);

    void addConnection(shared_connection connection);
    void removeConnection(shared_connection connection);

    boost::asio::ip::udp::socket          socket_;
    std::unordered_set<shared_connection> connections_;
};

} // namespace ST

#endif