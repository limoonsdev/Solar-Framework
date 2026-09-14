#include "solar/widgets/button.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/render/glow_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/shadow_caster.hpp"
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

        // Tactile micro-depression physics when clicked/held
        float pressInset = held ? 0.65f : 0.0f;
        float pressY = held ? 0.75f : 0.0f;
        ImVec2 btnMin(p.x + pressInset, p.y + pressY + pressInset);
        ImVec2 btnMax(p.x + size.x - pressInset, p.y + size.y + pressY - pressInset);

        // Soft ambient shadow under interactive buttons
        if (hovered && !held && style != ButtonStyle::Ghost) {
            Render::ShadowCaster::DrawShadow(draw, btnMin, btnMax, 8.0f, rounding, Color(0, 0, 0, 0.35f * anim), ImVec2(0, 2.5f));
        }

        // Body fill
        draw->AddRectFilled(btnMin, btnMax, baseBg.ToU32(), rounding);

        // Top specular sheen reflection
        if (style != ButtonStyle::Ghost) {
            Render::ImGuiExt::DrawSpecularEdge(draw, btnMin, btnMax,
                IM_COL32(255, 255, 255, style == ButtonStyle::Primary ? 40 : 20), rounding, 1.0f);
        }

        // Smooth pixel-aligned inset border
        Render::ImGuiExt::AddSmoothBorder(draw, btnMin, btnMax, borderCol.ToU32(), rounding, 1.0f);

        if (style == ButtonStyle::Primary && hovered && ThemeManager::Get().GetStyle().EnableGlow) {
            Render::GlowEngine::DrawGlowRect(draw, btnMin, btnMax, pal.Accent, 10.0f, rounding, anim * 0.45f);
        }

        // Render clean label (without ## ID)
        ImVec2 tp(btnMin.x + ((btnMax.x - btnMin.x) - lv.size.x) * 0.5f,
                  btnMin.y + ((btnMax.y - btnMin.y) - lv.size.y) * 0.5f);
        draw->AddText(tp, textCol.ToU32(), lv.textBegin, lv.textEnd);

        return pressed;
    }

} // namespace Solar::Widgets
