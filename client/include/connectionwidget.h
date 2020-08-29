#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include "widget.h"

#include <string>
#include <functional>

namespace ST::UI
{

class ConnectionWidget : public Widget
{
public:
    virtual void   render() override;
    std::string    host() const;
    unsigned short port() const;

    void setOnConnectClicked(std::function<void()> f);

private:
    char ip_[17]  = "127.0.0.1";
    char port_[6] = "50000";

    std::function<void()> onConnectClicked_;
};

} // namespace ST::UI

#endif