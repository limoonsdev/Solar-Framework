#include "solar/widgets/toggle.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/render/glow_engine.hpp"
#include <imgui_internal.h>

namespace Solar::Widgets {

    bool Toggle(const char* label, bool* v, const char* description) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        float height = 20.0f;
        float width = 36.0f;
        float availX = ImGui::GetContentRegionAvail().x;

        ImVec2 p = window->DC.CursorPos;
        ImRect bb(p, ImVec2(p.x + availX, p.y + height + (description ? 14.0f : 0.0f)));

        ImGui::ItemSize(bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id)) return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
        if (pressed) {
            *v = !(*v);
            Audio::PlayToggle(*v);
            ImGui::MarkItemEdited(id);
        }

        f32 anim = Anim::AnimationManager::Get().Transition(id, *v, 16.0f);
        ImDrawList* draw = window->DrawList;
        const auto& pal = ThemeManager::Get().GetPalette();

        // Label
        ImVec2 textPos(p.x, p.y + (height - ImGui::GetFontSize()) * 0.5f);
        draw->AddText(textPos, pal.TextPrimary.ToU32(), label);

        if (description) {
            ImVec2 descPos(p.x, p.y + height + 1.0f);
            draw->AddText(descPos, pal.TextDisabled.ToU32(), description);
        }

        // Switch track
        ImVec2 switchPos(p.x + availX - width - 4.0f, p.y);
        ImVec2 switchEnd(switchPos.x + width, switchPos.y + height);

        Color trackCol = Color::Lerp(pal.CardHover, pal.Accent, anim);
        draw->AddRectFilled(switchPos, switchEnd, trackCol.ToU32(), height * 0.5f);

        // Glow when ON
        if (anim > 0.05f && ThemeManager::Get().GetStyle().EnableGlow) {
            Render::GlowEngine::DrawGlowRect(draw, switchPos, switchEnd, pal.Accent, 8.0f, height * 0.5f, anim);
        }

        // Knob circle
        float knobRadius = (height - 6.0f) * 0.5f;
        float minKnobX = switchPos.x + 3.0f + knobRadius;
        float maxKnobX = switchEnd.x - 3.0f - knobRadius;
        float knobX = minKnobX + (maxKnobX - minKnobX) * anim;
        float knobY = switchPos.y + height * 0.5f;

        draw->AddCircleFilled(ImVec2(knobX, knobY), knobRadius, 0xFFFFFFFF, 16);

        return pressed;
    }

} // namespace Solar::Widgets
