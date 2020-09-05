#include "streamwindow.h"
#include "connectionwidget.h"
#include "streamselectionwidget.h"

#include <exception>
#include <spdlog/spdlog.h>

#include <iostream>

int main(int argc, char** argv)
{
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("DEBUG");
#endif

    try
    {
        ST::UI::StreamWindow mainwindow;
        return mainwindow.exec();
    }
    catch (std::exception const& e)
    {
        spdlog::critical(e.what());
        return 1;
    }
}