#include "solar/widgets/range_slider.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>
#include <algorithm>
#include <cstdio>

namespace Solar::Widgets {

    bool RangeSlider(const char* label, float* vMin, float* vMax,
                     float minLimit, float maxLimit,
                     const char* format, const char* unit) {
        if (!vMin || !vMax || minLimit >= maxLimit) return false;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        float width = ImGui::GetContentRegionAvail().x;
        float height = 38.0f;
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImRect bb(pos, ImVec2(pos.x + width, pos.y + height));

        ImGuiID id = window->GetID(label);
        ImGui::ItemSize(bb, g.Style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id)) return false;

        // Label and Value Readout
        char valBuf[64];
        char fmtBuf[64];
        snprintf(fmtBuf, sizeof(fmtBuf), "%s - %s %s", format, format, unit);
        snprintf(valBuf, sizeof(valBuf), fmtBuf, *vMin, *vMax);

        auto lv = Render::CleanLabel(label);
        draw->AddText(pos, ThemeManager::ToU32(pal.TextPrimary), lv.textBegin, lv.textEnd);
        ImVec2 valTs = ImGui::CalcTextSize(valBuf);
        draw->AddText(ImVec2(pos.x + width - valTs.x, pos.y), ThemeManager::ToU32(pal.Accent), valBuf);

        // Track bar
        float trackY = pos.y + 24.0f;
        float trackH = 4.0f;
        float trackMinX = pos.x + 8.0f;
        float trackMaxX = pos.x + width - 8.0f;
        float trackW = trackMaxX - trackMinX;

        draw->AddRectFilled(ImVec2(trackMinX, trackY), ImVec2(trackMaxX, trackY + trackH),
                            ThemeManager::ToU32(pal.Border), 2.0f);

        float range = maxLimit - minLimit;
        float normMin = std::clamp((*vMin - minLimit) / range, 0.0f, 1.0f);
        float normMax = std::clamp((*vMax - minLimit) / range, 0.0f, 1.0f);

        float thumb1X = trackMinX + normMin * trackW;
        float thumb2X = trackMinX + normMax * trackW;

        // Highlight active range
        draw->AddRectFilled(ImVec2(thumb1X, trackY), ImVec2(thumb2X, trackY + trackH),
                            ThemeManager::ToU32(pal.Accent), 2.0f);

        // Dragging logic
        bool changed = false;
        bool hovered = ImGui::IsMouseHoveringRect(ImVec2(pos.x, trackY - 8.0f), ImVec2(pos.x + width, trackY + trackH + 8.0f));
        static int activeThumb = 0; // 1 = min, 2 = max

        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            float mouseX = g.IO.MousePos.x;
            float dist1 = std::abs(mouseX - thumb1X);
            float dist2 = std::abs(mouseX - thumb2X);
            activeThumb = (dist1 < dist2) ? 1 : 2;
        }

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && activeThumb != 0) {
            float mouseX = g.IO.MousePos.x;
            float norm = std::clamp((mouseX - trackMinX) / trackW, 0.0f, 1.0f);
            float val = minLimit + norm * range;

            if (activeThumb == 1) {
                if (val > *vMax) val = *vMax;
                if (*vMin != val) {
                    *vMin = val;
                    changed = true;
                }
            } else if (activeThumb == 2) {
                if (val < *vMin) val = *vMin;
                if (*vMax != val) {
                    *vMax = val;
                    changed = true;
                }
            }
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            activeThumb = 0;
        }

        // Draw Dual Thumbs
        float thumbRadius = 6.0f;
        draw->AddCircleFilled(ImVec2(thumb1X, trackY + trackH * 0.5f), thumbRadius, ThemeManager::ToU32(pal.Accent), 16);
        draw->AddCircle(ImVec2(thumb1X, trackY + trackH * 0.5f), thumbRadius, IM_COL32(255, 255, 255, 200), 16, 1.2f);

        draw->AddCircleFilled(ImVec2(thumb2X, trackY + trackH * 0.5f), thumbRadius, ThemeManager::ToU32(pal.Accent), 16);
        draw->AddCircle(ImVec2(thumb2X, trackY + trackH * 0.5f), thumbRadius, IM_COL32(255, 255, 255, 200), 16, 1.2f);

        return changed;
    }

} // namespace Solar::Widgets
