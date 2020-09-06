#ifndef CLIENT_H
#define CLIENT_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <memory>

namespace ST::Network
{

class Client : public std::enable_shared_from_this<Client>
{
public:
    using shared_client              = std::shared_ptr<Client>;
    using StreamData                 = std::vector<char>;
    using StreamReceivedCallback     = std::function<void(StreamData)>;
    using GetStreamsReceivedCallback = std::function<void(std::vector<std::string> const&)>;

    ~Client();
    static shared_client create(boost::asio::io_service& ioService, boost::asio::ip::udp::endpoint serverEndpoint)
    {
        return std::shared_ptr<Client>(new Client(ioService, serverEndpoint));
    }

    void receive();
    void selectStream(std::string const& stream);
    void sendStream(unsigned char* data, size_t size);
    void getStreams();

    bool isConnected() const;

    void close();

    void setOnStreamReceived(StreamReceivedCallback f);
    void setOnGetStreamsReceived(GetStreamsReceivedCallback f);

private:
    Client(boost::asio::io_service& ioService, boost::asio::ip::udp::endpoint serverEndpoint);

    void handleReceive(const boost::system::error_code& error, size_t bytesTransferred);
    void handleSend(const boost::system::error_code& error, size_t bytesTransferred);
    void handleTimeout(const boost::system::error_code& error);

    boost::uuids::uuid             uuid_;
    boost::asio::ip::udp::socket   socket_;
    boost::asio::ip::udp::endpoint serverEndpoint_;
    boost::asio::deadline_timer    deadlineTimer_;
    boost::array<char, 4096>       networkBuffer_;

    std::atomic<bool> connected_;

    StreamReceivedCallback     onStreamReceived_;
    GetStreamsReceivedCallback onGetStreamsReceived_;
};

} // namespace ST::Network

#endif