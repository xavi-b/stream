#include "client.h"

#include <iostream>

namespace ST
{

using boost::asio::ip::udp;

void Client::receive()
{
    socket_.async_receive(boost::asio::buffer(network_buffer_),
                          boost::bind(&Client::handle_receive,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void Client::send()
{
    // TODO getStreams
    // TODO selectStream
    // TODO sendStream
}

Client::Client(boost::asio::io_service& io_service, boost::asio::ip::udp::endpoint server_endpoint)
    : socket_(io_service, udp::udp::v4()), server_endpoint_(server_endpoint)
{
    // TODO udp::endpoint receiver_endpoint (boost::asio::ip::address::from_string("127.0.0.1"), 50000);
    receive();
}

void Client::handle_receive(const boost::system::error_code& error, size_t /*number_bytes_read*/)
{
    if (!error)
    {
        // TODO if stream list
        // TODO if stream
        receive();
    }
    else
    {
        std::cerr << error.message();
    }
}

void Client::handle_send(const boost::system::error_code& error, size_t /*number_bytes_read*/)
{
    if (!error)
    {
        // TODO ?
    }
    else
    {
        std::cerr << error.message();
    }
}

} // namespace ST