#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include "widget.h"

namespace ST::UI
{

class ConnectionWidget : public Widget
{
public:
    virtual void render() const override;
};

} // namespace ST::UI

#endif