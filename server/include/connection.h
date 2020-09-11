#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <unordered_set>

namespace ST
{

class Connection : public std::enable_shared_from_this<Connection>
{

public:
    Connection(boost::asio::io_service& ioService);

    boost::uuids::uuid& uuid();
    bool                streaming() const;
    void                setStreaming(bool s);
    std::string const&  selectedStream() const;
    void                setSelectedStream(std::string const& stream);

    explicit operator std::string() const;

private:
    void keepAlive();
    void handleKeepAlive(const boost::system::error_code& error);

    boost::uuids::uuid          uuid_;
    bool                        streaming_ = false;
    boost::asio::deadline_timer streamingTimer_;
    std::string                 selectedStream_;
};

} // namespace ST

#endif