#include "solar/game/keybind_list.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui.h>

namespace Solar::Game {

    void KeybindList::Render(bool* p_open, const std::vector<std::pair<std::string, std::string>>& binds) {
        if (!p_open || !(*p_open)) return;

        ImGui::SetNextWindowSize(ImVec2(210, 140), ImGuiCond_FirstUseEver);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

        if (ImGui::Begin("Active Keybinds", p_open, flags)) {
            const auto& pal = ThemeManager::Get().GetPalette();
            if (binds.empty()) {
                ImGui::TextColored(pal.TextDisabled, "No active binds");
            } else {
                for (const auto& b : binds) {
                    ImGui::Text("%s", b.first.c_str());
                    ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(b.second.c_str()).x - 14.0f);
                    ImGui::TextColored(pal.Accent, "%s", b.second.c_str());
                }
            }
        }
        ImGui::End();
    }

} // namespace Solar::Game
