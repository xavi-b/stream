#include "connectionwidget.h"

namespace ST::UI
{

void ConnectionWidget::render() const
{
    float height = 100;
    float width  = 200;
    ImGui::SetNextWindowPos(
        ImVec2((ImGui::GetIO().DisplaySize.x - width) / 2, (ImGui::GetIO().DisplaySize.y - height) / 2), 0);
    ImGui::SetNextWindowSize(ImVec2(width, height), 0);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;

    ImGui::Begin("Connection", NULL, window_flags);

    // TODO
    static char ip[17] = "127.0.0.1";
    struct TextFilters
    {
        static int FilterIp(ImGuiInputTextCallbackData* data)
        {
            if ((data->EventChar > '0' && data->EventChar < '9') || data->EventChar == '.')
                return 0;
            return 1;
        }
    };
    ImGui::InputText("IP", ip, 17, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterIp);

    static char port[6] = "50000";
    ImGui::InputText("Port", port, 6, ImGuiInputTextFlags_CharsDecimal);

    if (ImGui::Button("Connect", ImGui::GetContentRegionAvail()))
    {
        // TODO
    }

    ImGui::End();
}

} // namespace ST::UI