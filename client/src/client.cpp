#include "client.h"

#include <spdlog/spdlog.h>

namespace ST::Network
{

using boost::asio::ip::udp;

Client::~Client()
{
    spdlog::debug("~Client");
}

void Client::receive()
{
    boost::asio::streambuf::mutable_buffers_type mutableBuffer = networkBuffer_.prepare(65536);

    socket_.async_receive(boost::asio::buffer(mutableBuffer),
                          boost::bind(&Client::handleReceive,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
    deadlineTimer_.expires_from_now(boost::posix_time::seconds(30));
    deadlineTimer_.async_wait(
        boost::bind(&Client::handleTimeout, shared_from_this(), boost::asio::placeholders::error));
}

void Client::selectStream(std::string const& stream)
{
    boost::shared_ptr<std::string> message = boost::make_shared<std::string>("selectStream" + stream);

    socket_.async_send_to(boost::asio::buffer(*message),
                          serverEndpoint_,
                          boost::bind(&Client::handleSend,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void Client::sendStream(unsigned char* data, size_t size)
{
    spdlog::debug("sendStream, size {}", size);
    socket_.async_send_to(boost::asio::buffer(data, size),
                          serverEndpoint_,
                          boost::bind(&Client::handleSend,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void Client::getStreams()
{
    boost::shared_ptr<std::string> message =
        boost::make_shared<std::string>("getStreams" + boost::lexical_cast<std::string>(uuid_));

    socket_.async_send_to(boost::asio::buffer(*message),
                          serverEndpoint_,
                          boost::bind(&Client::handleSend,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

bool Client::isConnected() const
{
    return connected_.load();
}

void Client::close()
{
    socket_.close();
}

void Client::setOnStreamReceived(StreamReceivedCallback f)
{
    onStreamReceived_ = f;
}

void Client::setOnGetStreamsReceived(GetStreamsReceivedCallback f)
{
    onGetStreamsReceived_ = f;
}

Client::Client(boost::asio::io_service& ioService, boost::asio::ip::udp::endpoint serverEndpoint)
    : uuid_(boost::uuids::random_generator()()), socket_(ioService, udp::udp::v4()), serverEndpoint_(serverEndpoint),
      deadlineTimer_(ioService), connected_(false)
{
}

void Client::handleReceive(const boost::system::error_code& error, size_t bytesTransferred)
{
    spdlog::debug("handleReceive");

    if (!error || error == boost::asio::error::message_size)
    {
        std::string stringData = std::string((const char*)networkBuffer_.data().data(), bytesTransferred);

        connected_.store(true);

        if (stringData.rfind("alive", 0) == 0)
        {
            spdlog::debug("Received from {}: {}", serverEndpoint_.address().to_string(), stringData);
        }
        else if (stringData.rfind("getStreams", 0) == 0)
        {
            spdlog::debug("Received from {}: {}", serverEndpoint_.address().to_string(), stringData);

            std::string str = stringData.substr(strlen("getStreams"));
            spdlog::debug("getStreams uuids: '{}'", str);

            std::vector<std::string> uuids;
            boost::split(uuids, str, [](char c) {
                return c == '|';
            });

            uuids.erase(std::remove_if(uuids.begin(), uuids.end(), [](std::string const& e) {
                return e.empty();
            }));

            if (onGetStreamsReceived_)
                onGetStreamsReceived_(uuids);
        }
        else // stream
        {
            spdlog::debug("Received stream from {}, size: {}", serverEndpoint_.address().to_string(), bytesTransferred);
            if (error == boost::asio::error::message_size)
            {
                spdlog::warn("boost::asio::error::message_size");
                // TODO
            }

            StreamData data((const unsigned char*)networkBuffer_.data().data(),
                            ((const unsigned char*)networkBuffer_.data().data()) + bytesTransferred);
            if (onStreamReceived_)
                onStreamReceived_(data);
        }

        networkBuffer_.consume(bytesTransferred);
        receive();
    }
    else
    {
        spdlog::warn(error.message());
    }
}

void Client::handleSend(const boost::system::error_code& error, size_t bytesTransferred)
{
    spdlog::debug("handleSend {}", bytesTransferred);

    if (error)
    {
        spdlog::warn(error.message());
    }
}

void Client::handleTimeout(const boost::system::error_code& error)
{
    spdlog::debug("handleTimeout");

    if (!error)
    {
        close();
    }
}

} // namespace ST::Network