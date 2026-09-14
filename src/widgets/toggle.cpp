#include "solar/widgets/toggle.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/render/glow_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>

namespace Solar::Widgets {

    bool Toggle(const char* label, bool* v, const char* description) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        auto lv = Render::CleanLabel(label);

        float height = 22.0f;
        float width = 42.0f;
        float availX = ImGui::GetContentRegionAvail().x;
        float totalH = description ? 36.0f : 24.0f;

        ImVec2 p = window->DC.CursorPos;
        ImRect bb(p, ImVec2(p.x + availX, p.y + totalH));

        ImGui::ItemSize(bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id)) return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
        if (pressed) {
            *v = !(*v);
            Audio::PlayToggle(*v);
            ImGui::MarkItemEdited(id);
        }

        f32 anim = Anim::AnimationManager::Get().Transition(id, *v, 18.0f);
        f32 hoverAnim = Anim::AnimationManager::Get().Transition(id + 1, hovered, 14.0f);

        ImDrawList* draw = window->DrawList;
        const auto& pal = ThemeManager::Get().GetPalette();

        // Optical alignment & vertical centering
        float centerY = p.y + totalH * 0.5f;
        ImVec2 switchPos(p.x + availX - width - 2.0f, centerY - height * 0.5f);
        ImVec2 switchEnd(switchPos.x + width, switchPos.y + height);

        if (description) {
            // Two-line layout: Title at top, description below
            ImVec2 textPos(p.x, p.y + 2.0f);
            draw->AddText(textPos, pal.TextPrimary.ToU32(), lv.textBegin, lv.textEnd);

            ImVec2 descPos(p.x, p.y + lv.size.y + 4.0f);
            draw->AddText(descPos, pal.TextDisabled.ToU32(), description);
        } else {
            // Single-line layout: Exact vertical centerline alignment with switch
            ImVec2 textPos(p.x, centerY - lv.size.y * 0.5f - 0.5f);
            draw->AddText(textPos, pal.TextPrimary.ToU32(), lv.textBegin, lv.textEnd);
        }

        // Recessed Switch Track
        float rounding = height * 0.5f;

        Color offCol = Color::Lerp(Color(0.08f, 0.10f, 0.14f, 1.0f), Color(0.12f, 0.15f, 0.20f, 1.0f), hoverAnim);
        Color trackCol = Color::Lerp(offCol, pal.Accent, anim);

        // Track fill
        draw->AddRectFilled(switchPos, switchEnd, trackCol.ToU32(), rounding);

        // Recessed inner border (smooth inset)
        u32 trackBorder = anim > 0.05f ? pal.AccentActive.ToU32() : IM_COL32(255, 255, 255, static_cast<int>(18 + 20 * hoverAnim));
        Render::ImGuiExt::AddSmoothBorder(draw, switchPos, switchEnd, trackBorder, rounding, 1.0f);

        // Glow bloom when ON
        if (anim > 0.05f && ThemeManager::Get().GetStyle().EnableGlow) {
            Render::GlowEngine::DrawGlowRect(draw, switchPos, switchEnd, pal.Accent, 10.0f, rounding, anim * 0.45f);
        }

        // Smooth Sliding Knob
        float knobRadius = (height - 6.0f) * 0.5f;
        float minKnobX = switchPos.x + 3.0f + knobRadius;
        float maxKnobX = switchEnd.x - 3.0f - knobRadius;
        float knobX = minKnobX + (maxKnobX - minKnobX) * anim;
        float knobY = switchPos.y + height * 0.5f;

        // Knob shadow
        draw->AddCircleFilled(ImVec2(knobX, knobY + 1.2f), knobRadius, IM_COL32(0, 0, 0, 130), 16);
        // Solid white knob core
        draw->AddCircleFilled(ImVec2(knobX, knobY), knobRadius, 0xFFFFFFFF, 16);

        return pressed;
    }

} // namespace Solar::Widgets
