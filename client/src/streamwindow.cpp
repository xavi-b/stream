#include "streamwindow.h"

#include "broadcaster.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <exception>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(ST::RC);

namespace ST::UI
{

StreamWindow::StreamWindow() : Window()
{
    WelsCreateDecoder(&pSvcDecoder_);
    sDecParam_.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_SVC;
    pSvcDecoder_->Initialize(&sDecParam_);

    connectionWidget_.setOnConnectClicked([this]() {
        std::string    host = connectionWidget_.host();
        unsigned short port = connectionWidget_.port();

        boost::asio::ip::udp::endpoint receiverEndpoint(boost::asio::ip::address::from_string(host), port);

        connectionFuture_ = std::async(std::launch::async, [this, receiverEndpoint]() {
            spdlog::debug("Client started");
            client_ = ST::Network::Client::create(ioService_, receiverEndpoint);
            client_->setOnStreamReceived([this](ST::Network::Client::StreamData data) {
                decodeStreamData(data.data(), data.size());
            });
            client_->setOnGetStreamsReceived([this](std::vector<std::string> const& streams) {
                streamSelectionWidget_.streams() = streams;
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

    streamSelectionWidget_.setOnGetStreamsClicked([this]() {
        client_->getStreams();
    });

    streamSelectionWidget_.setOnSelectStreamClicked([this](std::string const& stream) {
        client_->selectStream(stream);
    });

    Broadcaster::instance()->setOnNewFrame([this](ST::Broadcaster::Frame frame) {
        client_->sendStream(frame.data(), frame.size());
    });

    auto fs = cmrc::ST::RC::get_filesystem();
    if (!fs.exists("logo.png"))
        throw std::runtime_error("Resource file logo.png does not exists !");
    auto      iconFile = fs.open("logo.png");
    GLFWimage icons[1];
    icons[0].pixels = stbi_load_from_memory(
        (unsigned char*)&(*iconFile.begin()), iconFile.size(), &icons[0].width, &icons[0].height, NULL, 4);
    glfwSetWindowIcon(window_, 1, icons);
    stbi_image_free(icons[0].pixels);

    glGenTextures(1, &bgTextureId_);
    glBindTexture(GL_TEXTURE_2D, bgTextureId_);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

StreamWindow::~StreamWindow()
{
    pSvcDecoder_->Uninitialize();
    WelsDestroyDecoder(pSvcDecoder_);
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
    Window::renderBackground();

    int display_w, display_h;
    glfwGetFramebufferSize(window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTextureId_);
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_.data()); // TODO

    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void StreamWindow::onClose()
{
    ioService_.stop();
    connectionFuture_.wait();
}

void StreamWindow::decodeStreamData(unsigned char* data, int size)
{
    // TODO producer thread

    spdlog::debug("decodeStreamData, size: {}", size);

    SBufferInfo sDstBufInfo;
    memset(&sDstBufInfo, 0, sizeof(SBufferInfo));
    sDstBufInfo.UsrData.sSystemBuffer.iWidth = 1920;
    sDstBufInfo.UsrData.sSystemBuffer.iHeight = 1080;
    DECODING_STATE iRet = pSvcDecoder_->DecodeFrameNoDelay(data, size, pData_, &sDstBufInfo);
    if (iRet != 0)
    {
        spdlog::warn("DecodeFrameNoDelay failed: 0x{0:x}", iRet);
    }
    spdlog::debug("iBufferStatus: {}", sDstBufInfo.iBufferStatus);
    if (sDstBufInfo.iBufferStatus == 1)
    {
        texture_.resize(size);

        int h = sDstBufInfo.UsrData.sSystemBuffer.iHeight;
        int w = sDstBufInfo.UsrData.sSystemBuffer.iWidth;

        int frameImageCounter = 0;

        for (int y = 0; y < h; ++y) // traverse through the frame's height
        {
            for (int x = 0; x < w; ++x) // traverse through the frame's width
            {
                float Y = (float)(pData_[0][y * w + x]) / 255;
                float U = -0.436 + (float)(pData_[1][y * w + x]) / 255 * (0.436 * 2);
                float V = -0.615 + (float)(pData_[2][y * w + x]) / 255 * (0.615 * 2);

                float RFormula = Y + 1.13983f * V;
                float GFormula = Y - 0.39465f * U - 0.58060f * V;
                float BFormula = Y + 2.03211f * U;

                texture_.data()[frameImageCounter++] = (unsigned char)RFormula;
                texture_.data()[frameImageCounter++] = (unsigned char)GFormula;
                texture_.data()[frameImageCounter++] = (unsigned char)BFormula;
            }
        }
    }
}

} // namespace ST::UI