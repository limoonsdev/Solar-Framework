#include "solar/game/spectator_list.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/font_awesome.hpp"
#include <imgui.h>

namespace Solar::Game {

    void SpectatorList::Render(bool* p_open, const std::vector<std::string>& spectators) {
        if (!p_open || !(*p_open)) return;

        ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiCond_FirstUseEver);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

        if (ImGui::Begin("Spectators", p_open, flags)) {
            const auto& pal = ThemeManager::Get().GetPalette();
            if (spectators.empty()) {
                ImGui::TextColored(pal.TextDisabled, "No active spectators");
            } else {
                for (const auto& s : spectators) {
                    ImGui::TextColored(pal.Accent, ICON_FA_EYE);
                    ImGui::SameLine(0, 8.0f);
                    ImGui::Text("%s", s.c_str());
                }
            }
        }
        ImGui::End();
    }

} // namespace Solar::Game
