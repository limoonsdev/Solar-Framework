#include "solar/widgets/badge.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    void Badge(const char* text, Color color) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 ts = ImGui::CalcTextSize(text);
        ImVec2 pad(8.0f, 3.0f);
        ImVec2 end(p.x + ts.x + pad.x * 2.0f, p.y + ts.y + pad.y * 2.0f);

        ImDrawList* draw = ImGui::GetWindowDrawList();
        draw->AddRectFilled(p, end, color.WithAlpha(0.18f).ToU32(), 4.0f);
        draw->AddRect(p, end, color.WithAlpha(0.5f).ToU32(), 4.0f);
        draw->AddText(ImVec2(p.x + pad.x, p.y + pad.y), color.ToU32(), text);

        ImGui::SetCursorScreenPos(ImVec2(end.x + 6.0f, p.y));
    }

} // namespace Solar::Widgets
