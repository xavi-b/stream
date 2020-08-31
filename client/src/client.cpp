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
    socket_.async_receive(boost::asio::buffer(networkBuffer_),
                          boost::bind(&Client::handleReceive,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
    deadlineTimer_.expires_from_now(boost::posix_time::seconds(30));
    deadlineTimer_.async_wait(
        boost::bind(&Client::handleTimeout, shared_from_this(), boost::asio::placeholders::error));
}

void Client::send()
{
    // TODO getStreams
    // TODO selectStream
    // TODO sendStream

    boost::array<char, 1> sendBuffer = {0};
    socket_.async_send_to(boost::asio::buffer(sendBuffer),
                          serverEndpoint_,
                          boost::bind(&Client::handleSend,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void Client::getStreams()
{
    // TODO getStreams

    boost::array<char, 1> sendBuffer = {0};
    socket_.async_send_to(boost::asio::buffer(sendBuffer),
                          serverEndpoint_,
                          boost::bind(&Client::handleSend,
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

Client::Client(boost::asio::io_service& ioService, boost::asio::ip::udp::endpoint serverEndpoint)
    : socket_(ioService, udp::udp::v4()), serverEndpoint_(serverEndpoint), deadlineTimer_(ioService), connected_(false)
{
}

void Client::handleReceive(const boost::system::error_code& error, size_t bytesTransferred)
{
    std::cout << "handleReceive\n";
    if (!error)
    {
        std::cout << "Received from " << serverEndpoint_.address().to_string() << ": "
                  << std::string(networkBuffer_.data(), bytesTransferred) << "\n";
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

void Client::handleSend(const boost::system::error_code& error, size_t /*bytesTransferred*/)
{
    std::cout << "handleSend\n";
    if (!error)
    {
        // TODO ?
    }
    else
    {
        std::cerr << error.message() << "\n";
    }
}

void Client::handleTimeout(const boost::system::error_code& error)
{
    std::cout << "handleTimeout\n";
    if (!error)
    {
        close();
    }
}

} // namespace ST::Network