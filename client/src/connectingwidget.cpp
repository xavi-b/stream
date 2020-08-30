#include "connectingwidget.h"

#include "imgui.h"

#include "imguispinner.h"

namespace ST::UI
{

void ConnectingWidget::render()
{
    float size      = 80;
    float thickness = 8;

    ImGui::SetNextWindowPos(
        ImVec2((ImGui::GetIO().DisplaySize.x - size) / 2, (ImGui::GetIO().DisplaySize.y - size) / 2), 0);
    ImGui::SetNextWindowSize(ImVec2(size, size), 0);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("Connecting", NULL, window_flags);

    ImGui::Spinner("##spinner", ImGui::GetContentRegionAvail(), thickness, ImGui::GetColorU32(ImGuiCol_ButtonHovered));

    ImGui::End();

    ImGui::PopStyleVar(1);
}

} // namespace ST::UI