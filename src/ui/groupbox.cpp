#include "solar/ui/groupbox.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui_internal.h>

namespace Solar::UI {

    void Groupbox::Begin(const char* title, const ImVec2& size) {
        const auto& pal = ThemeManager::Get().GetPalette();
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 14.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, pal.Border.ToU32());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, pal.Card.WithAlpha(0.65f).ToU32());

        ImGui::BeginChild(title, size, true);
        ImGui::TextColored(pal.Accent, "%s", title);
        ImGui::Spacing();
    }

    void Groupbox::End() {
        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);
    }

} // namespace Solar::UI
