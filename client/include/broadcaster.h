#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <stdexcept>
#include <vector>
#include <functional>
#include <sstream>

namespace ST
{

class Broadcaster
{
public:
    using Frame            = std::vector<unsigned char>;
    using NewFrameCallback = std::function<void(Frame)>;

    Broadcaster();
    ~Broadcaster();
    static Broadcaster* instance();
    virtual void        start();
    virtual void        stop();
    bool                started() const;

    void setOnNewFrame(NewFrameCallback f);

protected:
    bool             started_ = false;
    NewFrameCallback onNewFrame_;

private:
    static inline Broadcaster* singleton_ = nullptr;
};

} // namespace ST

#endif