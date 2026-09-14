#include "solar/widgets/search_input.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/icons.hpp"
#include <imgui.h>
#include <imgui_internal.h>

namespace Solar::Widgets {

    bool SearchInput(const char* label, char* buffer, size_t bufferSize, const char* hint) {
        if (!buffer || bufferSize == 0) return false;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        const auto& pal = ThemeManager::Get().GetPalette();
        float width = ImGui::GetContentRegionAvail().x;
        float height = 34.0f;
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImDrawList* draw = window->DrawList;

        // Custom styling for search bar
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(pal.Card.r, pal.Card.g, pal.Card.b, pal.Card.a));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(pal.Border.r, pal.Border.g, pal.Border.b, pal.Border.a));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(32.0f, 7.0f));

        ImGui::SetNextItemWidth(width);
        bool changed = ImGui::InputTextWithHint(label, hint, buffer, bufferSize);

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);

        // Draw Search Icon on left
        IconRenderer::DrawIcon(draw, IconType::Search, ImVec2(pos.x + 16.0f, pos.y + height * 0.5f), 10.0f,
                               ThemeManager::ToU32(pal.TextDisabled), 1.4f);

        // Draw [X] Clear button if buffer has text
        if (buffer[0] != '\0') {
            ImVec2 clearPos(pos.x + width - 22.0f, pos.y + height * 0.5f);
            bool hoverX = ImGui::IsMouseHoveringRect(ImVec2(clearPos.x - 10, clearPos.y - 10), ImVec2(clearPos.x + 10, clearPos.y + 10));

            draw->AddCircleFilled(clearPos, 8.0f, hoverX ? ThemeManager::ToU32(pal.CardHover) : ThemeManager::ToU32(pal.Border), 12);
            draw->AddText(ImVec2(clearPos.x - 4, clearPos.y - 7), hoverX ? ThemeManager::ToU32(pal.Accent) : ThemeManager::ToU32(pal.TextDisabled), "x");

            if (hoverX && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                buffer[0] = '\0';
                changed = true;
                Audio::PlayClick();
            }
        }

        return changed;
    }

} // namespace Solar::Widgets
