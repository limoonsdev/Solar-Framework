#include "solar/ui/subtabs.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include <imgui_internal.h>

namespace Solar::UI {

    bool Subtabs::Tab(const char* label, int id, int* currentSubTab) {
        bool selected = (*currentSubTab == id);
        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 size(textSize.x + 24.0f, 28.0f);

        ImGuiID imguiId = ImGui::GetID((std::string("##Subtab_") + label).c_str());
        ImVec2 p = ImGui::GetCursorScreenPos();

        bool clicked = ImGui::InvisibleButton(label, size);
        bool hovered = ImGui::IsItemHovered();

        if (clicked && !selected) {
            *currentSubTab = id;
            Audio::PlayClick();
        }

        f32 anim = Anim::AnimationManager::Get().Transition(imguiId, selected, 14.0f);
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        if (anim > 0.01f) {
            draw->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), pal.Accent.WithAlpha(0.15f * anim).ToU32(), 4.0f);
            draw->AddLine(ImVec2(p.x + 4.0f, p.y + size.y), ImVec2(p.x + size.x - 4.0f, p.y + size.y), pal.Accent.ToU32(), 2.0f);
        } else if (hovered) {
            draw->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), pal.CardHover.ToU32(), 4.0f);
        }

        u32 col = selected ? pal.TextPrimary.ToU32() : (hovered ? pal.TextPrimary.ToU32() : pal.TextSecondary.ToU32());
        draw->AddText(ImVec2(p.x + 12.0f, p.y + (size.y - textSize.y) * 0.5f), col, label);

        ImGui::SameLine(0, 6.0f);
        return clicked;
    }

} // namespace Solar::UI
