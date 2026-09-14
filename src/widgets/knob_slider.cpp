#include "solar/widgets/knob_slider.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <cmath>
#include <algorithm>
#include <cstdio>

namespace Solar::Widgets {

    bool KnobSlider(const char* label, float* v, float v_min, float v_max,
                    float radius, const char* format, const char* unit) {
        if (!v || v_min >= v_max) return false;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        float diameter = radius * 2.0f;
        float totalHeight = diameter + 32.0f;
        float width = std::max(diameter + 16.0f, ImGui::CalcTextSize(label).x + 8.0f);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImRect bb(pos, ImVec2(pos.x + width, pos.y + totalHeight));

        ImGuiID id = window->GetID(label);
        ImGui::ItemSize(bb, g.Style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id)) return false;

        bool hovered = ImGui::ItemHoverable(bb, id, 0);
        bool held = false;
        bool changed = false;

        if (hovered && ImGui::IsMouseClicked(0)) {
            ImGui::SetActiveID(id, window);
            ImGui::FocusWindow(window);
        }

        if (g.ActiveId == id) {
            held = true;
            if (ImGui::IsMouseDragging(0, 0.0f)) {
                float mouseDeltaY = -g.IO.MouseDelta.y;
                float mouseDeltaX = g.IO.MouseDelta.x;
                float delta = (mouseDeltaY + mouseDeltaX) * 0.005f * (v_max - v_min);
                if (g.IO.KeyShift) delta *= 0.1f;

                float oldVal = *v;
                *v = std::clamp(*v + delta, v_min, v_max);
                if (*v != oldVal) {
                    changed = true;
                }
            }
            if (ImGui::IsMouseReleased(0)) {
                ImGui::ClearActiveID();
                Audio::PlayClick();
            }
        }

        // Center of dial
        ImVec2 center(pos.x + width * 0.5f, pos.y + radius + 4.0f);

        constexpr float PI = 3.14159265358979323846f;
        const float angleMin = 0.75f * PI;
        const float angleMax = 2.25f * PI;
        const float angleSpan = angleMax - angleMin;

        float fraction = (*v - v_min) / (v_max - v_min);
        float currentAngle = angleMin + fraction * angleSpan;

        // Draw track ring (background arc)
        const int numSegments = 32;
        draw->PathClear();
        draw->PathArcTo(center, radius, angleMin, angleMax, numSegments);
        draw->PathStroke(ThemeManager::ToU32(pal.CardHover), 0, 4.0f);

        // Draw active arc
        if (fraction > 0.005f) {
            draw->PathClear();
            draw->PathArcTo(center, radius, angleMin, currentAngle, numSegments);
            draw->PathStroke(ThemeManager::ToU32(pal.Accent), 0, 4.0f);
        }

        // Inner knob dial body
        float innerRadius = radius - 5.0f;
        ImU32 knobBg = (held || hovered) ? ThemeManager::ToU32(pal.CardHover) : ThemeManager::ToU32(pal.Card);
        draw->AddCircleFilled(center, innerRadius, knobBg, 24);
        draw->AddCircle(center, innerRadius, ThemeManager::ToU32(held ? pal.Accent : pal.Border), 24, 1.0f);

        // Indicator line pointing outward
        ImVec2 indicatorStart(
            center.x + std::cos(currentAngle) * (innerRadius * 0.35f),
            center.y + std::sin(currentAngle) * (innerRadius * 0.35f)
        );
        ImVec2 indicatorEnd(
            center.x + std::cos(currentAngle) * (innerRadius * 0.85f),
            center.y + std::sin(currentAngle) * (innerRadius * 0.85f)
        );
        draw->AddLine(indicatorStart, indicatorEnd, ThemeManager::ToU32(pal.Accent), 2.5f);

        // Indicator dot on end
        draw->AddCircleFilled(indicatorEnd, 2.0f, ThemeManager::ToU32(pal.AccentHover), 8);

        // Value text
        char valBuf[64];
        char fmtBuf[64];
        snprintf(fmtBuf, sizeof(fmtBuf), "%s %s", format, unit);
        snprintf(valBuf, sizeof(valBuf), fmtBuf, *v);
        ImVec2 valSz = ImGui::CalcTextSize(valBuf);
        draw->AddText(ImVec2(center.x - valSz.x * 0.5f, center.y + radius + 3.0f),
                      ThemeManager::ToU32(pal.TextPrimary), valBuf);

        // Label text
        ImVec2 lblSz = ImGui::CalcTextSize(label);
        draw->AddText(ImVec2(center.x - lblSz.x * 0.5f, center.y + radius + 17.0f),
                      ThemeManager::ToU32(pal.TextSecondary), label);

        return changed;
    }

} // namespace Solar::Widgets
