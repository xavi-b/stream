#include "connection.h"

namespace ST
{

using boost::asio::ip::udp;

Connection::Connection(boost::asio::io_service& ioService) : streamingTimer_(ioService)
{
}

boost::uuids::uuid& Connection::uuid()
{
    return uuid_;
}

bool Connection::streaming() const
{
    return streaming_;
}

void Connection::setStreaming(bool s)
{
    if (s && !streaming_)
        keepAlive();

    streaming_ = s;
}

std::string const& Connection::selectedStream() const
{
    return selectedStream_;
}

void Connection::setSelectedStream(std::string const& stream)
{
    selectedStream_ = stream;
}

Connection::operator std::string() const
{
    std::ostringstream stream;
    stream << "UUID: " << boost::lexical_cast<boost::uuids::uuid>(uuid_);
    stream << "|";
    stream << "Streaming: " << streaming_;
    stream << "|";
    stream << "Selected stream: " << selectedStream_;
    return stream.str();
}

void Connection::keepAlive()
{
    streamingTimer_.expires_from_now(boost::posix_time::seconds(30));
    streamingTimer_.async_wait(
        boost::bind(&Connection::handleKeepAlive, shared_from_this(), boost::asio::placeholders::error));
}

void Connection::handleKeepAlive(const boost::system::error_code& error)
{
    if (!error)
    {
        streaming_ = false;
    }
}

} // namespace ST