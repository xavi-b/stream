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
    using receiving_buffer = boost::array<char, 4096>;

public:
    Connection();

    boost::asio::ip::udp::endpoint& endpoint();
    receiving_buffer&               buffer();

private:
    boost::asio::ip::udp::endpoint remoteEndpoint_;
    receiving_buffer               receivingBuffer_;
};

} // namespace ST

#endif