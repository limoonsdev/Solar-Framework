#include "solar/tools/inspector.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui.h>
#include <imgui_internal.h>

namespace Solar::Tools {

    void Inspector::Render(bool* p_open) {
        if (!p_open || !(*p_open)) return;

        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Solar Inspector", p_open)) {
            ImGuiContext& g = *GImGui;
            ImGui::Text("Active Window: %s", ImGui::GetCurrentWindow() ? ImGui::GetCurrentWindow()->Name : "None");
            ImGui::Text("Hovered Item ID: 0x%08X", g.HoveredId);
            ImGui::Text("Active Item ID:  0x%08X", g.ActiveId);
        }
        ImGui::End();
    }

} // namespace Solar::Tools
