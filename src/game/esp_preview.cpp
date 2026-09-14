#include "solar/game/esp_preview.hpp"
#include "solar/game/visuals_renderer.hpp"
#include "solar/game/obb_renderer.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/core/math.hpp"
#include <imgui.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>

namespace Solar::Game {

    static constexpr float DEG2RAD = 0.01745329251f;

    void ESPPreview::Render(const char* str_id, const ImVec2& size, ESPSettings& s) {
        ImGui::BeginChild(str_id, size, false, ImGuiWindowFlags_NoScrollbar);
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 sz = ImGui::GetContentRegionAvail();
        if (sz.x < 10.0f) sz.x = size.x;
        if (sz.y < 10.0f) sz.y = size.y;

        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();
        float timeSec = static_cast<float>(ImGui::GetTime());
        ImGuiIO& io = ImGui::GetIO();

        // 1. Invisible Interactive Drag Button for 360-degree Orbit
        ImGui::InvisibleButton("##3DOrbitInteraction", sz);
        bool isActive = ImGui::IsItemActive();
        bool isHovered = ImGui::IsItemHovered();

        if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            s.orbitYaw += io.MouseDelta.x * 0.75f;
            s.orbitPitch += io.MouseDelta.y * 0.5f;
            s.orbitPitch = (std::clamp)(s.orbitPitch, -45.0f, 45.0f);
        }

        // 2. Dark Obsidian Viewport Canvas
        draw->AddRectFilled(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), IM_COL32(10, 11, 15, 255), 6.0f);
        draw->AddRect(pos, ImVec2(pos.x + sz.x, pos.y + sz.y),
                      isActive ? pal.Accent.ToU32() : IM_COL32(255, 255, 255, 18), 6.0f);

        // Tactical range rings in backdrop
        ImVec2 center(pos.x + sz.x * 0.5f, pos.y + sz.y * 0.52f);
        draw->AddCircle(center, 95.0f, IM_COL32(255, 255, 255, 6), 36, 1.0f);
        draw->AddCircle(center, 140.0f, IM_COL32(255, 255, 255, 5), 48, 1.0f);

        // 3. Stance Height & Dimensions Configuration
        float boxW = 105.0f;
        float boxH = 210.0f;
        float crouchShift = 0.0f;

        if (s.stance == 1) { // Crouching
            boxH = 160.0f;
            crouchShift = 25.0f;
        } else if (s.stance == 3) { // Jumping
            crouchShift = -18.0f;
        }

        ImVec2 boxMin(center.x - boxW * 0.5f, center.y - boxH * 0.5f + crouchShift);
        ImVec2 boxMax(center.x + boxW * 0.5f, center.y + boxH * 0.5f + crouchShift);

        Color boxColor = s.isFriendly ? Color(0.24f, 0.72f, 1.0f, 1.0f) : Color(s.boxColor.x, s.boxColor.y, s.boxColor.z, s.boxColor.w);
        Color skelColor = s.isFriendly ? Color(0.6f, 0.9f, 1.0f, 0.85f) : Color(s.skeletonColor.x, s.skeletonColor.y, s.skeletonColor.z, s.skeletonColor.w);

        // 4. Acoustic Wave Rings at Feet (Sound ESP)
        if (s.enableAcousticWaves) {
            float wavePulse = std::fmod(timeSec * 45.0f, 75.0f);
            float waveAlpha = (1.0f - wavePulse / 75.0f);
            VisualsRenderer::DrawAcousticWave(draw, ImVec2(center.x, boxMax.y), wavePulse, wavePulse * 0.35f, 0.0f,
                                              pal.Accent.WithAlpha(waveAlpha * 0.8f), 1.5f);
        }

        // 5. 3D Oriented Bounding Box (OBB)
        if (s.enable3DBox) {
            OBB3D obb;
            obb.screenCenter = ImVec2(center.x, center.y + crouchShift);
            obb.width = boxW;
            obb.height = boxH;
            obb.depth = boxW * 0.7f;
            obb.yawDeg = s.orbitYaw;
            obb.pitchDeg = s.orbitPitch;
            obb.rollDeg = 0.0f;

            OBBRenderer::Render(draw, obb, boxColor, boxColor.WithAlpha(0.12f), 1.5f, true);
        }

        // 6. Targeting Snapline
        if (s.enableSnapline) {
            VisualsRenderer::DrawSnapline(draw, ImVec2(center.x, boxMax.y), SnaplineOrigin::ScreenBottom, boxColor, 1.4f, false);
        }

        // 7. 3D Rotational Skeleton Transform
        float radY = s.orbitYaw * DEG2RAD;
        float cosY = std::cos(radY), sinY = std::sin(radY);

        auto RotatePt = [&](float lx, float ly, float lz) -> ImVec2 {
            float rx = lx * cosY + lz * sinY;
            float rz = -lx * sinY + lz * cosY;
            float scale = 320.0f / (320.0f + rz);
            return ImVec2(center.x + rx * scale, center.y + (ly + crouchShift) * scale);
        };

        if (s.enableSkeleton) {
            float headY = -boxH * 0.5f + 20.0f;
            float neckY = headY + 16.0f;
            float spineY = neckY + 45.0f;
            float pelvisY = spineY + 30.0f;

            ImVec2 head = RotatePt(0, headY, 0);
            ImVec2 neck = RotatePt(0, neckY, 0);
            ImVec2 spine = RotatePt(0, spineY, 0);
            ImVec2 pelvis = RotatePt(0, pelvisY, 0);

            float shoulderW = (s.stance == 2) ? 18.0f : 24.0f; // Scoped narrower shoulders
            ImVec2 lShoulder = RotatePt(-shoulderW, neckY + 6.0f, 0);
            ImVec2 rShoulder = RotatePt(shoulderW, neckY + 6.0f, 0);

            float armZ = (s.stance == 2) ? 18.0f : 8.0f;
            ImVec2 lElbow = RotatePt(-28.0f, neckY + 38.0f, armZ);
            ImVec2 rElbow = RotatePt(28.0f, neckY + 38.0f, armZ);
            ImVec2 lHand = RotatePt(-14.0f, neckY + 62.0f, armZ + 12.0f);
            ImVec2 rHand = RotatePt(18.0f, neckY + 62.0f, armZ + 12.0f);

            float kneeY = pelvisY + ((s.stance == 1) ? 22.0f : 42.0f);
            float footY = boxH * 0.5f - 4.0f;
            float legSpread = (s.stance == 1) ? 22.0f : 16.0f;

            ImVec2 lKnee = RotatePt(-legSpread, kneeY, 0);
            ImVec2 rKnee = RotatePt(legSpread, kneeY, 0);
            ImVec2 lFoot = RotatePt(-legSpread - 2.0f, footY, 0);
            ImVec2 rFoot = RotatePt(legSpread + 2.0f, footY, 0);

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

            // Weapon Vector in hands
            ImVec2 barrelTip = RotatePt(12.0f, neckY + 54.0f, armZ + 42.0f);
            draw->AddLine(lHand, barrelTip, IM_COL32(200, 200, 215, 220), 2.8f);
            draw->AddLine(rHand, barrelTip, IM_COL32(200, 200, 215, 220), 2.8f);

            // Barrel Ray (Line of sight forward vector)
            if (s.enableBarrelRay) {
                ImVec2 rayEnd = RotatePt(12.0f, neckY + 54.0f, armZ + 120.0f);
                VisualsRenderer::DrawLineOfSightTracer(draw, barrelTip, rayEnd,
                                                      pal.Accent.WithAlpha(0.85f), pal.Accent, 1.5f);
            }
        }

        // 8. 2D Bounding Boxes (Standard or Visuals 2.0 Glow/Gradient)
        if (s.enableBox && !s.enable3DBox) {
            if (s.enableGlowOutline) {
                VisualsRenderer::DrawGlowOutlineBox2D(draw, boxMin, boxMax, boxColor, pal.Accent, 10.0f, 1.5f);
            } else if (s.boxType == 3) { // Gradient Box
                Color colGradBot(s.boxGradientBottom.x, s.boxGradientBottom.y, s.boxGradientBottom.z, s.boxGradientBottom.w);
                VisualsRenderer::DrawGradientBox2D(draw, boxMin, boxMax, boxColor, colGradBot, 1.5f);
            } else {
                BoxStyle style = BoxStyle::Full;
                if (s.boxType == 1) style = BoxStyle::Corner;
                else if (s.boxType == 2) style = BoxStyle::Filled;
                VisualsRenderer::DrawBoundingBox2D(draw, boxMin, boxMax, style, boxColor, Color(0, 0, 0, 0.9f), 14.0f, 1.5f);
            }
        }

        // 9. Status Bars (Health & Armor)
        if (s.enableHealthBar) {
            VisualsRenderer::DrawHealthBar(draw, boxMin, boxMax, s.health, 100.0f, BarPosition::Left, true, true);
        }
        if (s.enableArmorBar) {
            VisualsRenderer::DrawArmorBar(draw, boxMin, boxMax, s.armor, 100.0f, BarPosition::Left);
        }

        // 10. Typography Elements
        if (s.enableName) {
            VisualsRenderer::DrawNameTag(draw, ImVec2(center.x, boxMin.y - 4.0f), s.playerName,
                                         s.isFriendly ? Color(0.24f, 0.72f, 1.0f, 1.0f) : Color(1, 1, 1, 1));
        }
        if (s.enableWeapon) {
            VisualsRenderer::DrawWeaponTag(draw, ImVec2(center.x, boxMax.y + 8.0f), s.weaponName, 25, 75);
        }
        if (s.enableDistance) {
            VisualsRenderer::DrawDistanceTag(draw, ImVec2(center.x, boxMax.y + 24.0f), s.distance);
        }

        // 11. Stacked Badge Flags
        std::vector<std::pair<std::string, Color>> flags;
        if (s.stance == 2) flags.push_back({ "SCOPED", Color(0.25f, 0.72f, 1.0f, 1.0f) });
        if (s.stance == 4) flags.push_back({ "DEFUSING", Color(1.0f, 0.25f, 0.25f, 1.0f) });
        if (s.armor > 0.0f) flags.push_back({ "ARMOR", Color(0.35f, 0.85f, 0.40f, 1.0f) });
        flags.push_back({ "KIT", Color(0.85f, 0.40f, 0.95f, 1.0f) });
        VisualsRenderer::DrawFlagTags(draw, boxMax, flags);

        // 12. 3D Orbital HUD Guidance Overlay
        char orbitText[64];
        snprintf(orbitText, sizeof(orbitText), "Drag to Orbit: Yaw %.0f deg | Pitch %.0f deg", s.orbitYaw, s.orbitPitch);
        ImVec2 ts = ImGui::CalcTextSize(orbitText);
        draw->AddText(ImVec2(pos.x + 8.0f, pos.y + sz.y - ts.y - 6.0f),
                      isHovered ? pal.Accent.ToU32() : IM_COL32(255, 255, 255, 100), orbitText);

        ImGui::EndChild();
    }

} // namespace Solar::Game
