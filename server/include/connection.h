#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <unordered_set>

namespace ST
{

class Connection
{
    using receiving_buffer = boost::array<char, 4096>;

public:
    Connection();

    boost::uuids::uuid&             uuid();
    boost::asio::ip::udp::endpoint& endpoint();
    receiving_buffer&               buffer();

private:
    boost::uuids::uuid             uuid_;
    boost::asio::ip::udp::endpoint remoteEndpoint_;
    receiving_buffer               receivingBuffer_;
};

} // namespace ST

#endif