#ifndef CLIENT_H
#define CLIENT_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>

namespace ST
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

private:
    Client(boost::asio::io_service& io_service, boost::asio::ip::udp::endpoint server_endpoint);

    void handle_receive(const boost::system::error_code& error, size_t number_bytes_read);
    void handle_send(const boost::system::error_code& error, size_t number_bytes_read);

    boost::asio::ip::udp::socket   socket_;
    boost::asio::ip::udp::endpoint server_endpoint_;
    boost::array<char, 128>        network_buffer_;
};

} // namespace ST

#endif