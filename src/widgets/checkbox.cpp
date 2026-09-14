#include "solar/widgets/checkbox.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>
#include <cmath>

namespace Solar::Widgets {

    bool Checkbox(const char* label, bool* v) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        auto lv = Render::CleanLabel(label);

        float boxSize = 19.0f;
        ImVec2 p = window->DC.CursorPos;

        float totalWidth = boxSize + (lv.size.x > 0.0f ? (style.ItemInnerSpacing.x + 8.0f + lv.size.x) : 0.0f);
        float totalHeight = (std::max)(boxSize, lv.size.y);

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

        f32 anim = Anim::AnimationManager::Get().Transition(id, *v, 18.0f);
        f32 hoverAnim = Anim::AnimationManager::Get().Transition(id + 1, hovered, 14.0f);

        ImDrawList* draw = window->DrawList;
        const auto& pal = ThemeManager::Get().GetPalette();

        ImVec2 boxMin(p.x, p.y + (totalHeight - boxSize) * 0.5f);
        ImVec2 boxMax(boxMin.x + boxSize, boxMin.y + boxSize);
        float rounding = 5.5f;

        // 1. Subtle soft shadow under checkbox
        draw->AddRectFilled(
            ImVec2(boxMin.x, boxMin.y + 1.0f),
            ImVec2(boxMax.x, boxMax.y + 1.5f),
            IM_COL32(0, 0, 0, 80),
            rounding
        );

        // 2. Background Fill
        if (anim > 0.01f) {
            // Luminous Accent fill with subtle vertical gradient
            u32 topCol = pal.AccentHover.WithAlpha(anim).ToU32();
            u32 botCol = pal.Accent.WithAlpha(anim).ToU32();
            draw->AddRectFilledMultiColor(boxMin, boxMax, topCol, topCol, botCol, botCol);

            // Subtle outer micro-glow
            float glowAlpha = 0.28f * anim;
            draw->AddRect(
                ImVec2(boxMin.x - 1.5f, boxMin.y - 1.5f),
                ImVec2(boxMax.x + 1.5f, boxMax.y + 1.5f),
                pal.Accent.WithAlpha(glowAlpha).ToU32(),
                rounding + 1.5f, 0, 1.2f
            );
        } else {
            // Dark Obsidian background
            u32 bgCol = hovered ? IM_COL32(24, 26, 34, 255) : IM_COL32(16, 17, 22, 255);
            draw->AddRectFilled(boxMin, boxMax, bgCol, rounding);
        }

        // 3. Smooth Border
        u32 borderCol;
        if (anim > 0.05f) {
            borderCol = pal.AccentActive.ToU32();
        } else if (hovered) {
            borderCol = pal.Accent.WithAlpha(0.50f * hoverAnim).ToU32();
        } else {
            borderCol = IM_COL32(255, 255, 255, 22);
        }
        draw->AddRect(boxMin, boxMax, borderCol, rounding, 0, 1.2f);

        // 4. Smooth Animated Vector Checkmark
        if (anim > 0.01f) {
            float cx = boxMin.x + boxSize * 0.5f;
            float cy = boxMin.y + boxSize * 0.5f;

            ImVec2 p1(cx - 4.5f, cy - 0.5f);
            ImVec2 p2(cx - 1.5f, cy + 3.2f);
            ImVec2 p3(cx + 4.8f, cy - 3.8f);

            u32 checkCol = IM_COL32(255, 255, 255, static_cast<int>(255 * anim));

            if (anim < 0.45f) {
                float t = anim / 0.45f;
                ImVec2 curP2(p1.x + (p2.x - p1.x) * t, p1.y + (p2.y - p1.y) * t);
                draw->AddLine(p1, curP2, checkCol, 2.2f);
            } else {
                draw->AddLine(p1, p2, checkCol, 2.2f);
                float t = (anim - 0.45f) / 0.55f;
                ImVec2 curP3(p2.x + (p3.x - p2.x) * t, p2.y + (p3.y - p2.y) * t);
                draw->AddLine(p2, curP3, checkCol, 2.2f);
            }
        }

        // 5. Label Typography without ## hash
        if (lv.size.x > 0.0f) {
            u32 textCol = (*v) ? pal.TextPrimary.ToU32() : (hovered ? pal.TextPrimary.ToU32() : pal.TextSecondary.ToU32());
            draw->AddText(ImVec2(boxMax.x + 8.0f, p.y + (totalHeight - lv.size.y) * 0.5f), textCol, lv.textBegin, lv.textEnd);
        }

        return pressed;
    }

} // namespace Solar::Widgets
