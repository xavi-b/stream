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

public:
    Connection();

    boost::uuids::uuid&             uuid();
    boost::asio::ip::udp::endpoint& endpoint();

private:
    boost::uuids::uuid             uuid_;
    boost::asio::ip::udp::endpoint remoteEndpoint_;
};

} // namespace ST

#endif