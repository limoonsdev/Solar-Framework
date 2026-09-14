#include "solar/widgets/checkbox.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/render/glow_engine.hpp"
#include <imgui_internal.h>

namespace Solar::Widgets {

    bool Checkbox(const char* label, bool* v) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        float boxSize = 18.0f;
        ImVec2 labelSize = ImGui::CalcTextSize(label, nullptr, true);
        ImVec2 p = window->DC.CursorPos;

        float totalWidth = boxSize + (labelSize.x > 0.0f ? (style.ItemInnerSpacing.x + labelSize.x) : 0.0f);
        float totalHeight = std::max(boxSize, labelSize.y);

        ImRect totalBB(p, ImVec2(p.x + totalWidth, p.y + totalHeight));
        ImGui::ItemSize(totalBB, style.FramePadding.y);
        if (!ImGui::ItemAdd(totalBB, id)) return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(totalBB, id, &hovered, &held);
        if (pressed) {
            *v = !(*v);
            Audio::PlayToggle(*v);
            ImGui::MarkItemEdited(id);
        }

        f32 anim = Anim::AnimationManager::Get().Transition(id, *v, 16.0f);
        f32 hoverAnim = Anim::AnimationManager::Get().Transition(id + 1, hovered, 12.0f);

        ImDrawList* draw = window->DrawList;
        const auto& pal = ThemeManager::Get().GetPalette();

        ImVec2 boxMin(p.x, p.y + (totalHeight - boxSize) * 0.5f);
        ImVec2 boxMax(boxMin.x + boxSize, boxMin.y + boxSize);

        Color bgCol = Color::Lerp(pal.Background, pal.Accent, anim);
        if (anim < 0.05f && hoverAnim > 0.01f) {
            bgCol = Color::Lerp(pal.Background, pal.CardHover, hoverAnim);
        }
        draw->AddRectFilled(boxMin, boxMax, bgCol.ToU32(), 4.0f);

        Color borderCol = Color::Lerp(pal.Border, pal.AccentHover, anim);
        if (anim < 0.05f && hoverAnim > 0.01f) {
            borderCol = Color::Lerp(pal.Border, pal.BorderHover, hoverAnim);
        }
        draw->AddRect(boxMin, boxMax, borderCol.ToU32(), 4.0f, 0, 1.2f);

        if (anim > 0.05f && ThemeManager::Get().GetStyle().EnableGlow) {
            Render::GlowEngine::DrawGlowRect(draw, boxMin, boxMax, pal.Accent, 7.0f, 4.0f, anim);
        }

        if (anim > 0.01f) {
            ImVec2 p1(boxMin.x + 4.5f, boxMin.y + 9.5f);
            ImVec2 p2(boxMin.x + 7.5f, boxMin.y + 13.0f);
            ImVec2 p3(boxMin.x + 13.5f, boxMin.y + 5.5f);

            u32 checkCol = Color(1.0f, 1.0f, 1.0f, anim).ToU32();
            if (anim < 0.5f) {
                float t = anim / 0.5f;
                ImVec2 currentP2(p1.x + (p2.x - p1.x) * t, p1.y + (p2.y - p1.y) * t);
                draw->AddLine(p1, currentP2, checkCol, 2.0f);
            } else {
                draw->AddLine(p1, p2, checkCol, 2.0f);
                float t = (anim - 0.5f) / 0.5f;
                ImVec2 currentP3(p2.x + (p3.x - p2.x) * t, p2.y + (p3.y - p2.y) * t);
                draw->AddLine(p2, currentP3, checkCol, 2.0f);
            }
        }

        if (labelSize.x > 0.0f) {
            ImVec2 textPos(boxMax.x + style.ItemInnerSpacing.x, p.y + (totalHeight - labelSize.y) * 0.5f);
            Color textCol = hovered ? pal.TextPrimary : pal.TextSecondary;
            draw->AddText(textPos, textCol.ToU32(), label);
        }

        return pressed;
    }

} // namespace Solar::Widgets
