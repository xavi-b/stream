#include "connection.h"

namespace ST
{

using boost::asio::ip::udp;

Connection::Connection()
{
}

boost::asio::ip::udp::endpoint& Connection::endpoint()
{
    return remote_endpoint_;
}

Connection::receiving_buffer& Connection::buffer()
{
    return receiving_buffer_;
}

} // namespace ST