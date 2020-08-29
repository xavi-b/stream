#ifndef STREAMWINDOW_H
#define STREAMWINDOW_H

#include "window.h"
#include "connectionwidget.h"
#include "streamselectionwidget.h"
#include "client.h"

#include <future>

namespace ST::UI
{

class StreamWindow : public Window
{
    enum class ConnectionState
    {
        NotConnected = 0,
        Connecting   = 1,
        Connected    = 2
    };

public:
    StreamWindow();
    virtual ~StreamWindow();
    virtual void render() override;
    virtual void renderBackground() override;
    virtual void onClose() override;

private:
    ConnectionState                      connectionState_ = ConnectionState::NotConnected;
    std::future<void>                    connection_future_;
    boost::asio::io_service              io_service_;
    std::shared_ptr<ST::Network::Client> client_;

    ST::UI::ConnectionWidget      connectionWidget_;
    ST::UI::StreamSelectionWidget streamSelectionWidget_;
};

} // namespace ST::UI

#endif