#include "solar/game/visuals_renderer.hpp"
#include "solar/render/drawlist_utils.hpp"
#include <imgui.h>
#include <cmath>
#include <algorithm>
#include <cstdio>

namespace Solar::Game {

    // ==============================================================================
    // 1. Aim FOV Circles & Dynamic Target Cones
    // ==============================================================================
    void VisualsRenderer::DrawFOVCircle(ImDrawList* draw, const ImVec2& screenCenter, float radius,
                                        Color color, float thickness, bool enableGlow) {
        if (!draw || radius <= 0.5f) return;

        // Subtle outer glow rings for luxury look
        if (enableGlow && color.a > 0.05f) {
            for (int i = 1; i <= 3; ++i) {
                float glowRadius = radius + static_cast<float>(i) * 1.5f;
                float glowAlpha = (color.a * 0.15f) / static_cast<float>(i);
                draw->AddCircle(screenCenter, glowRadius, color.WithAlpha(glowAlpha).ToU32(), 64, thickness + 1.0f);
            }
        }

        // Contrast drop shadow
        draw->AddCircle(screenCenter, radius, IM_COL32(0, 0, 0, static_cast<int>(color.a * 160.0f)), 64, thickness + 1.2f);

        // Core FOV Ring
        draw->AddCircle(screenCenter, radius, color.ToU32(), 64, thickness);
    }

    // ==============================================================================
    // 2. 2D & 3D Bounding Boxes
    // ==============================================================================
    void VisualsRenderer::DrawBoundingBox2D(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                            BoxStyle style, Color color,
                                            Color outlineColor, float cornerLength, float thickness) {
        if (!draw) return;

        float width = max.x - min.x;
        float height = max.y - min.y;
        if (width <= 1.0f || height <= 1.0f) return;

        // Background fill for Filled and CornerFilled modes
        if (style == BoxStyle::Filled || style == BoxStyle::CornerFilled) {
            draw->AddRectFilled(min, max, color.WithAlpha(0.12f).ToU32());
        }

        if (style == BoxStyle::Full || style == BoxStyle::Filled) {
            // Triple-pass crisp outline (Black outer - Color - Black inner)
            u32 outCol = outlineColor.ToU32();
            draw->AddRect(ImVec2(min.x - 1.0f, min.y - 1.0f), ImVec2(max.x + 1.0f, max.y + 1.0f), outCol, 0.0f, 0, thickness);
            draw->AddRect(ImVec2(min.x + 1.0f, min.y + 1.0f), ImVec2(max.x - 1.0f, max.y - 1.0f), outCol, 0.0f, 0, thickness);
            draw->AddRect(min, max, color.ToU32(), 0.0f, 0, thickness);
        }
        else if (style == BoxStyle::Corner || style == BoxStyle::CornerFilled) {
            float clw = (std::min)(cornerLength, width * 0.45f);
            float clh = (std::min)(cornerLength, height * 0.45f);
            u32 mainCol = color.ToU32();
            u32 outCol = outlineColor.ToU32();

            auto drawCorner = [&](const ImVec2& corner, const ImVec2& dirH, const ImVec2& dirV) {
                ImVec2 hEnd(corner.x + dirH.x * clw, corner.y);
                ImVec2 vEnd(corner.x, corner.y + dirV.y * clh);

                // Black outline under horizontal & vertical segments
                draw->AddLine(ImVec2(corner.x - dirH.x, corner.y - dirV.y), ImVec2(hEnd.x + dirH.x, hEnd.y), outCol, thickness + 1.6f);
                draw->AddLine(ImVec2(corner.x - dirH.x, corner.y - dirV.y), ImVec2(vEnd.x, vEnd.y + dirV.y), outCol, thickness + 1.6f);

                // Main color segment
                draw->AddLine(corner, hEnd, mainCol, thickness);
                draw->AddLine(corner, vEnd, mainCol, thickness);
            };

            // Top-Left
            drawCorner(min, ImVec2(1.0f, 0.0f), ImVec2(0.0f, 1.0f));
            // Top-Right
            drawCorner(ImVec2(max.x, min.y), ImVec2(-1.0f, 0.0f), ImVec2(0.0f, 1.0f));
            // Bottom-Left
            drawCorner(ImVec2(min.x, max.y), ImVec2(1.0f, 0.0f), ImVec2(0.0f, -1.0f));
            // Bottom-Right
            drawCorner(max, ImVec2(-1.0f, 0.0f), ImVec2(0.0f, -1.0f));
        }
    }

    void VisualsRenderer::DrawFilledBox(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                        Color fillColor, Color borderColor, float rounding) {
        if (!draw) return;
        if (fillColor.a > 0.0f) {
            draw->AddRectFilled(min, max, fillColor.ToU32(), rounding);
        }
        if (borderColor.a > 0.0f) {
            draw->AddRect(min, max, borderColor.ToU32(), rounding);
        }
    }

    // ==============================================================================
    // 3. Status Bars (Health, Armor, Ammo)
    // ==============================================================================
    void VisualsRenderer::DrawHealthBar(ImDrawList* draw, const ImVec2& boxMin, const ImVec2& boxMax,
                                        float currentHp, float maxHp, BarPosition pos,
                                        bool segmented, bool showValue) {
        if (!draw || maxHp <= 0.0f) return;

        float pct = std::clamp(currentHp / maxHp, 0.0f, 1.0f);
        float barThickness = 3.5f;
        float offset = 5.0f;

        // Dynamic gradient color based on health:
        // 100% -> Emerald Green, 50% -> Amber Gold, 15% -> Crimson Red
        Color hpColor;
        if (pct > 0.5f) {
            float t = (pct - 0.5f) * 2.0f;
            hpColor = Color(
                (1.0f - t) * 1.0f + t * 0.20f,
                (1.0f - t) * 0.80f + t * 0.88f,
                (1.0f - t) * 0.15f + t * 0.35f,
                1.0f
            );
        } else {
            float t = pct * 2.0f;
            hpColor = Color(
                0.95f,
                t * 0.80f,
                0.15f,
                1.0f
            );
        }

        if (pos == BarPosition::Left || pos == BarPosition::Right) {
            float boxH = boxMax.y - boxMin.y;
            float barX = (pos == BarPosition::Left) ? (boxMin.x - offset - barThickness) : (boxMax.x + offset);
            ImVec2 trackMin(barX, boxMin.y);
            ImVec2 trackMax(barX + barThickness, boxMax.y);

            // Black outline & Obsidian track
            draw->AddRectFilled(ImVec2(trackMin.x - 1, trackMin.y - 1), ImVec2(trackMax.x + 1, trackMax.y + 1), IM_COL32(0, 0, 0, 220));
            draw->AddRectFilled(trackMin, trackMax, IM_COL32(18, 18, 22, 230));

            // Fill from bottom up
            float fillH = boxH * pct;
            ImVec2 fillMin(trackMin.x, trackMax.y - fillH);
            ImVec2 fillMax(trackMax.x, trackMax.y);
            draw->AddRectFilled(fillMin, fillMax, hpColor.ToU32());

            // Optional 10-HP divider segments
            if (segmented && boxH > 40.0f) {
                for (int i = 1; i < 10; ++i) {
                    float segY = trackMax.y - (boxH / 10.0f) * static_cast<float>(i);
                    draw->AddLine(ImVec2(trackMin.x, segY), ImVec2(trackMax.x, segY), IM_COL32(0, 0, 0, 200), 1.0f);
                }
            }

            // Health value text when injured
            if (showValue && currentHp < maxHp && currentHp > 0.0f) {
                char buf[16];
                snprintf(buf, sizeof(buf), "%d", static_cast<int>(currentHp));
                ImVec2 ts = ImGui::CalcTextSize(buf);
                ImVec2 textPos(trackMin.x - ts.x - 2.0f, fillMin.y - ts.y * 0.5f);
                draw->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 230), buf);
                draw->AddText(textPos, hpColor.ToU32(), buf);
            }
        }
        else {
            // Horizontal bar (Bottom or Top)
            float boxW = boxMax.x - boxMin.x;
            float barY = (pos == BarPosition::Top) ? (boxMin.y - offset - barThickness) : (boxMax.y + offset);
            ImVec2 trackMin(boxMin.x, barY);
            ImVec2 trackMax(boxMax.x, barY + barThickness);

            draw->AddRectFilled(ImVec2(trackMin.x - 1, trackMin.y - 1), ImVec2(trackMax.x + 1, trackMax.y + 1), IM_COL32(0, 0, 0, 220));
            draw->AddRectFilled(trackMin, trackMax, IM_COL32(18, 18, 22, 230));

            float fillW = boxW * pct;
            ImVec2 fillMin(trackMin.x, trackMin.y);
            ImVec2 fillMax(trackMin.x + fillW, trackMax.y);
            draw->AddRectFilled(fillMin, fillMax, hpColor.ToU32());
        }
    }

    void VisualsRenderer::DrawArmorBar(ImDrawList* draw, const ImVec2& boxMin, const ImVec2& boxMax,
                                       float currentArmor, float maxArmor, BarPosition pos) {
        if (!draw || maxArmor <= 0.0f || currentArmor <= 0.0f) return;

        float pct = std::clamp(currentArmor / maxArmor, 0.0f, 1.0f);
        float barThickness = 3.0f;
        float offset = 10.5f; // Placed just outside the health bar

        Color armorColor(0.24f, 0.65f, 1.0f, 1.0f); // Electric Solar Cyan/Blue

        if (pos == BarPosition::Left || pos == BarPosition::Right) {
            float boxH = boxMax.y - boxMin.y;
            float barX = (pos == BarPosition::Left) ? (boxMin.x - offset - barThickness) : (boxMax.x + offset);
            ImVec2 trackMin(barX, boxMin.y);
            ImVec2 trackMax(barX + barThickness, boxMax.y);

            draw->AddRectFilled(ImVec2(trackMin.x - 1, trackMin.y - 1), ImVec2(trackMax.x + 1, trackMax.y + 1), IM_COL32(0, 0, 0, 220));
            draw->AddRectFilled(trackMin, trackMax, IM_COL32(18, 18, 22, 230));

            float fillH = boxH * pct;
            draw->AddRectFilled(ImVec2(trackMin.x, trackMax.y - fillH), trackMax, armorColor.ToU32());
        }
    }

    void VisualsRenderer::DrawAmmoBar(ImDrawList* draw, const ImVec2& boxMin, const ImVec2& boxMax,
                                      int currentAmmo, int maxAmmo, BarPosition pos) {
        if (!draw || maxAmmo <= 0 || currentAmmo < 0) return;

        float pct = std::clamp(static_cast<float>(currentAmmo) / static_cast<float>(maxAmmo), 0.0f, 1.0f);
        float barThickness = 2.5f;
        float offset = 4.0f;

        Color ammoColor(1.0f, 0.72f, 0.15f, 1.0f); // Golden Solar Amber

        float boxW = boxMax.x - boxMin.x;
        float barY = (pos == BarPosition::Bottom) ? (boxMax.y + offset) : (boxMin.y - offset - barThickness);
        ImVec2 trackMin(boxMin.x, barY);
        ImVec2 trackMax(boxMax.x, barY + barThickness);

        draw->AddRectFilled(ImVec2(trackMin.x - 1, trackMin.y - 1), ImVec2(trackMax.x + 1, trackMax.y + 1), IM_COL32(0, 0, 0, 220));
        draw->AddRectFilled(trackMin, trackMax, IM_COL32(18, 18, 22, 230));

        float fillW = boxW * pct;
        draw->AddRectFilled(trackMin, ImVec2(trackMin.x + fillW, trackMax.y), ammoColor.ToU32());
    }

    // ==============================================================================
    // 4. Skeleton Hierarchy & Bone Linkage
    // ==============================================================================
    void VisualsRenderer::DrawSkeleton(ImDrawList* draw, const std::vector<std::pair<ImVec2, ImVec2>>& bones,
                                       Color color, float thickness,
                                       bool drawJoints, Color jointColor) {
        if (!draw || bones.empty()) return;

        u32 outCol = IM_COL32(0, 0, 0, static_cast<int>(color.a * 180.0f));
        u32 boneCol = color.ToU32();
        u32 jCol = jointColor.ToU32();

        // 1. Draw outline pass
        for (const auto& bone : bones) {
            draw->AddLine(bone.first, bone.second, outCol, thickness + 1.2f);
        }

        // 2. Draw bone line pass
        for (const auto& bone : bones) {
            draw->AddLine(bone.first, bone.second, boneCol, thickness);
        }

        // 3. Draw joint nodes
        if (drawJoints) {
            for (const auto& bone : bones) {
                draw->AddCircleFilled(bone.first, thickness + 0.8f, outCol, 12);
                draw->AddCircleFilled(bone.first, thickness, jCol, 12);
                draw->AddCircleFilled(bone.second, thickness + 0.8f, outCol, 12);
                draw->AddCircleFilled(bone.second, thickness, jCol, 12);
            }
        }
    }

    void VisualsRenderer::DrawHeadCircle(ImDrawList* draw, const ImVec2& headCenter, float radius,
                                         Color color, Color outlineColor, bool filled) {
        if (!draw || radius <= 0.5f) return;

        if (filled) {
            draw->AddCircleFilled(headCenter, radius, color.WithAlpha(0.20f).ToU32(), 24);
        }

        draw->AddCircle(headCenter, radius, outlineColor.ToU32(), 24, 2.2f);
        draw->AddCircle(headCenter, radius, color.ToU32(), 24, 1.4f);
    }

    // ==============================================================================
    // 5. Targeting Rays & Snaplines
    // ==============================================================================
    void VisualsRenderer::DrawSnapline(ImDrawList* draw, const ImVec2& targetPos,
                                       SnaplineOrigin originType, Color color,
                                       float thickness, bool dashed) {
        if (!draw) return;

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 origin;
        if (originType == SnaplineOrigin::ScreenBottom) {
            origin = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y);
        } else if (originType == SnaplineOrigin::ScreenCenter) {
            origin = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        } else {
            origin = ImVec2(io.DisplaySize.x * 0.5f, 0.0f);
        }

        u32 outCol = IM_COL32(0, 0, 0, static_cast<int>(color.a * 160.0f));
        u32 lineCol = color.ToU32();

        if (dashed) {
            float dx = targetPos.x - origin.x;
            float dy = targetPos.y - origin.y;
            float dist = std::hypot(dx, dy);
            if (dist <= 1.0f) return;

            float step = 10.0f;
            float dashLen = 6.0f;
            float nx = dx / dist;
            float ny = dy / dist;

            for (float d = 0.0f; d < dist; d += step) {
                float endD = (std::min)(d + dashLen, dist);
                ImVec2 p1(origin.x + nx * d, origin.y + ny * d);
                ImVec2 p2(origin.x + nx * endD, origin.y + ny * endD);
                draw->AddLine(p1, p2, outCol, thickness + 1.2f);
                draw->AddLine(p1, p2, lineCol, thickness);
            }
        } else {
            draw->AddLine(origin, targetPos, outCol, thickness + 1.2f);
            draw->AddLine(origin, targetPos, lineCol, thickness);
        }
    }

    // ==============================================================================
    // 6. Text Elements & Tag Clouds
    // ==============================================================================
    void VisualsRenderer::DrawNameTag(ImDrawList* draw, const ImVec2& pos, const std::string& name,
                                      Color color, bool centered, bool dropShadow) {
        if (!draw || name.empty()) return;

        ImVec2 p = pos;
        if (centered) {
            ImVec2 ts = ImGui::CalcTextSize(name.c_str());
            p.x -= ts.x * 0.5f;
            p.y -= ts.y;
        }

        if (dropShadow) {
            draw->AddText(ImVec2(p.x + 1.0f, p.y + 1.0f), IM_COL32(0, 0, 0, 230), name.c_str());
        }
        draw->AddText(p, color.ToU32(), name.c_str());
    }

    void VisualsRenderer::DrawWeaponTag(ImDrawList* draw, const ImVec2& pos, const std::string& weaponName,
                                        int currentAmmo, int maxAmmo, Color color, bool centered) {
        if (!draw || weaponName.empty()) return;

        char buf[64];
        if (currentAmmo >= 0 && maxAmmo > 0) {
            snprintf(buf, sizeof(buf), "%s [%d/%d]", weaponName.c_str(), currentAmmo, maxAmmo);
        } else {
            snprintf(buf, sizeof(buf), "%s", weaponName.c_str());
        }

        ImVec2 p = pos;
        if (centered) {
            ImVec2 ts = ImGui::CalcTextSize(buf);
            p.x -= ts.x * 0.5f;
        }

        draw->AddText(ImVec2(p.x + 1.0f, p.y + 1.0f), IM_COL32(0, 0, 0, 230), buf);
        draw->AddText(p, color.ToU32(), buf);
    }

    void VisualsRenderer::DrawDistanceTag(ImDrawList* draw, const ImVec2& pos, float distanceMeters, Color color) {
        if (!draw) return;

        char buf[32];
        snprintf(buf, sizeof(buf), "%.0fm", distanceMeters);

        ImVec2 ts = ImGui::CalcTextSize(buf);
        ImVec2 p(pos.x - ts.x * 0.5f, pos.y);

        draw->AddText(ImVec2(p.x + 1.0f, p.y + 1.0f), IM_COL32(0, 0, 0, 230), buf);
        draw->AddText(p, color.ToU32(), buf);
    }

    void VisualsRenderer::DrawFlagTags(ImDrawList* draw, const ImVec2& boxMax,
                                       const std::vector<std::pair<std::string, Color>>& flags) {
        if (!draw || flags.empty()) return;

        float currY = boxMax.y - 12.0f;
        float startX = boxMax.x + 6.0f;

        for (const auto& flag : flags) {
            ImVec2 ts = ImGui::CalcTextSize(flag.first.c_str());
            ImVec2 badgeMin(startX - 2.0f, currY - 1.0f);
            ImVec2 badgeMax(startX + ts.x + 4.0f, currY + ts.y + 1.0f);

            // Subtle dark obsidian badge background with 1px border
            draw->AddRectFilled(badgeMin, badgeMax, IM_COL32(14, 14, 18, 210), 3.0f);
            draw->AddRect(badgeMin, badgeMax, IM_COL32(0, 0, 0, 180), 3.0f);

            // Text
            draw->AddText(ImVec2(startX + 1.0f, currY), flag.second.ToU32(), flag.first.c_str());

            currY += ts.y + 3.0f;
        }
    }

    // ==============================================================================
    // 7. Tactical Combat HUD (Offscreen Indicator & Hitmarkers)
    // ==============================================================================
    void VisualsRenderer::DrawOffscreenIndicator(ImDrawList* draw, const ImVec2& screenCenter,
                                                 float angleRadians, float screenRadius,
                                                 Color color, float distanceMeters) {
        if (!draw || screenRadius <= 10.0f) return;

        float cosA = std::cos(angleRadians);
        float sinA = std::sin(angleRadians);

        ImVec2 arrowTip(screenCenter.x + cosA * screenRadius, screenCenter.y + sinA * screenRadius);

        float arrowLen = 14.0f;
        float arrowHalfW = 6.0f;

        // Base center point behind the tip
        ImVec2 baseCenter(arrowTip.x - cosA * arrowLen, arrowTip.y - sinA * arrowLen);
        // Perpendicular vector for triangle wings
        ImVec2 perp(-sinA, cosA);

        ImVec2 pLeft(baseCenter.x + perp.x * arrowHalfW, baseCenter.y + perp.y * arrowHalfW);
        ImVec2 pRight(baseCenter.x - perp.x * arrowHalfW, baseCenter.y - perp.y * arrowHalfW);

        // Drop shadow pass
        draw->AddTriangleFilled(
            ImVec2(arrowTip.x + 1.0f, arrowTip.y + 1.0f),
            ImVec2(pLeft.x + 1.0f, pLeft.y + 1.0f),
            ImVec2(pRight.x + 1.0f, pRight.y + 1.0f),
            IM_COL32(0, 0, 0, 180)
        );

        // Core Triangle
        draw->AddTriangleFilled(arrowTip, pLeft, pRight, color.ToU32());
        draw->AddTriangle(arrowTip, pLeft, pRight, IM_COL32(0, 0, 0, 220), 1.2f);

        // Distance Tag if valid
        if (distanceMeters > 0.0f) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%.0fm", distanceMeters);
            ImVec2 ts = ImGui::CalcTextSize(buf);
            ImVec2 textPos(baseCenter.x - cosA * (ts.y + 4.0f) - ts.x * 0.5f,
                           baseCenter.y - sinA * (ts.y + 4.0f) - ts.y * 0.5f);
            draw->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 220), buf);
            draw->AddText(textPos, color.ToU32(), buf);
        }
    }

    void VisualsRenderer::DrawHitmarker(ImDrawList* draw, const ImVec2& screenCenter, float size,
                                        Color color, float progress, float damage) {
        if (!draw || progress <= 0.001f) return;

        float alpha = std::clamp(progress, 0.0f, 1.0f);
        u32 hitCol = color.WithAlpha(color.a * alpha).ToU32();
        u32 outCol = IM_COL32(0, 0, 0, static_cast<int>(color.a * alpha * 180.0f));

        float innerGap = 4.0f;
        float outerLen = innerGap + size * (0.85f + 0.15f * (1.0f - progress));

        const float diag[4][2] = {
            { -1.0f, -1.0f },
            {  1.0f, -1.0f },
            { -1.0f,  1.0f },
            {  1.0f,  1.0f }
        };

        for (int i = 0; i < 4; ++i) {
            ImVec2 p1(screenCenter.x + diag[i][0] * innerGap, screenCenter.y + diag[i][1] * innerGap);
            ImVec2 p2(screenCenter.x + diag[i][0] * outerLen, screenCenter.y + diag[i][1] * outerLen);

            draw->AddLine(p1, p2, outCol, 2.6f);
            draw->AddLine(p1, p2, hitCol, 1.6f);
        }

        // Floating Damage Number fading upwards
        if (damage >= 0.0f) {
            char buf[16];
            snprintf(buf, sizeof(buf), "-%.0f", damage);
            ImVec2 ts = ImGui::CalcTextSize(buf);

            float floatOffsetY = (1.0f - progress) * 28.0f + 14.0f;
            ImVec2 dmgPos(screenCenter.x - ts.x * 0.5f, screenCenter.y - floatOffsetY);

            u32 dmgCol = IM_COL32(255, 60, 60, static_cast<int>(255 * alpha));
            draw->AddText(ImVec2(dmgPos.x + 1, dmgPos.y + 1), IM_COL32(0, 0, 0, static_cast<int>(220 * alpha)), buf);
            draw->AddText(dmgPos, dmgCol, buf);
        }
    }

    // ==============================================================================
    // 8. Visuals 2.0 Modern Enhancements
    // ==============================================================================
    void VisualsRenderer::DrawGradientBox2D(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                          Color colTop, Color colBottom, float thickness) {
        if (!draw) return;

        u32 cTop = colTop.ToU32();
        u32 cBot = colBottom.ToU32();
        u32 outCol = IM_COL32(0, 0, 0, 200);

        // Black outer contour
        draw->AddRect(ImVec2(min.x - 1, min.y - 1), ImVec2(max.x + 1, max.y + 1), outCol, 0.0f, 0, thickness + 1.2f);

        // Top line
        draw->AddLine(min, ImVec2(max.x, min.y), cTop, thickness);
        // Bottom line
        draw->AddLine(ImVec2(min.x, max.y), max, cBot, thickness);
        // Left line (vertical gradient)
        draw->AddRectFilledMultiColor(ImVec2(min.x, min.y), ImVec2(min.x + thickness, max.y), cTop, cTop, cBot, cBot);
        // Right line (vertical gradient)
        draw->AddRectFilledMultiColor(ImVec2(max.x - thickness, min.y), ImVec2(max.x, max.y), cTop, cTop, cBot, cBot);
    }

    void VisualsRenderer::DrawGlowOutlineBox2D(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                              Color boxColor, Color glowColor,
                                              float glowRadius, float thickness) {
        if (!draw) return;

        // Multi-pass translucent halo
        int layers = 4;
        for (int i = layers; i >= 1; --i) {
            float expand = (glowRadius / static_cast<float>(layers)) * static_cast<float>(i);
            float layerAlpha = (glowColor.a / static_cast<float>(layers * 1.5f)) * (1.0f - static_cast<float>(i - 1) / static_cast<float>(layers));
            u32 haloCol = glowColor.WithAlpha(layerAlpha).ToU32();
            draw->AddRect(ImVec2(min.x - expand, min.y - expand),
                          ImVec2(max.x + expand, max.y + expand),
                          haloCol, 3.0f, 0, 1.2f);
        }

        // Inner solid box
        DrawBoundingBox2D(draw, min, max, BoxStyle::Corner, boxColor, Color(0, 0, 0, 0.85f), 12.0f, thickness);
    }

    void VisualsRenderer::DrawCapsuleHitbox3D(ImDrawList* draw, const ImVec2& screenStart, const ImVec2& screenEnd,
                                             float screenRadius, Color coreColor, Color glowColor) {
        if (!draw) return;

        // Outer glow
        draw->AddCircleFilled(screenStart, screenRadius + 3.0f, glowColor.WithAlpha(glowColor.a * 0.35f).ToU32(), 12);
        draw->AddCircleFilled(screenEnd, screenRadius + 3.0f, glowColor.WithAlpha(glowColor.a * 0.35f).ToU32(), 12);

        // Core line
        draw->AddLine(screenStart, screenEnd, glowColor.WithAlpha(glowColor.a * 0.45f).ToU32(), (screenRadius + 2.0f) * 2.0f);
        draw->AddLine(screenStart, screenEnd, coreColor.ToU32(), screenRadius * 2.0f);

        // Cap spheres
        draw->AddCircleFilled(screenStart, screenRadius, coreColor.ToU32(), 16);
        draw->AddCircleFilled(screenEnd, screenRadius, coreColor.ToU32(), 16);
    }

    void VisualsRenderer::DrawAcousticWave(ImDrawList* draw, const ImVec2& screenCenter,
                                          float radiusX, float radiusY, float angleDeg,
                                          Color waveColor, float thickness) {
        if (!draw || radiusX <= 1.0f || radiusY <= 1.0f) return;

        constexpr int segments = 32;
        ImVec2 points[segments];
        float rad = angleDeg * 0.01745329251f;
        float cosA = std::cos(rad), sinA = std::sin(rad);

        for (int i = 0; i < segments; ++i) {
            float theta = (static_cast<float>(i) / static_cast<float>(segments)) * 6.28318530718f;
            float lx = std::cos(theta) * radiusX;
            float ly = std::sin(theta) * radiusY;

            points[i] = ImVec2(
                screenCenter.x + lx * cosA - ly * sinA,
                screenCenter.y + lx * sinA + ly * cosA
            );
        }

        u32 col = waveColor.ToU32();
        draw->AddPolyline(points, segments, col, ImDrawFlags_Closed, thickness);
    }

    void VisualsRenderer::DrawSpreadCrosshair(ImDrawList* draw, const ImVec2& screenCenter,
                                             float baseGap, float spreadRadius, float length,
                                             Color crossColor, bool dot, bool tStyle) {
        if (!draw) return;

        u32 col = crossColor.ToU32();
        u32 outCol = IM_COL32(0, 0, 0, 220);
        float totalGap = baseGap + spreadRadius;

        // Center dot
        if (dot) {
            draw->AddCircleFilled(screenCenter, 1.8f, outCol, 8);
            draw->AddCircleFilled(screenCenter, 1.2f, col, 8);
        }

        // Left line
        draw->AddLine(ImVec2(screenCenter.x - totalGap - length, screenCenter.y), ImVec2(screenCenter.x - totalGap, screenCenter.y), outCol, 2.8f);
        draw->AddLine(ImVec2(screenCenter.x - totalGap - length, screenCenter.y), ImVec2(screenCenter.x - totalGap, screenCenter.y), col, 1.5f);

        // Right line
        draw->AddLine(ImVec2(screenCenter.x + totalGap, screenCenter.y), ImVec2(screenCenter.x + totalGap + length, screenCenter.y), outCol, 2.8f);
        draw->AddLine(ImVec2(screenCenter.x + totalGap, screenCenter.y), ImVec2(screenCenter.x + totalGap + length, screenCenter.y), col, 1.5f);

        // Bottom line
        draw->AddLine(ImVec2(screenCenter.x, screenCenter.y + totalGap), ImVec2(screenCenter.x, screenCenter.y + totalGap + length), outCol, 2.8f);
        draw->AddLine(ImVec2(screenCenter.x, screenCenter.y + totalGap), ImVec2(screenCenter.x, screenCenter.y + totalGap + length), col, 1.5f);

        // Top line (omitted if T-Style)
        if (!tStyle) {
            draw->AddLine(ImVec2(screenCenter.x, screenCenter.y - totalGap - length), ImVec2(screenCenter.x, screenCenter.y - totalGap), outCol, 2.8f);
            draw->AddLine(ImVec2(screenCenter.x, screenCenter.y - totalGap - length), ImVec2(screenCenter.x, screenCenter.y - totalGap), col, 1.5f);
        }
    }

    void VisualsRenderer::DrawFloatingDamage(ImDrawList* draw, const ImVec2& pos, float damage,
                                            Color color, float alpha, bool isCritical) {
        if (!draw || alpha <= 0.01f) return;

        char buf[32];
        if (isCritical) {
            snprintf(buf, sizeof(buf), "CRIT -%.0f", damage);
        } else {
            snprintf(buf, sizeof(buf), "-%.0f", damage);
        }

        ImVec2 ts = ImGui::CalcTextSize(buf);
        ImVec2 textPos(pos.x - ts.x * 0.5f, pos.y);

        u32 shadowCol = IM_COL32(0, 0, 0, static_cast<int>(220 * alpha));
        u32 textColor = color.WithAlpha(color.a * alpha).ToU32();

        draw->AddText(ImVec2(textPos.x + 1, textPos.y + 1), shadowCol, buf);
        draw->AddText(textPos, textColor, buf);
    }

    void VisualsRenderer::DrawLineOfSightTracer(ImDrawList* draw, const ImVec2& eyePos, const ImVec2& hitPos,
                                               Color beamColor, Color impactColor, float thickness) {
        if (!draw) return;

        u32 bCol = beamColor.ToU32();
        u32 outCol = IM_COL32(0, 0, 0, 180);

        draw->AddLine(eyePos, hitPos, outCol, thickness + 1.2f);
        draw->AddLine(eyePos, hitPos, bCol, thickness);

        // Impact Beacon Sphere
        draw->AddCircleFilled(hitPos, 4.0f, impactColor.WithAlpha(0.35f).ToU32(), 12);
        draw->AddCircleFilled(hitPos, 2.0f, impactColor.ToU32(), 8);
    }

} // namespace Solar::Game
