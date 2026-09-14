#include "solar/ui/titlebar.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/font_awesome.hpp"
#include <imgui_internal.h>

namespace Solar::UI {

    void Titlebar::Render(const char* title, const char* subtitle, bool* p_open, bool* p_minimized) {
        ImVec2 startPos = ImGui::GetCursorScreenPos();
        float winWidth = ImGui::GetWindowWidth();
        float barHeight = 52.0f;
        ImVec2 endPos = ImVec2(startPos.x + winWidth, startPos.y + barHeight);

        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();
        const auto& sty = ThemeManager::Get().GetStyle();

        // Titlebar Background
        draw->AddRectFilled(startPos, endPos, pal.Header.ToU32(), sty.WindowRounding, ImDrawFlags_RoundCornersTop);
        draw->AddLine(ImVec2(startPos.x, endPos.y), ImVec2(endPos.x, endPos.y), pal.Border.ToU32(), 1.0f);

        // Logo Sun Mark
        ImVec2 logoCenter(startPos.x + 28.0f, startPos.y + 26.0f);
        draw->AddCircleFilled(logoCenter, 7.5f, pal.Accent.ToU32(), 24);
        draw->AddCircle(logoCenter, 11.5f, pal.Accent.WithAlpha(0.35f).ToU32(), 24, 1.5f);

        // Title & Subtitle
        ImGui::SetCursorScreenPos(ImVec2(startPos.x + 50.0f, startPos.y + 11.0f));
        ImGui::BeginGroup();
        {
            ImGui::PushStyleColor(ImGuiCol_Text, pal.TextPrimary);
            ImGui::Text("%s", title);
            ImGui::PopStyleColor();

            ImGui::SameLine(0, 8.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, pal.Accent);
            ImGui::Text("%s", subtitle);
            ImGui::PopStyleColor();
        }
        ImGui::EndGroup();

        // Window buttons (Close / Minimize)
        float btnSize = 28.0f;
        float rightPadding = 16.0f;

        if (p_open) {
            ImVec2 closePos(startPos.x + winWidth - rightPadding - btnSize, startPos.y + (barHeight - btnSize) * 0.5f);
            ImGui::SetCursorScreenPos(closePos);
            ImGui::PushID("##TitleClose");
            if (ImGui::InvisibleButton("##CloseBtn", ImVec2(btnSize, btnSize))) {
                Audio::PlayClick();
                *p_open = false;
            }
            bool hovered = ImGui::IsItemHovered();
            u32 bg = hovered ? pal.Danger.WithAlpha(0.2f).ToU32() : 0x00000000;
            u32 ic = hovered ? pal.Danger.ToU32() : pal.TextSecondary.ToU32();
            draw->AddRectFilled(closePos, ImVec2(closePos.x + btnSize, closePos.y + btnSize), bg, 4.0f);
            ImVec2 center(closePos.x + btnSize * 0.5f, closePos.y + btnSize * 0.5f);
            float d = 4.5f;
            draw->AddLine(ImVec2(center.x - d, center.y - d), ImVec2(center.x + d, center.y + d), ic, 1.5f);
            draw->AddLine(ImVec2(center.x + d, center.y - d), ImVec2(center.x - d, center.y + d), ic, 1.5f);
            ImGui::PopID();
        }

        if (p_minimized) {
            ImVec2 minPos(startPos.x + winWidth - rightPadding - btnSize * 2.0f - 6.0f, startPos.y + (barHeight - btnSize) * 0.5f);
            ImGui::SetCursorScreenPos(minPos);
            ImGui::PushID("##TitleMin");
            if (ImGui::InvisibleButton("##MinBtn", ImVec2(btnSize, btnSize))) {
                Audio::PlayClick();
                *p_minimized = !(*p_minimized);
            }
            bool hovered = ImGui::IsItemHovered();
            u32 bg = hovered ? pal.CardHover.ToU32() : 0x00000000;
            u32 ic = hovered ? pal.TextPrimary.ToU32() : pal.TextSecondary.ToU32();
            draw->AddRectFilled(minPos, ImVec2(minPos.x + btnSize, minPos.y + btnSize), bg, 4.0f);
            ImVec2 center(minPos.x + btnSize * 0.5f, minPos.y + btnSize * 0.5f);
            draw->AddLine(ImVec2(center.x - 5.0f, center.y), ImVec2(center.x + 5.0f, center.y), ic, 1.5f);
            ImGui::PopID();
        }

        ImGui::SetCursorScreenPos(ImVec2(startPos.x, startPos.y + barHeight + 8.0f));
    }

} // namespace Solar::UI
