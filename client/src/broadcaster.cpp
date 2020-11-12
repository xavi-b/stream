#include "broadcaster.h"

namespace ST
{

Broadcaster::Broadcaster()
{
    if (singleton_)
        throw std::runtime_error("A broadcaster instance already exists !");

    singleton_ = this;
}

Broadcaster::~Broadcaster()
{
}

Broadcaster* Broadcaster::instance()
{
    return singleton_;
}

void Broadcaster::start()
{
    started_ = true;
}

void Broadcaster::stop()
{
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

} // namespace ST