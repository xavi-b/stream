#include "streamwindow.h"
#include "connectionwidget.h"
#include "streamselectionwidget.h"
#include "sclbroadcaster.h"

#include <exception>
#include <spdlog/spdlog.h>

#include <iostream>

int main(int, char**)
{
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("DEBUG");
#endif

    try
    {
        ST::SclBroadcaster broadcaster;
        ST::UI::StreamWindow mainwindow;
        return mainwindow.exec();
    }
    catch (std::exception const& e)
    {
        spdlog::critical(e.what());
        return 1;
    }
}