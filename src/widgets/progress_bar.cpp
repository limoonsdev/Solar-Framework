#include "solar/widgets/progress_bar.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/core/math.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    void ProgressBar(float fraction, const ImVec2& sizeArg, const char* overlay) {
        float availX = ImGui::GetContentRegionAvail().x;
        ImVec2 size = sizeArg;
        if (size.x <= 0.0f) size.x = availX;

        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        fraction = Math::Clamp(fraction, 0.0f, 1.0f);

        // Track
        draw->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), pal.Background.ToU32(), size.y * 0.5f);

        // Fill
        if (fraction > 0.001f) {
            draw->AddRectFilled(p, ImVec2(p.x + size.x * fraction, p.y + size.y), pal.Accent.ToU32(), size.y * 0.5f);
        }

        if (overlay) {
            ImVec2 ts = ImGui::CalcTextSize(overlay);
            draw->AddText(ImVec2(p.x + (size.x - ts.x) * 0.5f, p.y + (size.y - ts.y) * 0.5f), 0xFFFFFFFF, overlay);
        }

        ImGui::Dummy(ImVec2(size.x, size.y + 4.0f));
    }

} // namespace Solar::Widgets
