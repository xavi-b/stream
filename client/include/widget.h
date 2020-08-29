#ifndef WIDGET_H
#define WIDGET_H

namespace ST::UI
{

class Widget
{
public:
    virtual ~Widget();
    virtual void render() = 0;
};

} // namespace ST::UI

#endif