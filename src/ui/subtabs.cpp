#include "solar/ui/subtabs.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>

namespace Solar::UI {

    bool Subtabs::Tab(const char* label, int id, int* currentSubTab) {
        bool selected = (*currentSubTab == id);
        auto lv = Render::CleanLabel(label);

        ImVec2 size(lv.size.x + 28.0f, 32.0f);

        ImGuiID imguiId = ImGui::GetID(label);
        ImVec2 p = ImGui::GetCursorScreenPos();

        bool clicked = ImGui::InvisibleButton(label, size);
        bool hovered = ImGui::IsItemHovered();

        if (clicked && !selected) {
            *currentSubTab = id;
            Audio::PlayClick();
        }

        f32 anim = Anim::AnimationManager::Get().Transition(imguiId, selected, 16.0f);
        f32 hoverAnim = Anim::AnimationManager::Get().Transition(imguiId + 1, hovered, 14.0f);

        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        ImVec2 min = p;
        ImVec2 max(p.x + size.x, p.y + size.y);
        float rounding = 6.0f;

        // Active / Hovered Pill Styling
        if (anim > 0.01f) {
            // Subtle glowing background
            draw->AddRectFilled(min, max, pal.Accent.WithAlpha(0.14f * anim).ToU32(), rounding);
            draw->AddRect(min, max, pal.Accent.WithAlpha(0.32f * anim).ToU32(), rounding, 0, 1.0f);

            // Glowing Bottom Accent Indicator Bar
            float barW = (size.x - 14.0f) * anim;
            ImVec2 barMin(p.x + (size.x - barW) * 0.5f, p.y + size.y - 2.5f);
            ImVec2 barMax(barMin.x + barW, p.y + size.y);

            // Glow under bar
            draw->AddRectFilled(ImVec2(barMin.x, barMin.y - 1.0f), ImVec2(barMax.x, barMax.y + 1.0f),
                                pal.Accent.WithAlpha(0.40f * anim).ToU32(), 2.0f);
            draw->AddRectFilled(barMin, barMax, pal.Accent.ToU32(), 2.0f);
        } else if (hovered) {
            draw->AddRectFilled(min, max, pal.CardHover.WithAlpha(0.40f * hoverAnim).ToU32(), rounding);
            draw->AddRect(min, max, IM_COL32(255, 255, 255, static_cast<int>(16 * hoverAnim)), rounding, 0, 1.0f);
        }

        u32 col = selected ? pal.TextPrimary.ToU32() : (hovered ? pal.TextPrimary.ToU32() : pal.TextSecondary.ToU32());
        draw->AddText(ImVec2(p.x + (size.x - lv.size.x) * 0.5f, p.y + (size.y - lv.size.y) * 0.5f - 1.0f), col, lv.textBegin, lv.textEnd);

        ImGui::SameLine(0, 8.0f);
        return clicked;
    }

} // namespace Solar::UI
