#include "solar/widgets/button.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/render/glow_engine.hpp"
#include <imgui_internal.h>

namespace Solar::Widgets {

    bool Button(const char* label, const ImVec2& sizeArg, ButtonStyle style) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(label);

        ImVec2 size = sizeArg;
        if (size.x <= 0.0f) size.x = ImGui::GetContentRegionAvail().x;
        if (size.y <= 0.0f) size.y = 36.0f;

        ImVec2 p = window->DC.CursorPos;
        ImRect bb(p, ImVec2(p.x + size.x, p.y + size.y));

        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

        if (pressed) {
            Audio::PlayClick();
        }

        f32 anim = Anim::AnimationManager::Get().Transition(id, hovered, 14.0f);
        ImDrawList* draw = window->DrawList;
        const auto& pal = ThemeManager::Get().GetPalette();

        Color baseBg;
        Color borderCol = pal.Border;
        Color textCol = pal.TextPrimary;

        switch (style) {
        case ButtonStyle::Primary:
            baseBg = Color::Lerp(pal.Accent, pal.AccentHover, anim);
            borderCol = pal.AccentActive;
            textCol = Color(1, 1, 1, 1);
            break;
        case ButtonStyle::Secondary:
            baseBg = Color::Lerp(pal.Card, pal.CardHover, anim);
            borderCol = Color::Lerp(pal.Border, pal.BorderHover, anim);
            break;
        case ButtonStyle::Danger:
            baseBg = Color::Lerp(pal.Danger.WithAlpha(0.2f), pal.Danger, anim);
            borderCol = pal.Danger;
            break;
        case ButtonStyle::Ghost:
            baseBg = pal.CardHover.WithAlpha(0.25f * anim);
            borderCol = pal.Border.WithAlpha(anim);
            break;
        }

        draw->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), baseBg.ToU32(), 5.0f);
        draw->AddRect(p, ImVec2(p.x + size.x, p.y + size.y), borderCol.ToU32(), 5.0f);

        if (style == ButtonStyle::Primary && hovered && ThemeManager::Get().GetStyle().EnableGlow) {
            Render::GlowEngine::DrawGlowRect(draw, p, ImVec2(p.x + size.x, p.y + size.y), pal.Accent, 10.0f, 5.0f, anim);
        }

        ImVec2 ts = ImGui::CalcTextSize(label);
        ImVec2 tp(p.x + (size.x - ts.x) * 0.5f, p.y + (size.y - ts.y) * 0.5f);
        draw->AddText(tp, textCol.ToU32(), label);

        return pressed;
    }

} // namespace Solar::Widgets
