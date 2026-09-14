#include "solar/widgets/slider.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/core/math.hpp"
#include "solar/render/glow_engine.hpp"
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

        ImVec2 p = window->DC.CursorPos;
        ImRect bb(p, ImVec2(p.x + availX, p.y + height));

        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return false;

        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        // Label on top
        draw->AddText(p, pal.TextPrimary.ToU32(), label);

        // Value text on top right
        char valBuf[64];
        if (suffix) {
            char fmtBuf[64];
            snprintf(fmtBuf, sizeof(fmtBuf), "%s %s", format, suffix);
            snprintf(valBuf, sizeof(valBuf), fmtBuf, *v);
        } else {
            snprintf(valBuf, sizeof(valBuf), format, *v);
        }
        ImVec2 valSize = ImGui::CalcTextSize(valBuf);
        draw->AddText(ImVec2(p.x + availX - valSize.x - 4.0f, p.y), pal.Accent.ToU32(), valBuf);

        // Track rect
        float trackY = p.y + 24.0f;
        float trackH = 6.0f;
        ImVec2 trackMin(p.x, trackY);
        ImVec2 trackMax(p.x + availX - 4.0f, trackY + trackH);
        float trackW = trackMax.x - trackMin.x;

        ImRect trackBB(trackMin, trackMax);
        bool hovered, held;
        bool changed = false;

        ImRect hitBB(ImVec2(trackMin.x, trackMin.y - 6.0f), ImVec2(trackMax.x, trackMax.y + 6.0f));
        ImGui::ButtonBehavior(hitBB, id, &hovered, &held);

        if (held) {
            float mouseX = g.IO.MousePos.x;
            float norm = Math::Clamp((mouseX - trackMin.x) / trackW, 0.0f, 1.0f);
            float newVal = v_min + norm * (v_max - v_min);
            if (std::abs(*v - newVal) > 0.001f) {
                *v = newVal;
                changed = true;
                ImGui::MarkItemEdited(id);
            }
        }

        float normVal = Math::Clamp((*v - v_min) / (v_max - v_min), 0.0f, 1.0f);

        // Track background
        draw->AddRectFilled(trackMin, trackMax, pal.Background.ToU32(), trackH * 0.5f);

        // Active track fill
        ImVec2 fillMax(trackMin.x + trackW * normVal, trackMax.y);
        draw->AddRectFilled(trackMin, fillMax, pal.Accent.ToU32(), trackH * 0.5f);

        // Thumb handle
        float thumbRadius = 6.5f;
        ImVec2 thumbCenter(fillMax.x, trackMin.y + trackH * 0.5f);
        draw->AddCircleFilled(thumbCenter, thumbRadius, 0xFFFFFFFF, 16);

        if (hovered || held) {
            Render::GlowEngine::DrawGlowCircle(draw, thumbCenter, thumbRadius, pal.Accent, 8.0f, 1.0f);
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
