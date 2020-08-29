#include "client.h"

#include <iostream>

namespace ST::Network
{

using boost::asio::ip::udp;

Client::~Client()
{
    std::cout << "delete\n";
}

void Client::receive()
{
    socket_.async_receive(boost::asio::buffer(network_buffer_),
                          boost::bind(&Client::handle_receive,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
    deadline_timer_.expires_from_now(boost::posix_time::seconds(30));
    deadline_timer_.async_wait(
        boost::bind(&Client::handle_timeout, shared_from_this(), boost::asio::placeholders::error));
}

void Client::send()
{
    // TODO getStreams
    // TODO selectStream
    // TODO sendStream

    boost::array<char, 1> send_buf = {0};
    socket_.async_send_to(boost::asio::buffer(send_buf),
                          server_endpoint_,
                          boost::bind(&Client::handle_send,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void Client::getStreams()
{
    // TODO getStreams

    boost::array<char, 1> send_buf = {0};
    socket_.async_send_to(boost::asio::buffer(send_buf),
                          server_endpoint_,
                          boost::bind(&Client::handle_send,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

bool Client::isConnected() const
{
    return connected_.load();
}

void Client::close()
{
    socket_.close();
}

Client::Client(boost::asio::io_service& io_service, boost::asio::ip::udp::endpoint server_endpoint)
    : socket_(io_service, udp::udp::v4()), server_endpoint_(server_endpoint), deadline_timer_(io_service),
      connected_(false)
{
}

void Client::handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
{
    std::cout << "handle_receive\n";
    if (!error)
    {
        std::cout << "Received from " << server_endpoint_.address().to_string() << ": "
                  << std::string(network_buffer_.data(), bytes_transferred) << "\n";
        connected_.store(true);

        // TODO if stream list
        // TODO if stream
        receive();
    }
    else
    {
        std::cerr << error.message() << "\n";
    }
}

void Client::handle_send(const boost::system::error_code& error, size_t /*bytes_transferred*/)
{
    std::cout << "handle_send\n";
    if (!error)
    {
        // TODO ?
    }
    else
    {
        std::cerr << error.message() << "\n";
    }
}

void Client::handle_timeout(const boost::system::error_code& error)
{
    std::cout << "handle_timeout\n";
    if (!error)
    {
        close();
    }
}

} // namespace ST::Network