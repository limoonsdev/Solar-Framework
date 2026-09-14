#include "solar/ui/sidebar.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/icons/vector_icons.hpp"
#include <imgui_internal.h>

namespace Solar::UI {

    static float g_sidebarWidth = 210.0f;

    void Sidebar::Begin(float width) {
        g_sidebarWidth = width;
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float h = ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - 8.0f;
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        // Dark obsidian background with subtle vertical separator
        draw->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + h), pal.Sidebar.ToU32(), 0.0f);
        draw->AddLine(ImVec2(pos.x + width, pos.y), ImVec2(pos.x + width, pos.y + h), pal.Border.ToU32(), 1.0f);
        Render::ImGuiExt::DrawSpecularEdge(draw, pos, ImVec2(pos.x + width, pos.y + h), IM_COL32(255, 255, 255, 25), 10.0f, 1.0f);

        ImGui::BeginChild("##SidebarChild", ImVec2(width, h), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::SetCursorPosY(12.0f);
    }

    void Sidebar::End() {
        ImGui::EndChild();
    }

    void Sidebar::Category(const char* label) {
        ImGui::SetCursorPosX(16.0f);
        const auto& pal = ThemeManager::Get().GetPalette();
        auto lv = Render::CleanLabel(label);
        
        // Micro indicator dot
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        draw->AddCircleFilled(ImVec2(p.x - 4.0f, p.y + 7.0f), 2.0f, pal.Accent.WithAlpha(0.7f).ToU32(), 8);

        ImGui::PushStyleColor(ImGuiCol_Text, pal.TextDisabled);
        ImGui::TextUnformatted(lv.textBegin, lv.textEnd);
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

        auto lv = Render::CleanLabel(label);

        ImGuiID imguiId = ImGui::GetID(label);
        ImVec2 p = ImGui::GetCursorScreenPos();

        bool clicked = ImGui::InvisibleButton(label, ImVec2(itemWidth, itemHeight));
        bool hovered = ImGui::IsItemHovered();

        if (clicked && !selected) {
            *currentTab = id;
            Audio::PlayTabSwitch();
        }

        f32 anim = Anim::AnimationManager::Get().Transition(imguiId, selected, 16.0f);
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        ImVec2 min = p;
        ImVec2 max(p.x + itemWidth, p.y + itemHeight);

        // Hover & Active Background Pill
        if (anim > 0.01f) {
            // Multi-tier luminous accent pill
            u32 bgCol = pal.Accent.WithAlpha(0.14f * anim).ToU32();
            draw->AddRectFilled(min, max, bgCol, 6.0f);

            // Subtle luminous border (smooth inset)
            u32 borderCol = pal.Accent.WithAlpha(0.28f * anim).ToU32();
            Render::ImGuiExt::AddSmoothBorder(draw, min, max, borderCol, 6.0f, 1.0f);

            // Left active pill indicator with rounded ends and soft glow
            f32 indH = itemHeight * 0.55f * anim;
            ImVec2 indMin(p.x + 3.0f, p.y + (itemHeight - indH) * 0.5f);
            ImVec2 indMax(p.x + 6.5f, indMin.y + indH);

            // Pill glow
            draw->AddRectFilled(ImVec2(indMin.x - 1.0f, indMin.y - 1.0f), ImVec2(indMax.x + 1.0f, indMax.y + 1.0f),
                                pal.Accent.WithAlpha(0.35f * anim).ToU32(), 3.0f);
            // Main pill
            draw->AddRectFilled(indMin, indMax, pal.Accent.ToU32(), 2.0f);
        } else if (hovered) {
            draw->AddRectFilled(min, max, pal.CardHover.WithAlpha(0.50f).ToU32(), 6.0f);
        }

        // Text & Icon Rendering without ## hash
        Color textCol = selected ? pal.TextPrimary : (hovered ? pal.TextPrimary : pal.TextSecondary);
        float textOffsetX = p.x + 16.0f + (anim * 2.0f);
        float textOffsetY = p.y + (itemHeight - ImGui::GetTextLineHeight()) * 0.5f;

        if (icon) {
            u32 iconCol = selected ? pal.Accent.ToU32() : (hovered ? pal.TextPrimary.ToU32() : pal.TextSecondary.ToU32());
            ImVec2 sz = ImGui::CalcTextSize(icon);
            if (sz.x > 2.0f) {
                draw->AddText(ImVec2(textOffsetX, textOffsetY), iconCol, icon);
            } else {
                Color c = selected ? pal.Accent : (hovered ? pal.TextPrimary : pal.TextSecondary);
                Icons::VectorIconRenderer::DrawByGlyph(draw, icon, ImVec2(textOffsetX + 7.0f, textOffsetY + 7.0f), 13.0f, c);
            }
            textOffsetX += 24.0f;
        }

        draw->AddText(ImVec2(textOffsetX, textOffsetY), textCol.ToU32(), lv.textBegin, lv.textEnd);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
        return clicked;
    }

} // namespace Solar::UI
