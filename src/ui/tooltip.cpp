#include "solar/ui/tooltip.hpp"
#include "solar/theme/theme_manager.hpp"

namespace Solar::UI {

    void Tooltip::Show(const char* text) {
        ImGui::BeginTooltip();
        ImGui::PushStyleColor(ImGuiCol_Text, ThemeManager::Get().GetPalette().TextPrimary);
        ImGui::TextUnformatted(text);
        ImGui::PopStyleColor();
        ImGui::EndTooltip();
    }

    void Tooltip::ItemTooltip(const char* text) {
        if (ImGui::IsItemHovered() && text && text[0]) {
            Show(text);
        }
    }

} // namespace Solar::UI
