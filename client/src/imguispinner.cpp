#include "imguispinner.h"

#include "imgui_internal.h"

#include <math.h>

// https://github.com/ocornut/imgui/issues/1901

namespace ImGui
{

void Spinner(const char* label, const ImVec2& size, int thickness, const ImU32& color)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext&     g     = *GImGui;
    const ImGuiID     id    = window->GetID(label);

    float radius = size.y / 2;
    ImVec2 pos = window->DC.CursorPos;

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ItemSize(bb, 0);
    if (!ItemAdd(bb, id))
        return;

    // Render
    window->DrawList->PathClear();

    int num_segments = radius*2;
    int start        = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

    const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

    const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius);

    for (int i = 0; i < num_segments; i++)
    {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        window->DrawList->PathLineTo(
            ImVec2(centre.x + ImCos(a + g.Time * 8) * radius, centre.y + ImSin(a + g.Time * 8) * radius));
    }

    window->DrawList->PathStroke(color, false, thickness);
}

} // namespace ImGui