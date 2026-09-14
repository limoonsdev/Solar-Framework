#include "solar/game/esp_preview.hpp"
#include "solar/game/visuals_renderer.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/core/math.hpp"
#include <imgui.h>
#include <cmath>
#include <cstdio>
#include <vector>

namespace Solar::Game {

    void ESPPreview::Render(const char* str_id, const ImVec2& size, const ESPSettings& s) {
        ImGui::BeginChild(str_id, size, false, ImGuiWindowFlags_NoScrollbar);
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 sz = ImGui::GetWindowSize();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();
        float timeSec = static_cast<float>(ImGui::GetTime());

        // 1. Dark Tactical Viewport Canvas
        draw->AddRectFilled(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), IM_COL32(10, 11, 15, 255), 6.0f);
        draw->AddRect(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), IM_COL32(255, 255, 255, 18), 6.0f);

        // Subtle tactical radar range circles in backdrop
        ImVec2 center(pos.x + sz.x * 0.5f, pos.y + sz.y * 0.52f);
        draw->AddCircle(center, 90.0f, IM_COL32(255, 255, 255, 6), 36, 1.0f);
        draw->AddCircle(center, 135.0f, IM_COL32(255, 255, 255, 5), 48, 1.0f);

        // Mannequin Bounding Box
        float boxW = 105.0f;
        float boxH = 210.0f;
        ImVec2 boxMin(center.x - boxW * 0.5f, center.y - boxH * 0.5f);
        ImVec2 boxMax(center.x + boxW * 0.5f, center.y + boxH * 0.5f);

        Color boxColor(s.boxColor.x, s.boxColor.y, s.boxColor.z, s.boxColor.w);
        Color skelColor(s.skeletonColor.x, s.skeletonColor.y, s.skeletonColor.z, s.skeletonColor.w);

        // 2. Snapline
        if (s.enableSnapline) {
            VisualsRenderer::DrawSnapline(draw, ImVec2(center.x, boxMax.y), SnaplineOrigin::ScreenBottom, boxColor, 1.4f, false);
        }

        // 3. Viewcone directional indicator (pointing forward)
        {
            float coneAngle = -1.5707963f + std::sin(timeSec * 1.5f) * 0.15f;
            float cosA = std::cos(coneAngle);
            float sinA = std::sin(coneAngle);
            ImVec2 headPos(center.x, boxMin.y + 22.0f);
            ImVec2 coneTip(headPos.x + cosA * 28.0f, headPos.y + sinA * 28.0f);
            draw->AddLine(headPos, coneTip, pal.Accent.WithAlpha(0.6f).ToU32(), 1.4f);
            draw->AddCircleFilled(coneTip, 2.5f, pal.Accent.ToU32(), 8);
        }

        // 4. Detailed Tactical Skeleton
        if (s.enableSkeleton) {
            ImVec2 head(center.x, boxMin.y + 22.0f);
            ImVec2 neck(center.x, boxMin.y + 36.0f);
            ImVec2 spine(center.x, boxMin.y + 88.0f);
            ImVec2 pelvis(center.x, boxMin.y + 118.0f);

            ImVec2 lShoulder(center.x - 24.0f, boxMin.y + 44.0f);
            ImVec2 rShoulder(center.x + 24.0f, boxMin.y + 44.0f);
            ImVec2 lElbow(center.x - 30.0f, boxMin.y + 82.0f);
            ImVec2 rElbow(center.x + 30.0f, boxMin.y + 82.0f);
            ImVec2 lHand(center.x - 22.0f, boxMin.y + 112.0f);
            ImVec2 rHand(center.x + 22.0f, boxMin.y + 112.0f);

            ImVec2 lKnee(center.x - 16.0f, boxMin.y + 158.0f);
            ImVec2 rKnee(center.x + 16.0f, boxMin.y + 158.0f);
            ImVec2 lFoot(center.x - 18.0f, boxMax.y - 4.0f);
            ImVec2 rFoot(center.x + 18.0f, boxMax.y - 4.0f);

            std::vector<std::pair<ImVec2, ImVec2>> bones = {
                { neck, spine }, { spine, pelvis },
                { neck, lShoulder }, { neck, rShoulder },
                { lShoulder, lElbow }, { rShoulder, rElbow },
                { lElbow, lHand }, { rElbow, rHand },
                { pelvis, lKnee }, { pelvis, rKnee },
                { lKnee, lFoot }, { rKnee, rFoot }
            };

            VisualsRenderer::DrawHeadCircle(draw, head, 11.0f, skelColor, Color(0, 0, 0, 0.9f), true);
            VisualsRenderer::DrawSkeleton(draw, bones, skelColor, 1.8f, true, Color(1, 1, 1, 0.95f));

            // Weapon vector silhouette in hands
            draw->AddLine(lHand, ImVec2(center.x + 8.0f, boxMin.y + 104.0f), IM_COL32(200, 200, 215, 220), 2.8f);
            draw->AddLine(rHand, ImVec2(center.x + 8.0f, boxMin.y + 104.0f), IM_COL32(200, 200, 215, 220), 2.8f);
            draw->AddLine(ImVec2(center.x - 12.0f, boxMin.y + 104.0f), ImVec2(center.x + 36.0f, boxMin.y + 104.0f), IM_COL32(240, 240, 255, 230), 2.4f);
        }

        // 5. Bounding Box
        if (s.enableBox) {
            BoxStyle style = BoxStyle::Full;
            if (s.boxType == 1) style = BoxStyle::Corner;
            else if (s.boxType == 2) style = BoxStyle::Filled;
            VisualsRenderer::DrawBoundingBox2D(draw, boxMin, boxMax, style, boxColor, Color(0, 0, 0, 0.9f), 14.0f, 1.5f);
        }

        // 6. Status Bars (Health & Armor)
        if (s.enableHealthBar) {
            VisualsRenderer::DrawHealthBar(draw, boxMin, boxMax, s.health, 100.0f, BarPosition::Left, true, true);
        }
        if (s.enableArmorBar) {
            VisualsRenderer::DrawArmorBar(draw, boxMin, boxMax, s.armor, 100.0f, BarPosition::Left);
        }

        // 7. Typography Elements
        if (s.enableName) {
            VisualsRenderer::DrawNameTag(draw, ImVec2(center.x, boxMin.y - 4.0f), s.playerName);
        }
        if (s.enableWeapon) {
            VisualsRenderer::DrawWeaponTag(draw, ImVec2(center.x, boxMax.y + 8.0f), s.weaponName, 25, 75);
        }
        if (s.enableDistance) {
            VisualsRenderer::DrawDistanceTag(draw, ImVec2(center.x, boxMax.y + 24.0f), s.distance);
        }

        // 8. Stacked Badge Flags
        std::vector<std::pair<std::string, Color>> flags = {
            { "SCOPED", Color(0.25f, 0.72f, 1.0f, 1.0f) },
            { "FLASHED", Color(1.0f, 0.78f, 0.15f, 1.0f) },
            { "ARMOR", Color(0.35f, 0.85f, 0.40f, 1.0f) },
            { "KIT", Color(0.85f, 0.40f, 0.95f, 1.0f) }
        };
        VisualsRenderer::DrawFlagTags(draw, boxMax, flags);

        ImGui::EndChild();
    }

} // namespace Solar::Game
