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
    static shared_client create(boost::asio::io_service& ios, boost::asio::ip::udp::endpoint server_endpoint)
    {
        return std::shared_ptr<Client>(new Client(ios, server_endpoint));
    }

    void receive();
    void send();
    void getStreams();

    bool isConnected() const;

    void close();

private:
    Client(boost::asio::io_service& io_service, boost::asio::ip::udp::endpoint server_endpoint);

    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_send(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_timeout();

    boost::asio::ip::udp::socket   socket_;
    boost::asio::ip::udp::endpoint server_endpoint_;
    boost::asio::deadline_timer    deadline_timer_;
    boost::array<char, 128>        network_buffer_;

    std::atomic<bool> connected_;
};

} // namespace ST::Network

#endif