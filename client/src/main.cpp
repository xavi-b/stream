#include "streamwindow.h"
#include "connectionwidget.h"
#include "streamselectionwidget.h"

#include <iostream>
#include <exception>

int main(int, char**)
{
    try
    {
        ST::UI::StreamWindow mainwindow;
        return mainwindow.exec();
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}