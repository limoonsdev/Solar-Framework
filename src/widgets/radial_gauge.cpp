#include "solar/widgets/radial_gauge.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/core/math.hpp"
#include <imgui_internal.h>
#include <cmath>
#include <cstdio>
#include <algorithm>

namespace Solar::Widgets {

    bool RadialGauge(const char* label, float* v, float v_min, float v_max,
                     float radius, const char* format, const char* unit) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        const ImGuiID id = window->GetID(label);
        float diameter = radius * 2.0f;
        float totalH = diameter + 28.0f;
        ImVec2 p = window->DC.CursorPos;
        float availW = ImGui::GetContentRegionAvail().x;
        float totalW = (std::max)(diameter + 20.0f, availW);

        ImRect bb(p, ImVec2(p.x + totalW, p.y + totalH));
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return false;

        ImVec2 center(p.x + totalW * 0.5f, p.y + radius + 4.0f);
        ImDrawList* draw = window->DrawList;
        const auto& pal = ThemeManager::Get().GetPalette();

        // 240-degree arc: from 150 deg (5*PI/6) to 390 deg (13*PI/6)
        const float startAngle = 150.0f * (3.14159265f / 180.0f);
        const float sweepAngle = 240.0f * (3.14159265f / 180.0f);
        const float endAngle = startAngle + sweepAngle;

        ImRect hitBB(ImVec2(center.x - radius - 6.0f, center.y - radius - 6.0f),
                     ImVec2(center.x + radius + 6.0f, center.y + radius + 6.0f));
        bool hovered, held;
        bool changed = false;
        ImGui::ButtonBehavior(hitBB, id, &hovered, &held);

        float normVal = Math::Clamp((*v - v_min) / (v_max - v_min), 0.0f, 1.0f);

        if (held) {
            ImVec2 mPos = ImGui::GetIO().MousePos;
            float angle = std::atan2(mPos.y - center.y, mPos.x - center.x);
            if (angle < 0.0f) angle += 6.2831853f;

            // Map angle to normVal
            float relAngle = angle - startAngle;
            if (relAngle < 0.0f) relAngle += 6.2831853f;
            if (relAngle <= sweepAngle) {
                float newNorm = Math::Clamp(relAngle / sweepAngle, 0.0f, 1.0f);
                float newVal = v_min + newNorm * (v_max - v_min);
                if (std::abs(*v - newVal) > 0.0001f) {
                    *v = newVal;
                    normVal = newNorm;
                    changed = true;
                    ImGui::MarkItemEdited(id);
                    Audio::PlaySliderTick();
                }
            }
        }

        // 1. Background Arc Track
        const int arcSegments = 36;
        float trackThickness = 6.0f;
        draw->PathClear();
        draw->PathArcTo(center, radius, startAngle, endAngle, arcSegments);
        draw->PathStroke(IM_COL32(18, 22, 32, 230), 0, trackThickness);

        // 2. Graduation Ticks
        for (int i = 0; i <= 8; ++i) {
            float t = static_cast<float>(i) / 8.0f;
            float a = startAngle + t * sweepAngle;
            float rIn = radius - 7.0f;
            float rOut = radius - 2.0f;
            ImVec2 t0(center.x + std::cos(a) * rIn, center.y + std::sin(a) * rIn);
            ImVec2 t1(center.x + std::cos(a) * rOut, center.y + std::sin(a) * rOut);
            draw->AddLine(t0, t1, IM_COL32(255, 255, 255, (i == 0 || i == 8 || i == 4) ? 90 : 35), 1.0f);
        }

        // 3. Active Colored Fill Arc
        if (normVal > 0.01f) {
            float curAngle = startAngle + normVal * sweepAngle;
            draw->PathClear();
            draw->PathArcTo(center, radius, startAngle, curAngle, (std::max)(2, static_cast<int>(arcSegments * normVal)));
            draw->PathStroke(pal.Accent.ToU32(), 0, trackThickness);

            // Active Head Pip
            ImVec2 pipPos(center.x + std::cos(curAngle) * radius, center.y + std::sin(curAngle) * radius);
            draw->AddCircleFilled(pipPos, 5.0f, pal.Accent.WithAlpha(0.35f).ToU32(), 12);
            draw->AddCircleFilled(pipPos, 3.2f, IM_COL32(255, 255, 255, 255), 12);
            draw->AddCircle(pipPos, 3.2f, pal.Accent.ToU32(), 12, 1.2f);
        }

        // 4. Center Numerical Readout
        char valStr[64];
        if (unit) {
            char fmtStr[48];
            std::snprintf(fmtStr, sizeof(fmtStr), "%s %s", format, unit);
            std::snprintf(valStr, sizeof(valStr), fmtStr, *v);
        } else {
            std::snprintf(valStr, sizeof(valStr), format, *v);
        }
        ImVec2 valSz = ImGui::CalcTextSize(valStr);
        draw->AddText(ImVec2(center.x - valSz.x * 0.5f, center.y - valSz.y * 0.5f - 2.0f),
                      pal.TextPrimary.ToU32(), valStr);

        // 5. Label Underneath
        auto lv = Render::CleanLabel(label);
        ImVec2 lblSz = ImGui::CalcTextSize(lv.textBegin, lv.textEnd);
        draw->AddText(ImVec2(center.x - lblSz.x * 0.5f, center.y + radius * 0.45f + 8.0f),
                      hovered ? pal.Accent.ToU32() : pal.TextDisabled.ToU32(), lv.textBegin, lv.textEnd);

        return changed;
    }

} // namespace Solar::Widgets
