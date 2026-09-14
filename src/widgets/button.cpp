#include "solar/widgets/button.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/render/glow_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>

namespace Solar::Widgets {

    bool Button(const char* label, const ImVec2& sizeArg, ButtonStyle style) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(label);

        auto lv = Render::CleanLabel(label);

        ImVec2 size = sizeArg;
        if (size.x <= 0.0f) size.x = ImGui::GetContentRegionAvail().x;
        if (size.y <= 0.0f) size.y = 34.0f;

        ImVec2 p = window->DC.CursorPos;
        ImRect bb(p, ImVec2(p.x + size.x, p.y + size.y));

        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

        if (pressed) {
            Audio::PlayClick();
        }

        f32 anim = Anim::AnimationManager::Get().Transition(id, hovered, 16.0f);
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

        float rounding = 6.0f;

        // Subtle drop shadow under secondary & primary buttons
        if (hovered && style != ButtonStyle::Ghost) {
            draw->AddRectFilled(ImVec2(p.x, p.y + 1.5f), ImVec2(p.x + size.x, p.y + size.y + 1.5f),
                                IM_COL32(0, 0, 0, 90), rounding);
        }

        draw->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), baseBg.ToU32(), rounding);

        // Top specular highlight line for glass feel
        if (style != ButtonStyle::Ghost) {
            draw->AddLine(ImVec2(p.x + rounding, p.y + 0.5f), ImVec2(p.x + size.x - rounding, p.y + 0.5f),
                          IM_COL32(255, 255, 255, style == ButtonStyle::Primary ? 65 : 25), 1.0f);
        }

        draw->AddRect(p, ImVec2(p.x + size.x, p.y + size.y), borderCol.ToU32(), rounding, 0, 1.0f);

        if (style == ButtonStyle::Primary && hovered && ThemeManager::Get().GetStyle().EnableGlow) {
            Render::GlowEngine::DrawGlowRect(draw, p, ImVec2(p.x + size.x, p.y + size.y), pal.Accent, 10.0f, rounding, anim * 0.50f);
        }

        // Render clean label (without ## ID)
        float textYOffset = held ? 1.0f : 0.0f;
        ImVec2 tp(p.x + (size.x - lv.size.x) * 0.5f, p.y + (size.y - lv.size.y) * 0.5f + textYOffset);
        draw->AddText(tp, textCol.ToU32(), lv.textBegin, lv.textEnd);

        return pressed;
    }

} // namespace Solar::Widgets
