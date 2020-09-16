#include "connectionwidget.h"

#include "imgui.h"

namespace ST::UI
{

void ConnectionWidget::render()
{
    float height = 100;
    float width  = 200;
    ImGui::SetNextWindowPos(
        ImVec2((ImGui::GetIO().DisplaySize.x - width) / 2, (ImGui::GetIO().DisplaySize.y - height) / 2), 0);
    ImGui::SetNextWindowSize(ImVec2(width, height), 0);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;

    ImGui::Begin("Connection", NULL, window_flags);

    struct TextFilters
    {
        static int FilterIp(ImGuiInputTextCallbackData* data)
        {
            if ((data->EventChar > '0' && data->EventChar < '9') || data->EventChar == '.')
                return 0;
            return 1;
        }
    };
    ImGui::InputText("IP", ip_, sizeof(ip_), ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterIp);

    ImGui::InputText("Port", port_, sizeof(port_), ImGuiInputTextFlags_CharsDecimal);

    if (ImGui::Button("Connect", ImGui::GetContentRegionAvail()))
    {
        if (onConnectClicked_)
            onConnectClicked_();
    }

    ImGui::End();
}

std::string ConnectionWidget::host() const
{
    return std::string(ip_);
}

unsigned short ConnectionWidget::port() const
{
    try
    {
        return std::stoi(port_);
    }
    catch (const std::exception& e)
    {
        return 0;
    }
}

void ConnectionWidget::setOnConnectClicked(std::function<void()> f)
{
    onConnectClicked_ = f;
}

} // namespace ST::UI