#include "streamselectionwidget.h"

#include "imgui.h"

namespace ST::UI
{

void StreamSelectionWidget::render()
{
    float height = 200;
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - height), 0);
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, height), 0);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;

    ImGui::Begin("Streams", NULL, window_flags);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
    ImVec2 scrolling_child_size = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("scrolling", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar);
    for (int line = 0; line < 6; line++)
    {
        // Display random stuff. For the sake of this trivial demo we are using basic Button() + SameLine()
        // If you want to create your own time line for a real application you may be better off manipulating
        // the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position the widgets
        // yourself. You may also want to use the lower-level ImDrawList API.
        int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
        for (int n = 0; n < num_buttons; n++)
        {
            if (n > 0)
                ImGui::SameLine();
            ImGui::PushID(n + line * 1000);
            float hue = n * 0.05f;
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
            ImGui::Button("FizzBuzz", ImVec2(40.0f, 0.0f));
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(2);

    ImGui::End();
}

} // namespace ST::UI