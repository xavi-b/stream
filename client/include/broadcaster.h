#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <stdexcept>
#include <vector>
#include <functional>
#include <sstream>
#include <vlc/vlc.h>

namespace ST
{

class Broadcaster
{
public:
    using Frame            = std::vector<char>;
    using NewFrameCallback = std::function<void(Frame)>;

    void audioPrerender(void* p_audio_data, uint8_t** pp_pcm_buffer, unsigned int size);
    void audioPostrender(void*        p_audio_data,
                         uint8_t*     p_pcm_buffer,
                         unsigned int channels,
                         unsigned int rate,
                         unsigned int nb_samples,
                         unsigned int bits_per_sample,
                         unsigned int size,
                         int64_t      pts);
    void videoPrerender(void* p_video_data, uint8_t** pp_pixel_buffer, int size);
    void videoPostrender(
        void* p_video_data, uint8_t* p_pixel_buffer, int width, int height, int pixel_pitch, int size, int64_t pts);

    ~Broadcaster();
    static Broadcaster* instance();
    void                start();
    void                stop();
    bool                started() const;

    void setOnNewFrame(NewFrameCallback f);

private:
    Broadcaster();
    libvlc_instance_t* inst_    = NULL;
    bool               started_ = false;
    NewFrameCallback   onNewFrame_;

    std::vector<uint8_t> audioBuffer_;
    std::vector<uint8_t> videoBuffer_;
};

} // namespace ST

#endif