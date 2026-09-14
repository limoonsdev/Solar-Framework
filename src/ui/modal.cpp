#include "solar/ui/modal.hpp"
#include "solar/theme/theme_manager.hpp"

namespace Solar::UI {

    bool Modal::Begin(const char* name, bool* p_open, const ImVec2& size) {
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        return ImGui::BeginPopupModal(name, p_open, flags);
    }

    void Modal::End() {
        ImGui::EndPopup();
    }

} // namespace Solar::UI
