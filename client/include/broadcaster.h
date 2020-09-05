#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <stdexcept>
#include <vector>
#include <functional>
#include <vlc/vlc.h>

namespace ST
{

class Broadcaster
{
public:
    using Frame            = std::vector<char>;
    using NewFrameCallback = std::function<void(Frame)>;

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
};

} // namespace ST

#endif