#include "solar/widgets/badge.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>

namespace Solar::Widgets {

    void Badge(const char* text, Color color) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return;

        auto lv = Render::CleanLabel(text);
        if (lv.size.x <= 0.0f) return;

        ImVec2 pad(8.0f, 4.0f);
        ImVec2 totalSize(lv.size.x + pad.x * 2.0f, lv.size.y + pad.y * 2.0f);

        ImVec2 p = window->DC.CursorPos;
        ImRect bb(p, ImVec2(p.x + totalSize.x, p.y + totalSize.y));

        ImGui::ItemSize(bb, 0.0f);
        if (!ImGui::ItemAdd(bb, window->GetID(text))) return;

        ImDrawList* draw = window->DrawList;
        float rounding = 5.0f;

        // Subtle drop shadow
        draw->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y + 1.0f),
                            ImVec2(bb.Max.x, bb.Max.y + 1.0f),
                            IM_COL32(0, 0, 0, 80), rounding);

        // Background & border
        draw->AddRectFilled(bb.Min, bb.Max, color.WithAlpha(0.16f).ToU32(), rounding);
        draw->AddRect(bb.Min, bb.Max, color.WithAlpha(0.45f).ToU32(), rounding, 0, 1.0f);

        // Text without ## hash
        draw->AddText(ImVec2(bb.Min.x + pad.x, bb.Min.y + pad.y), color.ToU32(), lv.textBegin, lv.textEnd);
    }

} // namespace Solar::Widgets
