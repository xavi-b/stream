#ifndef STREAMSELECTIONWIDGET_H
#define STREAMSELECTIONWIDGET_H

#include "widget.h"

namespace ST::UI
{

class StreamSelectionWidget : public Widget
{
public:
    virtual void render() const override;
};

} // namespace ST::UI

#endif