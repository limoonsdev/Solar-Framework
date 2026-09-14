#include "solar/ui/groupbox.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui_internal.h>

namespace Solar::UI {

    void Groupbox::Begin(const char* title, const ImVec2& size) {
        ImGui::BeginChild(title, size, true);
        ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "%s", title);
        ImGui::Separator();
        ImGui::Spacing();
    }

    void Groupbox::End() {
        ImGui::EndChild();
    }

} // namespace Solar::UI
