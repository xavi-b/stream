#include "streamwindow.h"

#include "broadcaster.h"
#include "openh264_spdlog.h"

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
#include <algorithm>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(ST::RC);

#include <fstream>

namespace ST::UI
{

StreamWindow::StreamWindow() : Window()
{
    WelsCreateDecoder(&pSvcDecoder_);
    memset(&sDecParam_, 0, sizeof(SDecodingParam));
    // sDecParam_.eEcActiveIdc                = ERROR_CON_IDC::ERROR_CON_SLICE_COPY;
    sDecParam_.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_TYPE::VIDEO_BITSTREAM_DEFAULT;

    int ret = pSvcDecoder_->Initialize(&sDecParam_);
    if (ret != CM_RETURN::cmResultSuccess)
    {
        spdlog::critical("Decoder initialization failed with error: {}", ret);
    }

    // int32_t iErrorConMethod = (int32_t)ERROR_CON_SLICE_MV_COPY_CROSS_IDR_FREEZE_RES_CHANGE;
    // pSvcDecoder_->SetOption(DECODER_OPTION_ERROR_CON_IDC, &iErrorConMethod);

    int log_level = WELS_LOG_DETAIL;
    pSvcDecoder_->SetOption(DECODER_OPTION_TRACE_LEVEL, &log_level);

    WelsTraceCallback callback_function;
    callback_function = ST::openh264_spdlog;
    pSvcDecoder_->SetOption(DECODER_OPTION_TRACE_CALLBACK, (void*)&callback_function);

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
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_.data()); // TODO

    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
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
    DECODING_STATE ret = pSvcDecoder_->DecodeFrameNoDelay(data, size, pData_, &sDstBufInfo);
    if (ret != 0)
    {
        spdlog::warn("DecodeFrameNoDelay failed: 0x{0:x}", ret);
    }
    spdlog::debug("iBufferStatus: {}", sDstBufInfo.iBufferStatus);
    if (sDstBufInfo.iBufferStatus == 1)
    {
        int h = sDstBufInfo.UsrData.sSystemBuffer.iHeight;
        int w = sDstBufInfo.UsrData.sSystemBuffer.iWidth;

        spdlog::debug("format: {}, h: {}, w: {}", sDstBufInfo.UsrData.sSystemBuffer.iFormat, h, w);
        spdlog::debug("stride[0]: {}, stride[1]: {}", sDstBufInfo.UsrData.sSystemBuffer.iStride[0], sDstBufInfo.UsrData.sSystemBuffer.iStride[1]);

        texture_.resize(h * w * 3);

        int frameImageCounter = 0;

        unsigned char* pY   = pData_[0];
        unsigned char* pU   = pData_[1];
        unsigned char* pV   = pData_[2];

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; x += 2)
            {
                int Y1 = ((int)*(pY++)) - 16;
                int Y2 = ((int)*(pY++)) - 16;
                int U  = ((int)*(pU++)) - 128;
                int V  = ((int)*(pV++)) - 128;

                int R1 = std::clamp((298 * Y1 + 409 * V + 128) >> 8, 0, UINT8_MAX);
                int G1 = std::clamp((298 * Y1 - 100 * U - 208 * V + 128) >> 8, 0, UINT8_MAX);
                int B1 = std::clamp((298 * Y1 + 516 * U + 128) >> 8, 0, UINT8_MAX);

                int R2 = std::clamp((298 * Y2 + 409 * V + 128) >> 8, 0, UINT8_MAX);
                int G2 = std::clamp((298 * Y2 - 100 * U - 208 * V + 128) >> 8, 0, UINT8_MAX);
                int B2 = std::clamp((298 * Y2 + 516 * U + 128) >> 8, 0, UINT8_MAX);

                texture_.data()[frameImageCounter++] = (unsigned char)R1;
                texture_.data()[frameImageCounter++] = (unsigned char)G1;
                texture_.data()[frameImageCounter++] = (unsigned char)B1;

                texture_.data()[frameImageCounter++] = (unsigned char)R2;
                texture_.data()[frameImageCounter++] = (unsigned char)G2;
                texture_.data()[frameImageCounter++] = (unsigned char)B2;
            }

            pY += sDstBufInfo.UsrData.sSystemBuffer.iStride[0] - w;
            pU += sDstBufInfo.UsrData.sSystemBuffer.iStride[1] - w/2;
            pV += sDstBufInfo.UsrData.sSystemBuffer.iStride[1] - w/2;

            if (y % 2)
            {
                pU -= sDstBufInfo.UsrData.sSystemBuffer.iStride[1];
                pV -= sDstBufInfo.UsrData.sSystemBuffer.iStride[1];
            }
        }

        std::fstream fsyuv("received.yuv", std::fstream::out);

        pY   = pData_[0];
        pU   = pData_[1];
        pV   = pData_[2];

        for (int y = 0; y < h; ++y)
        {
            fsyuv.write((const char*)pY, w);
            pY += sDstBufInfo.UsrData.sSystemBuffer.iStride[0];
        }
        for (int y = 0; y < h; y+=2)
        {
            fsyuv.write((const char*)pU, w/2);
            pU += sDstBufInfo.UsrData.sSystemBuffer.iStride[1];
        }
        for (int y = 0; y < h; y+=2)
        {
            fsyuv.write((const char*)pV, w/2);
            pV += sDstBufInfo.UsrData.sSystemBuffer.iStride[1];
        }

        fsyuv.close();

        std::fstream fs("received.rgb", std::fstream::out);
        fs.write((const char*)texture_.data(), texture_.size());
        fs.close();
        // exit(0);
    }
}

} // namespace ST::UI