#include "solar/widgets/slider.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/core/math.hpp"
#include "solar/render/glow_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>
#include <cstdio>

namespace Solar::Widgets {

    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, const char* suffix) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(label);
        float availX = ImGui::GetContentRegionAvail().x;
        float height = 38.0f;

        auto lv = Render::CleanLabel(label);

        ImVec2 p = window->DC.CursorPos;
        ImRect bb(p, ImVec2(p.x + availX, p.y + height));

        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return false;

        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        // Label on top without ## hash
        draw->AddText(p, pal.TextPrimary.ToU32(), lv.textBegin, lv.textEnd);

        // Value text pill on top right
        char valBuf[64];
        if (suffix) {
            char fmtBuf[64];
            snprintf(fmtBuf, sizeof(fmtBuf), "%s %s", format, suffix);
            snprintf(valBuf, sizeof(valBuf), fmtBuf, *v);
        } else {
            snprintf(valBuf, sizeof(valBuf), format, *v);
        }
        ImVec2 valSize = ImGui::CalcTextSize(valBuf);
        ImVec2 valPillMin(p.x + availX - valSize.x - 14.0f, p.y - 1.0f);
        ImVec2 valPillMax(p.x + availX - 2.0f, p.y + valSize.y + 3.0f);

        // Value pill background
        draw->AddRectFilled(valPillMin, valPillMax, pal.CardHover.WithAlpha(0.70f).ToU32(), 4.0f);
        draw->AddRect(valPillMin, valPillMax, pal.Border.WithAlpha(0.60f).ToU32(), 4.0f, 0, 1.0f);
        draw->AddText(ImVec2(valPillMin.x + 6.0f, valPillMin.y + 2.0f), pal.Accent.ToU32(), valBuf);

        // Recessed track
        float trackY = p.y + 25.0f;
        float trackH = 5.0f;
        ImVec2 trackMin(p.x, trackY);
        ImVec2 trackMax(p.x + availX - 4.0f, trackY + trackH);
        float trackW = trackMax.x - trackMin.x;

        ImRect hitBB(ImVec2(trackMin.x, trackMin.y - 8.0f), ImVec2(trackMax.x, trackMax.y + 8.0f));
        bool hovered, held;
        bool changed = false;
        ImGui::ButtonBehavior(hitBB, id, &hovered, &held);

        if (held) {
            float mouseX = g.IO.MousePos.x;
            float norm = Math::Clamp((mouseX - trackMin.x) / trackW, 0.0f, 1.0f);
            float newVal = v_min + norm * (v_max - v_min);
            if (std::abs(*v - newVal) > 0.0001f) {
                *v = newVal;
                changed = true;
                ImGui::MarkItemEdited(id);
            }
        }

        f32 hoverAnim = Anim::AnimationManager::Get().Transition(id, hovered || held, 16.0f);

        float normVal = Math::Clamp((*v - v_min) / (v_max - v_min), 0.0f, 1.0f);
        float rounding = trackH * 0.5f;

        // Track background groove with shadow
        draw->AddRectFilled(trackMin, trackMax, IM_COL32(11, 13, 18, 255), rounding);
        draw->AddRect(trackMin, trackMax, IM_COL32(0, 0, 0, 150), rounding, 0, 1.0f);

        // Active track gradient fill
        ImVec2 fillMax(trackMin.x + trackW * normVal, trackMax.y);
        if (fillMax.x > trackMin.x) {
            u32 col1 = pal.Accent.ToU32();
            u32 col2 = pal.AccentActive.ToU32();
            draw->AddRectFilledMultiColor(trackMin, fillMax, col1, col2, col2, col1);
        }

        // Animated Thumb handle
        float baseRadius = 6.5f;
        float thumbRadius = baseRadius + hoverAnim * 1.5f;
        ImVec2 thumbCenter(fillMax.x, trackMin.y + trackH * 0.5f);

        // Thumb drop shadow
        draw->AddCircleFilled(ImVec2(thumbCenter.x, thumbCenter.y + 1.2f), thumbRadius, IM_COL32(0, 0, 0, 140), 16);

        // Thumb outer glow on hover
        if (hoverAnim > 0.05f) {
            Render::GlowEngine::DrawGlowCircle(draw, thumbCenter, thumbRadius, pal.Accent, 8.0f, hoverAnim * 0.70f);
        }

        // Thumb core
        draw->AddCircleFilled(thumbCenter, thumbRadius, 0xFFFFFFFF, 16);
        draw->AddCircle(thumbCenter, thumbRadius, pal.Accent.ToU32(), 16, 1.2f);

        // Floating Tooltip Bubble when dragging
        if (held) {
            ImVec2 tipSize = ImGui::CalcTextSize(valBuf);
            ImVec2 tipMin(thumbCenter.x - tipSize.x * 0.5f - 7.0f, trackMin.y - 24.0f);
            ImVec2 tipMax(thumbCenter.x + tipSize.x * 0.5f + 7.0f, trackMin.y - 6.0f);

            // Bubble shadow & background
            draw->AddRectFilled(ImVec2(tipMin.x, tipMin.y + 2.0f), ImVec2(tipMax.x, tipMax.y + 2.0f), IM_COL32(0, 0, 0, 120), 4.0f);
            draw->AddRectFilled(tipMin, tipMax, pal.Header.ToU32(), 4.0f);
            draw->AddRect(tipMin, tipMax, pal.Accent.ToU32(), 4.0f, 0, 1.0f);

            // Small triangle pointer pointing down to thumb
            ImVec2 triA(thumbCenter.x - 3.0f, tipMax.y);
            ImVec2 triB(thumbCenter.x + 3.0f, tipMax.y);
            ImVec2 triC(thumbCenter.x, tipMax.y + 3.5f);
            draw->AddTriangleFilled(triA, triB, triC, pal.Header.ToU32());

            // Bubble text
            draw->AddText(ImVec2(tipMin.x + 7.0f, tipMin.y + 2.0f), pal.TextPrimary.ToU32(), valBuf);
        }

        return changed;
    }

    bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, const char* suffix) {
        float f = static_cast<float>(*v);
        bool res = SliderFloat(label, &f, static_cast<float>(v_min), static_cast<float>(v_max), format, suffix);
        if (res) *v = static_cast<int>(std::round(f));
        return res;
    }

} // namespace Solar::Widgets
