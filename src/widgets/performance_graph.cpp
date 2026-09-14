#include "solar/widgets/performance_graph.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/shadow_caster.hpp"
#include <imgui_internal.h>
#include <algorithm>
#include <cstdio>

namespace Solar::Widgets {

    void PerformanceGraph(const char* label, const float* values, int count,
                          float minScale, float maxScale,
                          const ImVec2& sizeArg,
                          Color accent,
                          const char* unit,
                          GraphMetrics* outMetrics)
    {
        if (!values || count < 2) return;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return;

        ImGuiContext& g = *GImGui;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        float width = sizeArg.x > 0.0f ? sizeArg.x : ImGui::GetContentRegionAvail().x;
        float height = sizeArg.y > 0.0f ? sizeArg.y : 95.0f;

        ImVec2 p = ImGui::GetCursorScreenPos();
        ImRect bb(p, ImVec2(p.x + width, p.y + height));

        ImGuiID id = window->GetID(label);
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return;

        // Calculate Min, Max, Avg, Cur
        float minVal = values[0];
        float maxVal = values[0];
        float sum = 0.0f;
        for (int i = 0; i < count; ++i) {
            float v = values[i];
            if (v < minVal) minVal = v;
            if (v > maxVal) maxVal = v;
            sum += v;
        }
        float avgVal = sum / static_cast<float>(count);
        float curVal = values[count - 1];

        if (outMetrics) {
            outMetrics->minVal = minVal;
            outMetrics->maxVal = maxVal;
            outMetrics->avgVal = avgVal;
            outMetrics->curVal = curVal;
        }

        // Apply scales if defined
        float plotMinVal = (minScale < maxScale) ? minScale : minVal;
        float plotMaxVal = (minScale < maxScale) ? maxScale : maxVal;
        if (plotMaxVal - plotMinVal < 0.0001f) {
            plotMaxVal = plotMinVal + 1.0f;
        }

        float rounding = 8.0f;

        // Container Background & Soft Ambient Shadow
        Render::ShadowCaster::DrawShadow(draw, bb.Min, bb.Max, 10.0f, rounding, Color(0, 0, 0, 0.35f), ImVec2(0, 3.0f));
        draw->AddRectFilled(bb.Min, bb.Max, pal.Card.ToU32(), rounding);
        Render::ImGuiExt::DrawSpecularEdge(draw, bb.Min, bb.Max, IM_COL32(255, 255, 255, 22), rounding, 1.0f);
        Render::ImGuiExt::AddSmoothBorder(draw, bb.Min, bb.Max, pal.Border.ToU32(), rounding, 1.0f);

        // Header Title & Clean Label
        auto lv = Render::CleanLabel(label);
        float headerY = bb.Min.y + 8.0f;
        draw->AddText(ImVec2(bb.Min.x + 10.0f, headerY), pal.TextPrimary.ToU32(), lv.textBegin, lv.textEnd);

        // Header Metric Badges (Cur, Avg)
        char curBuf[32];
        std::snprintf(curBuf, sizeof(curBuf), "%.1f %s", curVal, unit ? unit : "");
        ImVec2 curSize = ImGui::CalcTextSize(curBuf);

        char avgBuf[32];
        std::snprintf(avgBuf, sizeof(avgBuf), "avg %.1f", avgVal);
        ImVec2 avgSize = ImGui::CalcTextSize(avgBuf);

        float badgeX = bb.Max.x - 10.0f;

        // Cur Badge
        badgeX -= curSize.x + 12.0f;
        ImVec2 curPillMin(badgeX, headerY - 1.0f);
        ImVec2 curPillMax(badgeX + curSize.x + 10.0f, headerY + curSize.y + 2.0f);
        draw->AddRectFilled(curPillMin, curPillMax, accent.WithAlpha(0.18f).ToU32(), 4.0f);
        Render::ImGuiExt::AddSmoothBorder(draw, curPillMin, curPillMax, accent.WithAlpha(0.35f).ToU32(), 4.0f, 1.0f);
        draw->AddText(ImVec2(badgeX + 5.0f, headerY), accent.ToU32(), curBuf);

        // Avg Badge
        badgeX -= avgSize.x + 14.0f;
        draw->AddText(ImVec2(badgeX, headerY), pal.TextSecondary.ToU32(), avgBuf);

        // Plot Region
        float plotTop = bb.Min.y + 28.0f;
        float plotBottom = bb.Max.y - 8.0f;
        float plotLeft = bb.Min.x + 10.0f;
        float plotRight = bb.Max.x - 10.0f;
        float plotW = plotRight - plotLeft;
        float plotH = plotBottom - plotTop;

        if (plotW <= 10.0f || plotH <= 10.0f) return;

        // Grid reference lines (25%, 50%, 75%)
        for (int line = 1; line <= 3; ++line) {
            float lineY = plotBottom - plotH * (static_cast<float>(line) * 0.25f);
            draw->AddLine(ImVec2(plotLeft, lineY), ImVec2(plotRight, lineY),
                          pal.Border.WithAlpha(0.20f).ToU32(), 1.0f);
        }

        // Generate plot coordinates
        auto sampleToPos = [&](int i) -> ImVec2 {
            float fracX = static_cast<float>(i) / static_cast<float>(count - 1);
            float normY = (values[i] - plotMinVal) / (plotMaxVal - plotMinVal);
            normY = (std::clamp)(normY, 0.0f, 1.0f);
            return ImVec2(plotLeft + fracX * plotW, plotBottom - normY * plotH);
        };

        // 1. Vertical Gradient Under Curve
        u32 topGradientCol = accent.WithAlpha(0.28f).ToU32();
        u32 btmGradientCol = accent.WithAlpha(0.01f).ToU32();

        for (int i = 0; i < count - 1; ++i) {
            ImVec2 p0 = sampleToPos(i);
            ImVec2 p1 = sampleToPos(i + 1);

            ImVec2 b0(p0.x, plotBottom);
            ImVec2 b1(p1.x, plotBottom);

            // Add quad filled with vertical gradient
            draw->AddQuadFilled(p0, p1, b1, b0, topGradientCol);
        }

        // 2. Glowing Contour Curve
        for (int i = 0; i < count - 1; ++i) {
            ImVec2 p0 = sampleToPos(i);
            ImVec2 p1 = sampleToPos(i + 1);

            // Soft glow aura
            draw->AddLine(p0, p1, accent.WithAlpha(0.22f).ToU32(), 3.5f);
            // Sharp laser core
            draw->AddLine(p0, p1, accent.ToU32(), 1.6f);
        }

        // 3. Interactive Mouse Scrub Cursor
        bool hovered = ImGui::IsMouseHoveringRect(ImVec2(plotLeft, plotTop), ImVec2(plotRight, plotBottom));
        if (hovered) {
            float mouseX = g.IO.MousePos.x;
            float relX = (mouseX - plotLeft) / plotW;
            int scrubIdx = static_cast<int>(relX * static_cast<float>(count - 1) + 0.5f);
            scrubIdx = (std::clamp)(scrubIdx, 0, count - 1);

            ImVec2 pointPos = sampleToPos(scrubIdx);

            // Vertical indicator needle
            draw->AddLine(ImVec2(pointPos.x, plotTop), ImVec2(pointPos.x, plotBottom),
                          IM_COL32(255, 255, 255, 60), 1.0f);

            // Glowing sample node
            draw->AddCircleFilled(pointPos, 5.0f, accent.WithAlpha(0.40f).ToU32());
            draw->AddCircleFilled(pointPos, 2.5f, IM_COL32(255, 255, 255, 255));

            // Floating value badge
            char tipBuf[32];
            std::snprintf(tipBuf, sizeof(tipBuf), "%.2f %s", values[scrubIdx], unit ? unit : "");
            ImVec2 tipSize = ImGui::CalcTextSize(tipBuf);

            float tipX = (std::clamp)(pointPos.x - tipSize.x * 0.5f - 4.0f, plotLeft, plotRight - tipSize.x - 8.0f);
            float tipY = (std::max)(plotTop, pointPos.y - tipSize.y - 8.0f);

            ImVec2 tipMin(tipX, tipY);
            ImVec2 tipMax(tipX + tipSize.x + 8.0f, tipY + tipSize.y + 4.0f);

            draw->AddRectFilled(tipMin, tipMax, pal.Header.ToU32(), 4.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, tipMin, tipMax, accent.ToU32(), 4.0f, 1.0f);
            draw->AddText(ImVec2(tipMin.x + 4.0f, tipMin.y + 2.0f), IM_COL32(255, 255, 255, 255), tipBuf);
        }
    }

} // namespace Solar::Widgets
