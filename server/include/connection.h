#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <unordered_set>

namespace ST
{

class Connection
{
    using receiving_buffer = boost::array<char, 1>;

public:
    Connection();

    boost::asio::ip::udp::endpoint& endpoint();
    receiving_buffer&               buffer();

private:
    boost::asio::ip::udp::endpoint remote_endpoint_;
    receiving_buffer               receiving_buffer_;
};

} // namespace ST

#endif