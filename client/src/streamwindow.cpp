#include "streamwindow.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <exception>

namespace ST::UI
{

StreamWindow::StreamWindow() : Window()
{

}

StreamWindow::~StreamWindow()
{
}

void StreamWindow::render()
{
    if (connectionWidget_.connectClicked())
    {
        std::string    host = connectionWidget_.host();
        unsigned short port = connectionWidget_.port();

        boost::asio::ip::udp::endpoint receiver_endpoint(boost::asio::ip::address::from_string(host), port);

        connection_future_ = std::async(std::launch::async, [this, &receiver_endpoint]() {
            client_ = ST::Network::Client::create(io_service_, receiver_endpoint);
            io_service_.run();
        });
    }

    using namespace std::chrono_literals;

    if (!connection_future_.valid() || connection_future_.wait_for(0ms) == std::future_status::ready)
        connectionState_ = ConnectionState::NotConnected;
    else
        connectionState_ = ConnectionState::Connected;

    bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    if (connectionState_ == ConnectionState::NotConnected)
        connectionWidget_.render();

    // TODO if (connectionState_ == ConnectionState::Connecting)

    if (connectionState_ == ConnectionState::Connected)
        streamSelectionWidget_.render();
}

void StreamWindow::renderBackground()
{
    Window::renderBackground();
}

} // namespace ST::UI