#ifndef CONNECTINGWIDGET_H
#define CONNECTINGWIDGET_H

#include "widget.h"

#include <string>
#include <functional>

namespace ST::UI
{

class ConnectingWidget : public Widget
{
public:
    virtual void render() override;
};

} // namespace ST::UI

#endif