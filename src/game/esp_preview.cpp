#include "solar/game/esp_preview.hpp"
#include "solar/game/visuals_renderer.hpp"
#include "solar/game/obb_renderer.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/imgui_ext.hpp"
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
            s.orbitPitch = (std::clamp)(s.orbitPitch, -35.0f, 35.0f);
        }

        // 2. Dark Obsidian Viewport Canvas with Inner Shadow
        draw->AddRectFilled(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), IM_COL32(9, 10, 14, 255), 8.0f);
        draw->AddRect(pos, ImVec2(pos.x + sz.x, pos.y + sz.y),
                      isActive ? pal.Accent.ToU32() : (isHovered ? IM_COL32(255, 255, 255, 32) : IM_COL32(255, 255, 255, 14)), 8.0f, 0, 1.0f);

        ImVec2 center(pos.x + sz.x * 0.5f, pos.y + sz.y * 0.48f);

        // 3. Stance Height & Dimensions Configuration
        float boxW = 100.0f;
        float boxH = 205.0f;
        float crouchShift = 0.0f;

        if (s.stance == 1) { // Crouching
            boxH = 155.0f;
            crouchShift = 25.0f;
        } else if (s.stance == 3) { // Jumping
            crouchShift = -18.0f;
        }

        ImVec2 boxMin(center.x - boxW * 0.5f, center.y - boxH * 0.5f + crouchShift);
        ImVec2 boxMax(center.x + boxW * 0.5f, center.y + boxH * 0.5f + crouchShift);

        Color boxColor = s.isFriendly ? Color(0.24f, 0.72f, 1.0f, 1.0f) : Color(s.boxColor.x, s.boxColor.y, s.boxColor.z, s.boxColor.w);
        Color skelColor = s.isFriendly ? Color(0.6f, 0.9f, 1.0f, 0.85f) : Color(s.skeletonColor.x, s.skeletonColor.y, s.skeletonColor.z, s.skeletonColor.w);

        // 4. 3D Projector with Euler Yaw & Pitch
        float radYaw = s.orbitYaw * DEG2RAD;
        float radPitch = s.orbitPitch * DEG2RAD;
        float cosY = std::cos(radYaw), sinY = std::sin(radYaw);
        float cosP = std::cos(radPitch), sinP = std::sin(radPitch);

        auto Project3D = [&](float lx, float ly, float lz) -> ImVec2 {
            // Rotate Yaw (around Y axis)
            float rx = lx * cosY + lz * sinY;
            float rz = -lx * sinY + lz * cosY;
            // Rotate Pitch (around X axis)
            float ry = ly * cosP - rz * sinP;
            float rzFinal = ly * sinP + rz * cosP;
            // Perspective Projection
            float scale = 340.0f / (340.0f + rzFinal);
            return ImVec2(center.x + rx * scale, center.y + ry * scale);
        };

        // 5. 3D Perspective Floor Grid
        float floorY = boxH * 0.5f + crouchShift + 6.0f;
        for (int gx = -3; gx <= 3; ++gx) {
            float lx = static_cast<float>(gx) * 26.0f;
            ImVec2 gP1 = Project3D(lx, floorY, -75.0f);
            ImVec2 gP2 = Project3D(lx, floorY, 75.0f);
            draw->AddLine(gP1, gP2, IM_COL32(255, 255, 255, (gx == 0) ? 35 : 16), 1.0f);
        }
        for (int gz = -2; gz <= 2; ++gz) {
            float lz = static_cast<float>(gz) * 35.0f;
            ImVec2 gP1 = Project3D(-78.0f, floorY, lz);
            ImVec2 gP2 = Project3D(78.0f, floorY, lz);
            float alphaFade = (std::clamp)(1.0f - (lz + 75.0f) / 150.0f, 0.2f, 1.0f);
            draw->AddLine(gP1, gP2, IM_COL32(255, 255, 255, static_cast<int>(24 * alphaFade)), 1.0f);
        }

        // 6. Acoustic Wave Rings at Feet (Sound ESP)
        if (s.enableAcousticWaves) {
            float wavePulse = std::fmod(timeSec * 45.0f, 75.0f);
            float waveAlpha = (1.0f - wavePulse / 75.0f);
            ImVec2 feetPos = Project3D(0.0f, floorY, 0.0f);
            VisualsRenderer::DrawAcousticWave(draw, feetPos, wavePulse, wavePulse * 0.35f, 0.0f,
                                              pal.Accent.WithAlpha(waveAlpha * 0.8f), 1.5f);
        }

        // 7. 3D Oriented Bounding Box (OBB)
        if (s.enable3DBox) {
            OBB3D obb;
            obb.screenCenter = ImVec2(center.x, center.y + crouchShift);
            obb.width = boxW;
            obb.height = boxH;
            obb.depth = boxW * 0.65f;
            obb.yawDeg = s.orbitYaw;
            obb.pitchDeg = s.orbitPitch;
            obb.rollDeg = 0.0f;

            OBBRenderer::Render(draw, obb, boxColor, boxColor.WithAlpha(0.10f), 1.5f, true);
        }

        // 8. Targeting Snapline
        if (s.enableSnapline) {
            VisualsRenderer::DrawSnapline(draw, ImVec2(center.x, boxMax.y), SnaplineOrigin::ScreenBottom, boxColor, 1.4f, false);
        }

        // 9. Volumetric 3D Skeleton
        if (s.enableSkeleton) {
            float headY = -boxH * 0.5f + 20.0f + crouchShift;
            float neckY = headY + 16.0f;
            float spineY = neckY + 45.0f;
            float pelvisY = spineY + 30.0f;

            ImVec2 head = Project3D(0, headY, 0);
            ImVec2 neck = Project3D(0, neckY, 0);
            ImVec2 spine = Project3D(0, spineY, 0);
            ImVec2 pelvis = Project3D(0, pelvisY, 0);

            float shoulderW = (s.stance == 2) ? 18.0f : 24.0f;
            ImVec2 lShoulder = Project3D(-shoulderW, neckY + 6.0f, 0);
            ImVec2 rShoulder = Project3D(shoulderW, neckY + 6.0f, 0);

            float armZ = (s.stance == 2) ? 18.0f : 8.0f;
            ImVec2 lElbow = Project3D(-28.0f, neckY + 38.0f, armZ);
            ImVec2 rElbow = Project3D(28.0f, neckY + 38.0f, armZ);
            ImVec2 lHand = Project3D(-14.0f, neckY + 62.0f, armZ + 12.0f);
            ImVec2 rHand = Project3D(18.0f, neckY + 62.0f, armZ + 12.0f);

            float kneeY = pelvisY + ((s.stance == 1) ? 22.0f : 42.0f);
            float footY = floorY - 6.0f;
            float legSpread = (s.stance == 1) ? 22.0f : 16.0f;

            ImVec2 lKnee = Project3D(-legSpread, kneeY, 0);
            ImVec2 rKnee = Project3D(legSpread, kneeY, 0);
            ImVec2 lFoot = Project3D(-legSpread - 2.0f, footY, 0);
            ImVec2 rFoot = Project3D(legSpread + 2.0f, footY, 0);

            std::vector<std::pair<ImVec2, ImVec2>> bones = {
                { neck, spine }, { spine, pelvis },
                { neck, lShoulder }, { neck, rShoulder },
                { lShoulder, lElbow }, { rShoulder, rElbow },
                { lElbow, lHand }, { rElbow, rHand },
                { pelvis, lKnee }, { pelvis, rKnee },
                { lKnee, lFoot }, { rKnee, rFoot }
            };

            // Render Volumetric Capsule Limbs
            for (const auto& bone : bones) {
                // Outer shadow
                draw->AddLine(bone.first, bone.second, IM_COL32(0, 0, 0, 200), 4.2f);
                // Core volumetric limb
                draw->AddLine(bone.first, bone.second, skelColor.ToU32(), 2.4f);
                // Spherical joint node
                draw->AddCircleFilled(bone.first, 3.0f, IM_COL32(255, 255, 255, 220), 12);
                draw->AddCircle(bone.first, 3.0f, skelColor.ToU32(), 12, 1.0f);
            }

            // Head Spherical Node with Visor
            VisualsRenderer::DrawHeadCircle(draw, head, 11.5f, skelColor, Color(0.08f, 0.09f, 0.13f, 0.95f), true);

            // Weapon Vector in hands
            ImVec2 barrelTip = Project3D(12.0f, neckY + 54.0f, armZ + 42.0f);
            draw->AddLine(lHand, barrelTip, IM_COL32(190, 195, 210, 240), 2.8f);
            draw->AddLine(rHand, barrelTip, IM_COL32(190, 195, 210, 240), 2.8f);

            // Barrel Ray (Line of sight forward vector)
            if (s.enableBarrelRay) {
                ImVec2 rayEnd = Project3D(12.0f, neckY + 54.0f, armZ + 120.0f);
                VisualsRenderer::DrawLineOfSightTracer(draw, barrelTip, rayEnd,
                                                      pal.Accent.WithAlpha(0.85f), pal.Accent, 1.5f);
            }
        }

        // 10. 2D Bounding Boxes
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

        // 11. Status Bars (Health & Armor) on Left
        if (s.enableHealthBar) {
            VisualsRenderer::DrawHealthBar(draw, boxMin, boxMax, s.health, 100.0f, BarPosition::Left, true, true);
        }
        if (s.enableArmorBar) {
            VisualsRenderer::DrawArmorBar(draw, boxMin, boxMax, s.armor, 100.0f, BarPosition::Left);
        }

        // 12. Non-Overlapping HUD Flags
        if (s.enableName) {
            VisualsRenderer::DrawNameTag(draw, ImVec2(center.x, boxMin.y - 18.0f), s.playerName,
                                         s.isFriendly ? Color(0.24f, 0.72f, 1.0f, 1.0f) : Color(1, 1, 1, 1));
        }

        float bottomY = boxMax.y + 6.0f;
        if (s.enableWeapon) {
            std::string wName = s.weaponName;
            if (wName.find('[') != std::string::npos) {
                VisualsRenderer::DrawWeaponTag(draw, ImVec2(center.x, bottomY), wName, -1, -1);
            } else {
                VisualsRenderer::DrawWeaponTag(draw, ImVec2(center.x, bottomY), wName, 25, 75);
            }
            bottomY += 15.0f;
        }

        if (s.enableDistance) {
            VisualsRenderer::DrawDistanceTag(draw, ImVec2(center.x, bottomY), s.distance);
        }

        // 13. Stacked Badge Flags on Right
        std::vector<std::pair<std::string, Color>> flags;
        if (s.stance == 2) flags.push_back({ "SCOPED", Color(0.25f, 0.72f, 1.0f, 1.0f) });
        if (s.stance == 4) flags.push_back({ "DEFUSING", Color(1.0f, 0.25f, 0.25f, 1.0f) });
        if (s.armor > 0.0f) flags.push_back({ "ARMOR", Color(0.35f, 0.85f, 0.40f, 1.0f) });
        flags.push_back({ "KIT", Color(0.85f, 0.40f, 0.95f, 1.0f) });
        VisualsRenderer::DrawFlagTags(draw, boxMax, flags);

        // 14. 3D Orbital HUD Guidance Overlay
        char orbitText[64];
        snprintf(orbitText, sizeof(orbitText), "360 Orbit: Yaw %.0f deg | Pitch %.0f deg", s.orbitYaw, s.orbitPitch);
        ImVec2 ts = ImGui::CalcTextSize(orbitText);
        draw->AddText(ImVec2(pos.x + 10.0f, pos.y + sz.y - ts.y - 8.0f),
                      isHovered ? pal.Accent.ToU32() : IM_COL32(255, 255, 255, 120), orbitText);

        ImGui::EndChild();
    }

} // namespace Solar::Game
