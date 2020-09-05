#ifndef STREAMSELECTIONWIDGET_H
#define STREAMSELECTIONWIDGET_H

#include "widget.h"

#include <functional>

namespace ST::UI
{

class StreamSelectionWidget : public Widget
{
public:
    virtual void render() override;

    void setOnBroadcastClicked(std::function<void()> f);

private:
    std::function<void()> onBroadcastClicked_;
    bool reduced_ = false;
};

} // namespace ST::UI

#endif