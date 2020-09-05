#include "streamwindow.h"

#include "broadcaster.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <chrono>
#include <exception>

namespace ST::UI
{

StreamWindow::StreamWindow() : Window()
{
    connectionWidget_.setOnConnectClicked([this]() {
        std::string    host = connectionWidget_.host();
        unsigned short port = connectionWidget_.port();

        boost::asio::ip::udp::endpoint receiverEndpoint(boost::asio::ip::address::from_string(host), port);

        connectionFuture_ = std::async(std::launch::async, [this, receiverEndpoint]() {
            spdlog::debug("Client started");
            client_ = ST::Network::Client::create(ioService_, receiverEndpoint);
            client_->setOnStreamReceived([this](ST::Network::Client::StreamData data) {
                // TODO producer thread
            });
            client_->receive();
            client_->getStreams();
            ioService_.restart();
            ioService_.run();
            spdlog::debug("Client closed");
        });
    });

    streamSelectionWidget_.setOnBroadcastClicked([this]() {
        if (Broadcaster::instance()->started())
            Broadcaster::instance()->stop();
        else
            Broadcaster::instance()->start();
    });

    Broadcaster::instance()->setOnNewFrame([this](ST::Broadcaster::Frame frame) {
        client_->sendStream(frame.data(), frame.size());
    });
}

StreamWindow::~StreamWindow()
{
}

void StreamWindow::render()
{
    using namespace std::chrono_literals;

    if (!connectionFuture_.valid() || connectionFuture_.wait_for(0ms) == std::future_status::ready)
        connectionState_ = ConnectionState::NotConnected;
    else
    {
        if (!client_ || !client_->isConnected())
            connectionState_ = ConnectionState::Connecting;
        else
            connectionState_ = ConnectionState::Connected;
    }

    bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    if (connectionState_ == ConnectionState::NotConnected)
        connectionWidget_.render();

    if (connectionState_ == ConnectionState::Connecting)
        connectingWidget_.render();

    if (connectionState_ == ConnectionState::Connected)
        streamSelectionWidget_.render();
}

void StreamWindow::renderBackground()
{
    // TODO render frame
    Window::renderBackground();
}

void StreamWindow::onClose()
{
    ioService_.stop();
    connectionFuture_.wait();
}

} // namespace ST::UI