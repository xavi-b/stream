#ifndef WIDGET_H
#define WIDGET_H

#include "imgui.h"

namespace ST::UI
{

class Widget
{
public:
    virtual ~Widget();
    virtual void render() const = 0;
};

} // namespace ST::UI

#endif