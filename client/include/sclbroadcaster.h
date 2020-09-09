#ifndef FFMPEGBROADCASTER_H
#define FFMPEGBROADCASTER_H

#include "broadcaster.h"

#include <atomic>
#include <chrono>

#include "ScreenCapture.h"

#include "codec/api/svc/codec_api.h"

namespace ST
{

class SclBroadcaster : public Broadcaster
{
public:
    SclBroadcaster();
    ~SclBroadcaster();
    static Broadcaster* instance();
    virtual void        start() override;
    virtual void        stop() override;

private:
    ISVCEncoder*   pSvcEncoder_;
    SEncParamBase  sEncParam_;
    SFrameBSInfo   info_;
    SSourcePicture pic_;

    std::chrono::system_clock::time_point onNewFramestart_;
    std::atomic<int>                      onNewFramecounter;

    std::shared_ptr<SL::Screen_Capture::IScreenCaptureManager> screenCaptureManager_;
};

} // namespace ST

#endif