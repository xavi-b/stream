#ifndef SERVER_H
#define SERVER_H

#include "connection.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <unordered_set>

namespace ST::Network
{

class Server
{
public:
    using receiving_buffer  = boost::asio::streambuf;
    using shared_connection = std::shared_ptr<Connection>;
    using shared_endpoint   = std::shared_ptr<boost::asio::ip::udp::endpoint>;

    Server(boost::asio::io_service& ioService, unsigned short portNum);

private:
    void receive();
    void keepAlive();

    void handleKeepAlive();

    void handleReceive(shared_endpoint endpoint, boost::system::error_code const& error, std::size_t bytesTransferred);

    void handleSend(shared_endpoint endpoint, boost::system::error_code const& error, std::size_t bytesTransferred);

    shared_connection addConnection(shared_endpoint endpoint);
    void              removeConnection(shared_endpoint endpoint);

    boost::asio::io_service&                                    ioService_;
    boost::asio::ip::udp::socket                                socket_;
    boost::asio::deadline_timer                                 aliveTimer_;
    std::map<boost::asio::ip::udp::endpoint, shared_connection> connections_;
    receiving_buffer                                            receivingBuffer_;
};

} // namespace ST::Network

#endif