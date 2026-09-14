#include "solar/widgets/separator.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    void Separator() {
        ImGui::Spacing();
        ImVec2 p = ImGui::GetCursorScreenPos();
        float w = ImGui::GetContentRegionAvail().x;
        ImDrawList* draw = ImGui::GetWindowDrawList();
        draw->AddLine(p, ImVec2(p.x + w, p.y), ThemeManager::Get().GetPalette().Border.ToU32(), 1.0f);
        ImGui::Spacing();
    }

    void Spacing(float h) {
        ImGui::Dummy(ImVec2(0.0f, h));
    }

} // namespace Solar::Widgets
