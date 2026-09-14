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
        u32 vpBorder = isActive ? pal.Accent.ToU32() : (isHovered ? IM_COL32(255, 255, 255, 32) : IM_COL32(255, 255, 255, 14));
        Render::ImGuiExt::AddSmoothBorder(draw, pos, ImVec2(pos.x + sz.x, pos.y + sz.y), vpBorder, 8.0f, 1.0f);

        ImVec2 center(pos.x + sz.x * 0.5f, pos.y + sz.y * 0.48f);

        // 3. Stance Height & Dimensions Configuration
        float boxW = 100.0f;
        float boxH = 205.0f;
        float crouchShift = 0.0f;

        if (s.stance == 0) { // Canonical T-Pose (Fortnite / Valorant Tactical Agent)
            boxW = 145.0f;
            boxH = 205.0f;
            crouchShift = 0.0f;
        } else if (s.stance == 1) { // Combat Stand
            boxW = 100.0f;
            boxH = 205.0f;
            crouchShift = 0.0f;
        } else if (s.stance == 2) { // Crouching
            boxW = 105.0f;
            boxH = 155.0f;
            crouchShift = 25.0f;
        } else if (s.stance == 3) { // Scoped ADS
            boxW = 100.0f;
            boxH = 205.0f;
            crouchShift = 0.0f;
        } else if (s.stance == 4) { // Jumping
            boxW = 100.0f;
            boxH = 205.0f;
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

        // 9. Valorant / Fortnite Stylized Tactical Agent Mesh & Volumetric Skeleton
        if (s.enableSkeleton) {
            float headY = -boxH * 0.5f + 20.0f + crouchShift;
            float neckY = headY + 16.0f;
            float spineY = neckY + 45.0f;
            float pelvisY = spineY + 30.0f;

            ImVec2 head = Project3D(0, headY, 0);
            ImVec2 neck = Project3D(0, neckY, 0);
            ImVec2 spine = Project3D(0, spineY, 0);
            ImVec2 pelvis = Project3D(0, pelvisY, 0);

            float shoulderW = 24.0f;
            ImVec2 lShoulder, rShoulder, lElbow, rElbow, lHand, rHand;
            float legSpread = (s.stance == 2) ? 22.0f : 14.0f;
            float kneeY = pelvisY + ((s.stance == 2) ? 22.0f : 42.0f);
            float footY = floorY - 6.0f;

            if (s.stance == 0) {
                // Canonical T-Pose (Valorant / Fortnite 90-degree horizontal arms, straight posture)
                shoulderW = 22.0f;
                lShoulder = Project3D(-shoulderW, neckY + 4.0f, 0.0f);
                rShoulder = Project3D(shoulderW, neckY + 4.0f, 0.0f);
                lElbow    = Project3D(-48.0f, neckY + 4.0f, 0.0f);
                rElbow    = Project3D(48.0f, neckY + 4.0f, 0.0f);
                lHand     = Project3D(-74.0f, neckY + 4.0f, 0.0f);
                rHand     = Project3D(74.0f, neckY + 4.0f, 0.0f);
                legSpread = 12.0f;
            } else if (s.stance == 3) { // Scoped ADS
                shoulderW = 18.0f;
                lShoulder = Project3D(-shoulderW, neckY + 6.0f, 0.0f);
                rShoulder = Project3D(shoulderW, neckY + 6.0f, 0.0f);
                lElbow    = Project3D(-24.0f, neckY + 34.0f, 18.0f);
                rElbow    = Project3D(24.0f, neckY + 34.0f, 18.0f);
                lHand     = Project3D(-10.0f, neckY + 46.0f, 32.0f);
                rHand     = Project3D(14.0f, neckY + 46.0f, 32.0f);
            } else { // Stand, Crouch, Jump
                lShoulder = Project3D(-shoulderW, neckY + 6.0f, 0.0f);
                rShoulder = Project3D(shoulderW, neckY + 6.0f, 0.0f);
                float armZ = (s.stance == 2) ? 14.0f : 8.0f;
                lElbow    = Project3D(-28.0f, neckY + 38.0f, armZ);
                rElbow    = Project3D(28.0f, neckY + 38.0f, armZ);
                lHand     = Project3D(-14.0f, neckY + 62.0f, armZ + 12.0f);
                rHand     = Project3D(18.0f, neckY + 62.0f, armZ + 12.0f);
            }

            ImVec2 lKnee = Project3D(-legSpread, kneeY, 0.0f);
            ImVec2 rKnee = Project3D(legSpread, kneeY, 0.0f);
            ImVec2 lFoot = Project3D(-legSpread - 2.0f, footY, 0.0f);
            ImVec2 rFoot = Project3D(legSpread + 2.0f, footY, 0.0f);

            // =========================================================================
            // A. TACTICAL AGENT MESH SILHOUETTE (Valorant / Fortnite Style Armor)
            // =========================================================================
            u32 armorFill = IM_COL32(18, 22, 32, 190);
            u32 armorBorder = pal.Accent.WithAlpha(0.40f).ToU32();
            u32 plateFill = IM_COL32(28, 33, 48, 210);

            // 1. Tactical Torso Vest & Chest Armor Plate
            ImVec2 vestTL = Project3D(-17.0f, neckY + 3.0f, 4.0f);
            ImVec2 vestTR = Project3D(17.0f, neckY + 3.0f, 4.0f);
            ImVec2 vestBR = Project3D(13.0f, pelvisY - 4.0f, 3.0f);
            ImVec2 vestBL = Project3D(-13.0f, pelvisY - 4.0f, 3.0f);
            draw->AddQuadFilled(vestTL, vestTR, vestBR, vestBL, armorFill);
            draw->AddQuad(vestTL, vestTR, vestBR, vestBL, armorBorder, 1.0f);

            // Center Chevron Accent on Vest
            ImVec2 chevA = Project3D(-8.0f, neckY + 14.0f, 6.0f);
            ImVec2 chevB = Project3D(0.0f, neckY + 23.0f, 7.0f);
            ImVec2 chevC = Project3D(8.0f, neckY + 14.0f, 6.0f);
            draw->AddLine(chevA, chevB, pal.Accent.WithAlpha(0.85f).ToU32(), 1.5f);
            draw->AddLine(chevB, chevC, pal.Accent.WithAlpha(0.85f).ToU32(), 1.5f);

            // 2. Tactical Shoulder Pauldrons
            ImVec2 pL1 = Project3D(-shoulderW - 4.0f, neckY + 2.0f, 2.0f);
            ImVec2 pL2 = Project3D(-shoulderW + 3.0f, neckY + 2.0f, 2.0f);
            ImVec2 pL3 = Project3D(-shoulderW + 1.0f, neckY + 14.0f, 2.0f);
            ImVec2 pL4 = Project3D(-shoulderW - 6.0f, neckY + 12.0f, 2.0f);
            draw->AddQuadFilled(pL1, pL2, pL3, pL4, plateFill);
            draw->AddQuad(pL1, pL2, pL3, pL4, armorBorder, 1.0f);

            ImVec2 pR1 = Project3D(shoulderW - 3.0f, neckY + 2.0f, 2.0f);
            ImVec2 pR2 = Project3D(shoulderW + 4.0f, neckY + 2.0f, 2.0f);
            ImVec2 pR3 = Project3D(shoulderW + 6.0f, neckY + 12.0f, 2.0f);
            ImVec2 pR4 = Project3D(shoulderW - 1.0f, neckY + 14.0f, 2.0f);
            draw->AddQuadFilled(pR1, pR2, pR3, pR4, plateFill);
            draw->AddQuad(pR1, pR2, pR3, pR4, armorBorder, 1.0f);

            // 3. Tactical Utility Belt with Pouches at Pelvis
            ImVec2 beltL = Project3D(-14.0f, pelvisY - 3.0f, 3.0f);
            ImVec2 beltR = Project3D(14.0f, pelvisY - 3.0f, 3.0f);
            draw->AddLine(beltL, beltR, IM_COL32(35, 42, 58, 255), 4.5f);
            draw->AddLine(beltL, beltR, pal.Accent.WithAlpha(0.50f).ToU32(), 1.2f);
            ImVec2 pouchA = Project3D(-6.0f, pelvisY + 1.0f, 4.0f);
            ImVec2 pouchB = Project3D(6.0f, pelvisY + 1.0f, 4.0f);
            draw->AddRectFilled(ImVec2(pouchA.x - 3.0f, pouchA.y - 3.0f), ImVec2(pouchA.x + 3.0f, pouchA.y + 4.0f), plateFill, 2.0f);
            draw->AddRectFilled(ImVec2(pouchB.x - 3.0f, pouchB.y - 3.0f), ImVec2(pouchB.x + 3.0f, pouchB.y + 4.0f), plateFill, 2.0f);

            // 4. Tactical Combat Boots at Feet
            ImVec2 bootL_T = Project3D(-legSpread - 2.0f, footY - 10.0f, 0.0f);
            ImVec2 bootL_B = Project3D(-legSpread - 2.0f, footY, 3.0f);
            ImVec2 bootR_T = Project3D(legSpread + 2.0f, footY - 10.0f, 0.0f);
            ImVec2 bootR_B = Project3D(legSpread + 2.0f, footY, 3.0f);
            draw->AddLine(bootL_T, bootL_B, IM_COL32(24, 28, 40, 255), 5.5f);
            draw->AddLine(bootL_T, bootL_B, pal.Accent.WithAlpha(0.60f).ToU32(), 1.2f);
            draw->AddLine(bootR_T, bootR_B, IM_COL32(24, 28, 40, 255), 5.5f);
            draw->AddLine(bootR_T, bootR_B, pal.Accent.WithAlpha(0.60f).ToU32(), 1.2f);

            // =========================================================================
            // B. VOLUMETRIC SKELETON RIG & GLOWING BONES
            // =========================================================================
            std::vector<std::pair<ImVec2, ImVec2>> bones = {
                { neck, spine }, { spine, pelvis },
                { neck, lShoulder }, { neck, rShoulder },
                { lShoulder, lElbow }, { rShoulder, rElbow },
                { lElbow, lHand }, { rElbow, rHand },
                { pelvis, lKnee }, { pelvis, rKnee },
                { lKnee, lFoot }, { rKnee, rFoot }
            };

            for (const auto& bone : bones) {
                draw->AddLine(bone.first, bone.second, IM_COL32(0, 0, 0, 220), 4.2f);
                draw->AddLine(bone.first, bone.second, skelColor.ToU32(), 2.4f);
                draw->AddCircleFilled(bone.first, 3.0f, IM_COL32(255, 255, 255, 240), 12);
                draw->AddCircle(bone.first, 3.0f, skelColor.ToU32(), 12, 1.0f);
            }
            draw->AddCircleFilled(lFoot, 3.0f, IM_COL32(255, 255, 255, 240), 12);
            draw->AddCircleFilled(rFoot, 3.0f, IM_COL32(255, 255, 255, 240), 12);

            // 5. Stylized Head with Valorant Cyber Visor
            VisualsRenderer::DrawHeadCircle(draw, head, 11.5f, skelColor, Color(0.08f, 0.09f, 0.13f, 0.95f), true);
            ImVec2 visorL = Project3D(-5.5f, headY + 1.0f, 9.0f);
            ImVec2 visorR = Project3D(5.5f, headY + 1.0f, 9.0f);
            draw->AddLine(visorL, visorR, pal.Accent.ToU32(), 2.2f);
            draw->AddCircleFilled(visorL, 1.2f, IM_COL32(255, 255, 255, 255), 8);
            draw->AddCircleFilled(visorR, 1.2f, IM_COL32(255, 255, 255, 255), 8);

            // 6. Weapon / Barrel Logic
            if (s.stance == 0) {
                // In T-Pose: Weapon slung diagonally across tactical back holster
                ImVec2 slingTop = Project3D(-12.0f, neckY + 2.0f, -8.0f);
                ImVec2 slingBot = Project3D(16.0f, pelvisY + 18.0f, -6.0f);
                draw->AddLine(slingTop, slingBot, IM_COL32(140, 145, 160, 240), 3.5f);
                draw->AddCircleFilled(slingTop, 2.0f, pal.Accent.ToU32(), 8);
            } else {
                // In Combat Stance: Weapon held forward in hands
                ImVec2 barrelTip = Project3D(12.0f, neckY + 54.0f, 48.0f);
                draw->AddLine(lHand, barrelTip, IM_COL32(190, 195, 210, 240), 2.8f);
                draw->AddLine(rHand, barrelTip, IM_COL32(190, 195, 210, 240), 2.8f);

                if (s.enableBarrelRay) {
                    ImVec2 rayEnd = Project3D(12.0f, neckY + 54.0f, 125.0f);
                    VisualsRenderer::DrawLineOfSightTracer(draw, barrelTip, rayEnd,
                                                          pal.Accent.WithAlpha(0.85f), pal.Accent, 1.5f);
                }
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

        // 12. Non-Overlapping HUD Flags & Tags (Clean vertical spacing)
        if (s.enableName) {
            VisualsRenderer::DrawNameTag(draw, ImVec2(center.x, boxMin.y - 12.0f), s.playerName,
                                         s.isFriendly ? Color(0.24f, 0.72f, 1.0f, 1.0f) : Color(1, 1, 1, 1));
        }

        // Generous breathing room below the bounding box, never overlapping bottom borders
        float bottomBarPad = (s.enableArmorBar ? 8.0f : 0.0f);
        float bottomY = boxMax.y + 10.0f + bottomBarPad;

        if (s.enableWeapon) {
            std::string wName = s.weaponName;
            if (wName.find('[') != std::string::npos) {
                VisualsRenderer::DrawWeaponTag(draw, ImVec2(center.x, bottomY), wName, -1, -1);
            } else {
                VisualsRenderer::DrawWeaponTag(draw, ImVec2(center.x, bottomY), wName, 25, 75);
            }
            bottomY += 16.0f;
        }

        if (s.enableDistance) {
            VisualsRenderer::DrawDistanceTag(draw, ImVec2(center.x, bottomY), s.distance);
        }

        // 13. Stacked Badge Flags on Right (Starting from top-right down, never colliding with bottom text)
        std::vector<std::pair<std::string, Color>> flags;
        if (s.stance == 0) flags.push_back({ "T-POSE", pal.Accent });
        else if (s.stance == 3) flags.push_back({ "SCOPED", Color(0.25f, 0.72f, 1.0f, 1.0f) });
        else if (s.stance == 4) flags.push_back({ "JUMPING", Color(1.0f, 0.75f, 0.20f, 1.0f) });
        if (s.armor > 0.0f) flags.push_back({ "ARMOR", Color(0.35f, 0.85f, 0.40f, 1.0f) });
        flags.push_back({ "KIT", Color(0.85f, 0.40f, 0.95f, 1.0f) });
        VisualsRenderer::DrawFlagTags(draw, boxMin, boxMax, flags);

        // 14. 3D Orbital HUD Guidance Overlay
        char orbitText[64];
        snprintf(orbitText, sizeof(orbitText), "360 Orbit: Yaw %.0f deg | Pitch %.0f deg", s.orbitYaw, s.orbitPitch);
        ImVec2 ts = ImGui::CalcTextSize(orbitText);
        draw->AddText(ImVec2(pos.x + 10.0f, pos.y + sz.y - ts.y - 8.0f),
                      isHovered ? pal.Accent.ToU32() : IM_COL32(255, 255, 255, 120), orbitText);

        ImGui::EndChild();
    }

} // namespace Solar::Game
