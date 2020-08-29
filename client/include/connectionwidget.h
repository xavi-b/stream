#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include "widget.h"

#include <string>

namespace ST::UI
{

class ConnectionWidget : public Widget
{
public:
    virtual void   render() override;
    bool           connectClicked() const;
    std::string    host() const;
    unsigned short port() const;

private:
    char ip_[17]         = "127.0.0.1";
    char port_[6]        = "50000";
    bool connectClicked_ = false;
};

} // namespace ST::UI

#endif