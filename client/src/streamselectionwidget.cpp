#include "streamselectionwidget.h"

#include "imgui.h"

#include "broadcaster.h"

namespace ST::UI
{

void StreamSelectionWidget::render()
{
    int   buttonHeight = 20;
    float height       = reduced_ ? buttonHeight + 16 : 200;
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - height), 0);
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, height), 0);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;

    ImGui::Begin("Streams", NULL, window_flags);

    int width = ImGui::GetWindowContentRegionMax().x;

    if (ImGui::Button(ST::Broadcaster::instance()->started() ? "Stop" : "Broadcast", ImVec2(100, buttonHeight)))
    {
        if (onBroadcastClicked_)
            onBroadcastClicked_();
    }

    ImGui::SameLine();

    if (ImGui::Button("Get Streams", ImVec2(100, buttonHeight)))
    {
        if (onGetStreamsClicked_)
            onGetStreamsClicked_();
    }

    int buttonWidth = 20;
    ImGui::SameLine(width - buttonWidth);

    if (ImGui::Button(reduced_ ? "^" : "X", ImVec2(buttonWidth, buttonHeight)))
    {
        reduced_ = !reduced_;
    }

    if (reduced_)
    {
        ImGui::End();
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
    ImVec2 scrolling_child_size = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("scrolling", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar);
    for (size_t i = 0; i < streams_.size(); ++i)
    {
        if (i > 0)
            ImGui::SameLine();

        float hue = i * 0.05f;
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
        int size = ImGui::GetContentRegionAvail().y;
        if (ImGui::Button(streams_[i].c_str(), ImVec2(size, size)))
        {
            if (onSelectStreamClicked_)
                onSelectStreamClicked_(streams_[i]);
        }
        ImGui::PopStyleColor(3);
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(2);

    ImGui::End();
}

void StreamSelectionWidget::setOnBroadcastClicked(std::function<void()> f)
{
    onBroadcastClicked_ = f;
}

void StreamSelectionWidget::setOnGetStreamsClicked(std::function<void()> f)
{
    onGetStreamsClicked_ = f;
}

void StreamSelectionWidget::setOnSelectStreamClicked(std::function<void(std::string const&)> f)
{
    onSelectStreamClicked_ = f;
}

std::vector<std::string>& StreamSelectionWidget::streams()
{
    return streams_;
}

std::vector<std::string> const& StreamSelectionWidget::streams() const
{
    return streams_;
}

} // namespace ST::UI