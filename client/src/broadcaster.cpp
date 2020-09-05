#include "broadcaster.h"

namespace ST
{

Broadcaster::~Broadcaster()
{
    if (inst_)
    {
        stop();
        libvlc_release(inst_);
    }
}

Broadcaster* Broadcaster::instance()
{
    static Broadcaster b;
    return &b;
}

void Broadcaster::start()
{
    libvlc_vlm_play_media(inst_, "stream");
    started_ = true;
}

void Broadcaster::stop()
{
    libvlc_vlm_stop_media(inst_, "stream");
    started_ = false;
}

bool Broadcaster::started() const
{
    return started_;
}

void Broadcaster::setOnNewFrame(NewFrameCallback f)
{
    onNewFrame_ = f;
}

Broadcaster::Broadcaster()
{
    inst_ = libvlc_new(0, NULL);

    // TODO select monitor
    std::vector<const char*> params = {
        "screen-top=0", "screen-left=0", "screen-width=1920", "screen-height=1080", "screen-fps=10"};

    libvlc_vlm_add_broadcast(
        inst_,
        "stream",
        "screen://",
        "#transcode{vcodec=h264,vb=800,scale=1,acodec=mpga,ab=128,channels=2,samplerate=44100}:http{mux=ts,dst=:7777/}",
        params.size(),
        params.data(),
        1,
        0);
}

} // namespace ST