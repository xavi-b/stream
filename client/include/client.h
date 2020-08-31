#ifndef CLIENT_H
#define CLIENT_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>

namespace ST::Network
{

class Client : public std::enable_shared_from_this<Client>
{
    using shared_client = std::shared_ptr<Client>;

public:
    ~Client();
    static shared_client create(boost::asio::io_service& ioService, boost::asio::ip::udp::endpoint serverEndpoint)
    {
        return std::shared_ptr<Client>(new Client(ioService, serverEndpoint));
    }

    void receive();
    void send();
    void getStreams();

    bool isConnected() const;

    void close();

private:
    Client(boost::asio::io_service& ioService, boost::asio::ip::udp::endpoint serverEndpoint);

    void handleReceive(const boost::system::error_code& error, size_t bytesTransferred);
    void handleSend(const boost::system::error_code& error, size_t bytesTransferred);
    void handleTimeout(const boost::system::error_code& error);

    boost::asio::ip::udp::socket   socket_;
    boost::asio::ip::udp::endpoint serverEndpoint_;
    boost::asio::deadline_timer    deadlineTimer_;
    boost::array<char, 128>        networkBuffer_;

    std::atomic<bool> connected_;
};

} // namespace ST::Network

#endif