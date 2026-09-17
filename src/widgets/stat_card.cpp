#include "solar/widgets/stat_card.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/shadow_caster.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <algorithm>
#include <vector>

namespace Solar::Widgets {

    void StatCard(const char* title, const char* value,
                  const char* deltaText, bool deltaPositive,
                  const float* sparklineData, int sparklineCount,
                  float width, float height) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return;

        ImGuiContext& g = *GImGui;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        if (width <= 0.0f) {
            width = ImGui::GetContentRegionAvail().x;
        }

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImRect bb(pos, ImVec2(pos.x + width, pos.y + height));

        ImGuiID id = window->GetID(title);
        ImGui::ItemSize(bb, g.Style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id)) return;

        bool hovered = ImGui::ItemHoverable(bb, id, 0);

        // Background & Ambient Shadow
        ImU32 bgCol = hovered ? ThemeManager::ToU32(pal.CardHover) : ThemeManager::ToU32(pal.Card);
        ImU32 borderCol = hovered ? ThemeManager::ToU32(pal.Accent) : ThemeManager::ToU32(pal.Border);
        Render::ShadowCaster::DrawShadow(draw, bb.Min, bb.Max, 8.0f, 8.0f, Color(0, 0, 0, 0.30f), ImVec2(0, 2.0f));
        draw->AddRectFilled(bb.Min, bb.Max, bgCol, 8.0f);
        Render::ImGuiExt::DrawSpecularEdge(draw, bb.Min, bb.Max, IM_COL32(255, 255, 255, 22), 8.0f, 1.0f);
        Render::ImGuiExt::AddSmoothBorder(draw, bb.Min, bb.Max, borderCol, 8.0f, 1.0f);

        // Title on top left
        draw->AddText(ImVec2(bb.Min.x + 12.0f, bb.Min.y + 10.0f),
                      ThemeManager::ToU32(pal.TextSecondary), title);

        // Delta badge on top right
        if (deltaText && deltaText[0] != '\0') {
            ImVec2 dSz = ImGui::CalcTextSize(deltaText);
            float badgeW = dSz.x + 12.0f;
            float badgeH = dSz.y + 4.0f;
            ImVec2 bMin(bb.Max.x - 12.0f - badgeW, bb.Min.y + 10.0f);
            ImVec2 bMax(bb.Max.x - 12.0f, bMin.y + badgeH);

            ImU32 badgeBg = deltaPositive ? IM_COL32(34, 197, 94, 40) : IM_COL32(239, 68, 68, 40);
            ImU32 badgeFg = deltaPositive ? IM_COL32(74, 222, 128, 255) : IM_COL32(248, 113, 113, 255);

            draw->AddRectFilled(bMin, bMax, badgeBg, 4.0f);
            draw->AddText(ImVec2(bMin.x + 6.0f, bMin.y + 2.0f), badgeFg, deltaText);
        }

        // Value text
        draw->AddText(ImVec2(bb.Min.x + 12.0f, bb.Min.y + 32.0f),
                      ThemeManager::ToU32(pal.TextPrimary), value);

        // Sparkline graph docked cleanly on the right half to avoid overlapping value text
        if (sparklineData && sparklineCount > 1) {
            float sparkH = 22.0f;
            float sparkMinY = bb.Max.y - 10.0f - sparkH;
            float sparkMaxY = bb.Max.y - 10.0f;
            float sparkW = (std::min)(width * 0.42f, 115.0f);
            float sparkMaxX = bb.Max.x - 14.0f;
            float sparkMinX = sparkMaxX - sparkW;

            float minV = sparklineData[0];
            float maxV = sparklineData[0];
            for (int i = 1; i < sparklineCount; ++i) {
                if (sparklineData[i] < minV) minV = sparklineData[i];
                if (sparklineData[i] > maxV) maxV = sparklineData[i];
            }
            if (maxV <= minV) maxV = minV + 1.0f;

            float stepX = sparkW / float(sparklineCount - 1);
            std::vector<ImVec2> pts;
            pts.reserve(sparklineCount);

            for (int i = 0; i < sparklineCount; ++i) {
                float norm = (sparklineData[i] - minV) / (maxV - minV);
                float px = sparkMinX + i * stepX;
                float py = sparkMaxY - norm * sparkH;
                pts.push_back(ImVec2(px, py));
            }

            // Draw sparkline curve
            ImU32 lineCol = ThemeManager::ToU32(pal.Accent);
            for (size_t i = 0; i + 1 < pts.size(); ++i) {
                draw->AddLine(pts[i], pts[i + 1], lineCol, 1.5f);
            }

            // Glowing dot on the latest data point
            draw->AddCircleFilled(pts.back(), 3.0f, ThemeManager::ToU32(pal.AccentHover), 12);
        }
    }

} // namespace Solar::Widgets
