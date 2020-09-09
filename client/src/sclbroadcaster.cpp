#include "sclbroadcaster.h"

#include "openh264_spdlog.h"

#include <spdlog/spdlog.h>

#include <fstream>

namespace ST
{

static void ExtractAndConvertToYUV(const SL::Screen_Capture::Image& img, std::array<std::vector<unsigned char>, 3>& yuv)
{
    auto imgsrc = StartSrc(img);

    int yPos = 0;
    int uPos = 0;
    int vPos = 0;

    for (auto h = 0; h < Height(img); ++h)
    {
        auto startimgsrc = imgsrc;
        if (!(h % 2))
        {
            for (auto w = 0; w < Width(img); w += 2)
            {
                // pixel 1
                {
                    int r = imgsrc->R;
                    int g = imgsrc->G;
                    int b = imgsrc->B;

                    int y = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
                    int u = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
                    int v = ((112 * r + -94 * g + -18 * b) >> 8) + 128;

                    yuv[0][yPos++] = y;
                    yuv[1][uPos++] = u;
                    yuv[2][vPos++] = v;

                    imgsrc++;
                }

                // pixel 2
                {
                    int r = imgsrc->R;
                    int g = imgsrc->G;
                    int b = imgsrc->B;

                    int y = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

                    yuv[0][yPos++] = y;

                    imgsrc++;
                }
            }
        }
        else
        {
            for (auto w = 0; w < Width(img); ++w)
            {
                int r = imgsrc->R;
                int g = imgsrc->G;
                int b = imgsrc->B;

                int y = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

                yuv[0][yPos++] = y;

                imgsrc++;
            }
        }
        imgsrc = SL::Screen_Capture::GotoNextRow(img, startimgsrc);
    }
}

SclBroadcaster::SclBroadcaster() : Broadcaster()
{
    // TODO udp size
    int width  = 512;
    int height = 512;

    WelsCreateSVCEncoder(&pSvcEncoder_);
    memset(&sEncParam_, 0, sizeof(SEncParamBase));
    sEncParam_.iUsageType     = EUsageType::SCREEN_CONTENT_REAL_TIME;
    sEncParam_.fMaxFrameRate  = 30;
    sEncParam_.iPicWidth      = width;
    sEncParam_.iPicHeight     = height;
    sEncParam_.iTargetBitrate = 5000000;

    int ret = pSvcEncoder_->Initialize(&sEncParam_);
    if (ret)
    {
        spdlog::warn("Encoder initialization failed: {}", ret);
    }

    int log_level = WELS_LOG_DETAIL;
    pSvcEncoder_->SetOption(ENCODER_OPTION_TRACE_LEVEL, &log_level);

    WelsTraceCallback callback_function;
    callback_function = ST::openh264_spdlog;
    pSvcEncoder_->SetOption(ENCODER_OPTION_TRACE_CALLBACK, (void*)&callback_function);

    int videoFormat = videoFormatI420;
    pSvcEncoder_->SetOption(ENCODER_OPTION_DATAFORMAT, &videoFormat);

    memset(&info_, 0, sizeof(SFrameBSInfo));
    memset(&pic_, 0, sizeof(SSourcePicture));

    pic_.iPicWidth    = width;
    pic_.iPicHeight   = height;
    pic_.iColorFormat = EVideoFormatType::videoFormatI420;
    pic_.iStride[0]   = pic_.iPicWidth;
    pic_.iStride[1]   = pic_.iPicWidth >> 1;
    pic_.iStride[2]   = pic_.iStride[1];

    screenCaptureManager_ =
        SL::Screen_Capture::CreateCaptureConfiguration([]() {
            auto mons = SL::Screen_Capture::GetMonitors();
            spdlog::debug("Library is requesting the list of monitors to capture!");
            for (auto& m : mons)
            {
                spdlog::debug("{}", m.Name);
                SL::Screen_Capture::Height(m, 512);
                SL::Screen_Capture::Width(m, 512);
            }
            return mons;
        })
            ->onFrameChanged([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {

            })
            ->onNewFrame([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
                if (!started_)
                    return;

                if (monitor.Id != 0)
                    return;

                auto size = Width(img) * Height(img);
                spdlog::debug("Image dimensions {}", size);
                std::array<std::vector<unsigned char>, 3> yuv;
                yuv[0].resize(size);
                yuv[1].resize(size / 4);
                yuv[2].resize(size / 4);
                ExtractAndConvertToYUV(img, yuv);

                std::fstream fs("test.yuv", std::fstream::out);
                fs.write((const char*)yuv[0].data(), yuv[0].size());
                fs.write((const char*)yuv[1].data(), yuv[1].size());
                fs.write((const char*)yuv[2].data(), yuv[2].size());
                fs.close();
                // exit(0);

                if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() -
                                                                          onNewFramestart_)
                        .count() >= 1000)
                {
                    spdlog::debug("onNewFrame fps {}", onNewFramecounter);
                    onNewFramecounter = 0;
                    onNewFramestart_  = std::chrono::high_resolution_clock::now();
                }
                onNewFramecounter += 1;

                pic_.pData[0] = yuv[0].data();
                pic_.pData[1] = yuv[1].data();
                pic_.pData[2] = yuv[2].data();

                int ret = pSvcEncoder_->EncodeFrame(&pic_, &info_);
                if (ret != CM_RETURN::cmResultSuccess)
                {
                    spdlog::warn("EncodeFrame failed: {}", ret);
                }
                if (info_.eFrameType != EVideoFrameType::videoFrameTypeSkip)
                {
                    if (onNewFrame_)
                    {
                        for (int iLayer = 0; iLayer < info_.iLayerNum; iLayer++)
                        {
                            SLayerBSInfo* pLayerBsInfo = &info_.sLayerInfo[iLayer];

                            int iLayerSize = 0;
                            int iNalIdx    = pLayerBsInfo->iNalCount - 1;
                            do
                            {
                                iLayerSize += pLayerBsInfo->pNalLengthInByte[iNalIdx];
                                --iNalIdx;
                            } while (iNalIdx >= 0);

                            unsigned char* outBuf = pLayerBsInfo->pBsBuf;

                            Frame frame(outBuf, outBuf + iLayerSize);
                            onNewFrame_(frame);
                        }
                    }
                }
            })
            ->start_capturing();
    screenCaptureManager_->pause();
}

SclBroadcaster::~SclBroadcaster()
{
    pSvcEncoder_->Uninitialize();
    WelsDestroySVCEncoder(pSvcEncoder_);
}

void SclBroadcaster::start()
{
    onNewFramestart_ = std::chrono::high_resolution_clock::now();
    screenCaptureManager_->setFrameChangeInterval(std::chrono::milliseconds(100));
    screenCaptureManager_->resume();
    started_ = true;
}

void SclBroadcaster::stop()
{
    screenCaptureManager_->pause();
    started_ = false;
}

} // namespace ST