#include "solar/ui/sidebar.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include <imgui_internal.h>

namespace Solar::UI {

    static float g_sidebarWidth = 210.0f;

    void Sidebar::Begin(float width) {
        g_sidebarWidth = width;
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float h = ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - 8.0f;
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        draw->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + h), pal.Sidebar.ToU32(), 0.0f);
        draw->AddLine(ImVec2(pos.x + width, pos.y), ImVec2(pos.x + width, pos.y + h), pal.Border.ToU32(), 1.0f);

        ImGui::BeginChild("##SidebarChild", ImVec2(width, h), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::SetCursorPosY(12.0f);
    }

    void Sidebar::End() {
        ImGui::EndChild();
    }

    void Sidebar::Category(const char* label) {
        ImGui::SetCursorPosX(16.0f);
        const auto& pal = ThemeManager::Get().GetPalette();
        ImGui::PushStyleColor(ImGuiCol_Text, pal.TextDisabled);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
    }

    bool Tab(const char* label, int id, int* currentTab, const char* icon) {
        return Sidebar::Tab(label, id, currentTab, icon);
    }

    bool Sidebar::Tab(const char* label, int id, int* currentTab, const char* icon) {
        bool selected = (*currentTab == id);
        ImGui::SetCursorPosX(10.0f);

        float itemWidth = g_sidebarWidth - 20.0f;
        float itemHeight = 36.0f;

        ImGuiID imguiId = ImGui::GetID((std::string("##Tab_") + label).c_str());
        ImVec2 p = ImGui::GetCursorScreenPos();

        bool clicked = ImGui::InvisibleButton(label, ImVec2(itemWidth, itemHeight));
        bool hovered = ImGui::IsItemHovered();

        if (clicked && !selected) {
            *currentTab = id;
            Audio::PlayTabSwitch();
        }

        f32 anim = Anim::AnimationManager::Get().Transition(imguiId, selected, 14.0f);
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        // Background
        if (anim > 0.01f) {
            u32 bgCol = pal.Accent.WithAlpha(0.12f * anim).ToU32();
            draw->AddRectFilled(p, ImVec2(p.x + itemWidth, p.y + itemHeight), bgCol, 6.0f);

            // Left active pill indicator
            f32 indH = itemHeight * 0.55f * anim;
            ImVec2 indMin(p.x + 2.0f, p.y + (itemHeight - indH) * 0.5f);
            ImVec2 indMax(p.x + 5.0f, indMin.y + indH);
            draw->AddRectFilled(indMin, indMax, pal.Accent.ToU32(), 2.0f);
        } else if (hovered) {
            draw->AddRectFilled(p, ImVec2(p.x + itemWidth, p.y + itemHeight), pal.CardHover.WithAlpha(0.4f).ToU32(), 6.0f);
        }

        // Text & Icon
        Color textCol = selected ? pal.TextPrimary : (hovered ? pal.TextPrimary : pal.TextSecondary);
        float textOffsetX = p.x + 18.0f;
        if (icon) {
            draw->AddText(ImVec2(textOffsetX, p.y + 9.0f), selected ? pal.Accent.ToU32() : textCol.ToU32(), icon);
            textOffsetX += 24.0f;
        }

        draw->AddText(ImVec2(textOffsetX, p.y + 9.0f), textCol.ToU32(), label);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
        return clicked;
    }

} // namespace Solar::UI
