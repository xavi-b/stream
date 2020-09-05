#ifndef STREAMSELECTIONWIDGET_H
#define STREAMSELECTIONWIDGET_H

#include "widget.h"

#include <functional>
#include <string>
#include <vector>

namespace ST::UI
{

class StreamSelectionWidget : public Widget
{
public:
    virtual void render() override;

    void setOnBroadcastClicked(std::function<void()> f);
    void setOnGetStreamsClicked(std::function<void()> f);
    void setOnSelectStreamClicked(std::function<void(std::string const&)> f);

    std::vector<std::string>&       streams();
    std::vector<std::string> const& streams() const;

private:
    std::function<void()>                   onBroadcastClicked_;
    std::function<void()>                   onGetStreamsClicked_;
    std::function<void(std::string const&)> onSelectStreamClicked_;
    bool                                    reduced_ = false;
    std::vector<std::string>                streams_;
};

} // namespace ST::UI

#endif