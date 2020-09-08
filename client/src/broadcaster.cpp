#include "broadcaster.h"

#include <spdlog/spdlog.h>

namespace ST
{

void cbAudioPrerender(void* p_audio_data, uint8_t** pp_pcm_buffer, unsigned int size)
{
    Broadcaster::instance()->audioPrerender(p_audio_data, pp_pcm_buffer, size);
}

void cbAudioPostrender(void*        p_audio_data,
                       uint8_t*     p_pcm_buffer,
                       unsigned int channels,
                       unsigned int rate,
                       unsigned int nb_samples,
                       unsigned int bits_per_sample,
                       unsigned int size,
                       int64_t      pts)
{
    Broadcaster::instance()->audioPostrender(
        p_audio_data, p_pcm_buffer, channels, rate, nb_samples, bits_per_sample, size, pts);
}

void cbVideoPrerender(void* p_video_data, uint8_t** pp_pixel_buffer, int size)
{
    Broadcaster::instance()->videoPrerender(p_video_data, pp_pixel_buffer, size);
}

void cbVideoPostrender(
    void* p_video_data, uint8_t* p_pixel_buffer, int width, int height, int pixel_pitch, int size, int64_t pts)
{
    Broadcaster::instance()->videoPostrender(p_video_data, p_pixel_buffer, width, height, pixel_pitch, size, pts);
}

void Broadcaster::audioPrerender(void* p_audio_data, uint8_t** pp_pcm_buffer, unsigned int size)
{
    spdlog::debug("audioPrerender");
    audioBuffer_.resize(size);
    *pp_pcm_buffer = audioBuffer_.data();
}

void Broadcaster::audioPostrender(void*        p_audio_data,
                                  uint8_t*     p_pcm_buffer,
                                  unsigned int channels,
                                  unsigned int rate,
                                  unsigned int nb_samples,
                                  unsigned int bits_per_sample,
                                  unsigned int size,
                                  int64_t      pts)
{
    spdlog::debug("audioPostrender");
}

void Broadcaster::videoPrerender(void* p_video_data, uint8_t** pp_pixel_buffer, int size)
{
    spdlog::debug("videoPrerender");
    videoBuffer_.resize(size);
    *pp_pixel_buffer = videoBuffer_.data();
}

void Broadcaster::videoPostrender(
    void* p_video_data, uint8_t* p_pixel_buffer, int width, int height, int pixel_pitch, int size, int64_t pts)
{
    spdlog::debug("videoPostrender");
    if(onNewFrame_)
    {
        Frame frame;
        frame.resize(size);
        memcpy(frame.data(), p_pixel_buffer, size);
        spdlog::debug("size: {}", size);
        onNewFrame_(frame);
    }
}

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
    std::ostringstream stream;
    stream << "#transcode{vcodec=h264,venc=x264{";
    stream << "keyint=30,bframes=0,ref=1,level=3.0,profile=baseline,hrd=cbr,crf=20,ratetol=1.0,";
    stream << "vbv-maxrate=1200,vbv-bufsize=1200,preset=ultrafast,tune=zerolatency,aud,lookahead=0,";
    stream << "sps-id=1,verbose";
    stream << "},vb=800,scale=0.5,acodec=mp3,ab=128,channels=2,samplerate=44100}:smem{";
    stream << "video-prerender-callback=" << (long long int)(intptr_t)(void*)&cbVideoPrerender << ",";
    stream << "video-postrender-callback=" << (long long int)(intptr_t)(void*)&cbVideoPostrender << ",";
    stream << "audio-prerender-callback=" << (long long int)(intptr_t)(void*)&cbAudioPrerender << ",";
    stream << "audio-postrender-callback=" << (long long int)(intptr_t)(void*)&cbAudioPostrender << ",";
    stream << "audio-data=" << (long long int)0 << ",";
    stream << "video-data=" << (long long int)0 << "";
    stream << "},";

    inst_ = libvlc_new(0, NULL);

    // TODO select monitor
    std::vector<const char*> params = {
        "sout", "sout-all", "screen-top=0", "screen-left=0", "screen-width=1920", "screen-height=1080", "screen-fps=10"};

    libvlc_vlm_add_broadcast(inst_, "stream", "screen://", stream.str().c_str(), params.size(), params.data(), 1, 0);
}

} // namespace ST