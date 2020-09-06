#include "connection.h"

namespace ST
{

using boost::asio::ip::udp;

Connection::Connection()
{
}

boost::uuids::uuid& Connection::uuid()
{
    return uuid_;
}

boost::asio::ip::udp::endpoint& Connection::endpoint()
{
    return remoteEndpoint_;
}

} // namespace ST