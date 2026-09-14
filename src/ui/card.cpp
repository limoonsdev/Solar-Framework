#include "solar/ui/card.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/drawlist_utils.hpp"
#include <imgui_internal.h>

namespace Solar::UI {

    bool Card::Begin(const char* str_id, const char* title, const ImVec2& size, const char* icon) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ThemeManager::Get().GetStyle().CardRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

        bool visible = ImGui::BeginChild(str_id, size, true, ImGuiWindowFlags_None);
        if (visible) {
            ImDrawList* draw = ImGui::GetWindowDrawList();
            ImVec2 pos = ImGui::GetWindowPos();
            float w = ImGui::GetWindowWidth();
            const auto& pal = ThemeManager::Get().GetPalette();

            // Header line inside card
            float headerH = 38.0f;
            draw->AddLine(ImVec2(pos.x, pos.y + headerH), ImVec2(pos.x + w, pos.y + headerH), pal.Border.ToU32(), 1.0f);

            // Title & optional Icon
            float startX = pos.x + 14.0f;
            if (icon) {
                draw->AddText(ImVec2(startX, pos.y + 10.0f), pal.Accent.ToU32(), icon);
                startX += 20.0f;
            }
            draw->AddText(ImVec2(startX, pos.y + 10.0f), pal.TextPrimary.ToU32(), title);

            ImGui::SetCursorPosY(headerH + 10.0f);
            ImGui::SetCursorPosX(14.0f);
        }

        return visible;
    }

    void Card::End() {
        ImGui::EndChild();
        ImGui::PopStyleVar(2);
    }

} // namespace Solar::UI
