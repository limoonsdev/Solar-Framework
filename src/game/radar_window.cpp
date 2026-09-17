#include "solar/game/radar_window.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/shadow_caster.hpp"
#include "solar/font_awesome.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <cmath>
#include <cstdio>
#include <algorithm>

namespace Solar::Game {

    void RadarWindow::Render(bool* p_open,
                             const Widgets::RadarSettings& settings,
                             const std::vector<Widgets::RadarEntity>& entities)
    {
        if (!p_open || !(*p_open)) return;

        ImGui::SetNextWindowSize(ImVec2(245.0f, 305.0f), ImGuiCond_FirstUseEver);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;

        const auto& pal = ThemeManager::Get().GetPalette();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, pal.Card.WithAlpha(0.96f).ToVec4());
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));

        if (ImGui::Begin("##TacticalRadarWindow", p_open, flags)) {
            ImVec2 winPos = ImGui::GetWindowPos();
            ImVec2 winSize = ImGui::GetWindowSize();
            ImDrawList* draw = ImGui::GetWindowDrawList();

            // 1. Soft Ambient Multi-Layer Shadow & Specular Glass Sheen
            Render::ShadowCaster::DrawShadow(draw, winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
                                            16.0f, 10.0f, Color(0, 0, 0, 0.55f), ImVec2(0, 4.0f));
            Render::ImGuiExt::DrawSpecularEdge(draw, winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
                                              IM_COL32(255, 255, 255, 30), 10.0f, 1.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
                                             pal.Border.ToU32(), 10.0f, 1.0f);

            // 2. Window Header (Glowing Status Dot + Crosshair + Title + Range Pill + Close Button)
            float headerH = 28.0f;

            // Indicator dot + icon
            draw->AddCircleFilled(ImVec2(winPos.x + 14.0f, winPos.y + 15.0f), 2.5f, pal.Accent.ToU32(), 10);
            draw->AddText(ImVec2(winPos.x + 22.0f, winPos.y + 9.0f), pal.Accent.ToU32(), ICON_FA_CROSSHAIRS);
            draw->AddText(ImVec2(winPos.x + 40.0f, winPos.y + 9.0f), pal.TextPrimary.ToU32(), "Tactical Radar");

            // Range badge pill in header
            char rangePill[16];
            std::snprintf(rangePill, sizeof(rangePill), "%.0fM", settings.rangeMeters);
            ImVec2 pillSz = ImGui::CalcTextSize(rangePill);
            float pillX = winPos.x + winSize.x - 52.0f - pillSz.x;
            ImVec2 pillMin(pillX, winPos.y + 7.0f);
            ImVec2 pillMax(pillX + pillSz.x + 10.0f, winPos.y + 21.0f);
            draw->AddRectFilled(pillMin, pillMax, pal.Header.WithAlpha(0.80f).ToU32(), 3.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, pillMin, pillMax, pal.Accent.WithAlpha(0.35f).ToU32(), 3.0f, 1.0f);
            draw->AddText(ImVec2(pillMin.x + 5.0f, pillMin.y + 1.0f), pal.Accent.ToU32(), rangePill);

            // Close button with dedicated relative positioning
            ImGui::SetCursorPos(ImVec2(winSize.x - 26.0f, 5.0f));
            if (ImGui::InvisibleButton("##radar_close", ImVec2(18.0f, 18.0f))) {
                *p_open = false;
                Audio::PlayClick();
            }
            bool hoverClose = ImGui::IsItemHovered();
            draw->AddText(ImVec2(winPos.x + winSize.x - 22.0f, winPos.y + 7.0f),
                          hoverClose ? pal.Danger.ToU32() : pal.TextDisabled.ToU32(),
                          ICON_FA_XMARK);

            // Divider line with specular accent center
            float divY = winPos.y + headerH + 2.0f;
            draw->AddLine(ImVec2(winPos.x + 10.0f, divY), ImVec2(winPos.x + winSize.x - 10.0f, divY),
                          pal.Border.WithAlpha(0.35f).ToU32(), 1.0f);

            // 3. Viewport Construction (Relative cursor placement)
            float vpSize = winSize.x - 20.0f;
            ImGui::SetCursorPos(ImVec2(10.0f, headerH + 5.0f));
            ImVec2 vpMin = ImGui::GetCursorScreenPos();
            ImVec2 vpMax(vpMin.x + vpSize, vpMin.y + vpSize);
            ImVec2 vpCenter(vpMin.x + vpSize * 0.5f, vpMin.y + vpSize * 0.5f);
            float radius = vpSize * 0.46f;

            // Submit bounding dummy item so ImGui knows the layout bounds
            ImGui::Dummy(ImVec2(vpSize, vpSize));

            // Viewport Obsidian Glass Housing
            draw->AddRectFilled(vpMin, vpMax, pal.Background.WithAlpha(0.96f).ToU32(), 6.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, vpMin, vpMax, pal.Border.WithAlpha(0.60f).ToU32(), 6.0f, 1.0f);

            // Subtle tactical polar background gradient / circle
            draw->AddCircleFilled(vpCenter, radius, pal.Header.WithAlpha(0.40f).ToU32(), 64);
            draw->AddCircle(vpCenter, radius, pal.Border.WithAlpha(0.65f).ToU32(), 64, 1.2f);

            // Outer Azimuth Graduation Dial (Compass Ticks every 30°)
            for (int deg = 0; deg < 360; deg += 30) {
                float a = deg * (3.14159265f / 180.0f);
                float rInner = (deg % 90 == 0) ? (radius - 5.0f) : (radius - 3.0f);
                ImVec2 p0(vpCenter.x + std::sin(a) * rInner, vpCenter.y - std::cos(a) * rInner);
                ImVec2 p1(vpCenter.x + std::sin(a) * radius, vpCenter.y - std::cos(a) * radius);
                u32 tickCol = (deg == 0) ? pal.Accent.ToU32() : pal.Border.WithAlpha(0.45f).ToU32();
                draw->AddLine(p0, p1, tickCol, 1.0f);
            }

            // Concentric Range Rings with micro distance labels
            draw->AddCircle(vpCenter, radius * 0.33f, pal.Border.WithAlpha(0.20f).ToU32(), 48, 1.0f);
            draw->AddCircle(vpCenter, radius * 0.66f, pal.Border.WithAlpha(0.28f).ToU32(), 48, 1.0f);

            // Tactical Center Crosshairs (with center deadzone)
            float deadzone = 8.0f;
            draw->AddLine(ImVec2(vpCenter.x - radius + 5.0f, vpCenter.y), ImVec2(vpCenter.x - deadzone, vpCenter.y), pal.Border.WithAlpha(0.25f).ToU32(), 1.0f);
            draw->AddLine(ImVec2(vpCenter.x + deadzone, vpCenter.y), ImVec2(vpCenter.x + radius - 5.0f, vpCenter.y), pal.Border.WithAlpha(0.25f).ToU32(), 1.0f);
            draw->AddLine(ImVec2(vpCenter.x, vpCenter.y - radius + 5.0f), ImVec2(vpCenter.x, vpCenter.y - deadzone), pal.Border.WithAlpha(0.25f).ToU32(), 1.0f);
            draw->AddLine(ImVec2(vpCenter.x, vpCenter.y + deadzone), ImVec2(vpCenter.x, vpCenter.y + radius - 5.0f), pal.Border.WithAlpha(0.25f).ToU32(), 1.0f);

            // Cardinal Indicators: North in accent glow
            draw->AddText(ImVec2(vpCenter.x - 3.5f, vpCenter.y - radius + 4.0f), pal.Accent.ToU32(), "N");
            draw->AddText(ImVec2(vpCenter.x - 3.0f, vpCenter.y + radius - 14.0f), pal.TextDisabled.WithAlpha(0.45f).ToU32(), "S");
            draw->AddText(ImVec2(vpCenter.x + radius - 12.0f, vpCenter.y - 6.0f), pal.TextDisabled.WithAlpha(0.45f).ToU32(), "E");
            draw->AddText(ImVec2(vpCenter.x - radius + 5.0f, vpCenter.y - 6.0f), pal.TextDisabled.WithAlpha(0.45f).ToU32(), "W");

            // 4. Phosphorescent Rotating Sweep Beam (24-Segment Exponential Fade)
            if (settings.showSweep) {
                static float s_sweepAngle = 0.0f;
                s_sweepAngle += ImGui::GetIO().DeltaTime * settings.sweepSpeed;
                if (s_sweepAngle >= 360.0f) s_sweepAngle -= 360.0f;

                float sweepRad = s_sweepAngle * (3.14159265f / 180.0f);
                const int trailSegments = 24;
                const float trailArc = 48.0f * (3.14159265f / 180.0f);

                for (int t = 0; t < trailSegments; ++t) {
                    float a0 = sweepRad - trailArc * (static_cast<float>(t + 1) / static_cast<float>(trailSegments));
                    float a1 = sweepRad - trailArc * (static_cast<float>(t) / static_cast<float>(trailSegments));

                    float ratio = 1.0f - static_cast<float>(t) / static_cast<float>(trailSegments);
                    float alpha = std::pow(ratio, 2.2f) * 0.22f;
                    u32 wedgeCol = pal.Accent.WithAlpha(alpha).ToU32();

                    ImVec2 p0(vpCenter.x + std::cos(a0) * radius, vpCenter.y + std::sin(a0) * radius);
                    ImVec2 p1(vpCenter.x + std::cos(a1) * radius, vpCenter.y + std::sin(a1) * radius);

                    draw->AddTriangleFilled(vpCenter, p0, p1, wedgeCol);
                }

                // Lead beam with glowing laser bloom
                ImVec2 beamEnd(vpCenter.x + std::cos(sweepRad) * radius, vpCenter.y + std::sin(sweepRad) * radius);
                draw->AddLine(vpCenter, beamEnd, pal.Accent.WithAlpha(0.35f).ToU32(), 3.0f);
                draw->AddLine(vpCenter, beamEnd, pal.Accent.ToU32(), 1.2f);
            }

            // 5. Local Player Stealth Craft Avatar & Translucent FOV Cone
            float fovLen = 22.0f;
            float fovAngle = 32.0f * (3.14159265f / 180.0f);
            ImVec2 fovLeft(vpCenter.x - std::sin(fovAngle) * fovLen, vpCenter.y - std::cos(fovAngle) * fovLen);
            ImVec2 fovRight(vpCenter.x + std::sin(fovAngle) * fovLen, vpCenter.y - std::cos(fovAngle) * fovLen);

            // Forward Vision Sector Fill
            draw->AddTriangleFilled(vpCenter, fovLeft, fovRight, pal.Accent.WithAlpha(0.10f).ToU32());
            draw->AddLine(vpCenter, fovLeft, pal.Accent.WithAlpha(0.40f).ToU32(), 1.0f);
            draw->AddLine(vpCenter, fovRight, pal.Accent.WithAlpha(0.40f).ToU32(), 1.0f);

            // Center Player Delta Craft Glyph
            ImVec2 craftTip(vpCenter.x, vpCenter.y - 5.5f);
            ImVec2 craftLeft(vpCenter.x - 4.5f, vpCenter.y + 4.0f);
            ImVec2 craftRight(vpCenter.x + 4.5f, vpCenter.y + 4.0f);
            ImVec2 craftNotch(vpCenter.x, vpCenter.y + 2.0f);
            draw->AddTriangleFilled(craftTip, craftLeft, craftNotch, pal.Accent.ToU32());
            draw->AddTriangleFilled(craftTip, craftNotch, craftRight, pal.Accent.ToU32());
            draw->AddCircleFilled(vpCenter, 1.5f, IM_COL32(255, 255, 255, 255), 10);

            // 6. Entity Blips (Tactical Threat Pips)
            float meterToPix = radius / (settings.rangeMeters > 0.001f ? settings.rangeMeters : 1.0f);
            int enemyCount = 0;
            int friendlyCount = 0;

            static float s_pulseTimer = 0.0f;
            s_pulseTimer += ImGui::GetIO().DeltaTime * 3.0f;

            for (const auto& ent : entities) {
                if (ent.isEnemy) enemyCount++; else friendlyCount++;

                float relX = ent.x * meterToPix;
                float relY = -ent.y * meterToPix;
                float dist = std::sqrt(relX * relX + relY * relY);

                ImVec2 blipPos;
                bool clamped = false;

                if (dist > radius - 4.0f) {
                    float norm = (radius - 4.0f) / dist;
                    blipPos = ImVec2(vpCenter.x + relX * norm, vpCenter.y + relY * norm);
                    clamped = true;
                } else {
                    blipPos = ImVec2(vpCenter.x + relX, vpCenter.y + relY);
                }

                Color blipColor = ent.isEnemy ? pal.Danger : Color(0.18f, 0.82f, 0.55f, 1.0f);
                if (ent.isDormant) blipColor = blipColor.WithAlpha(0.40f);

                if (clamped) {
                    // Clamped Perimeter Chevron
                    float angle = std::atan2(relY, relX);
                    ImVec2 arrTip(vpCenter.x + std::cos(angle) * (radius - 1.5f), vpCenter.y + std::sin(angle) * (radius - 1.5f));
                    ImVec2 arrB1(vpCenter.x + std::cos(angle + 0.20f) * (radius - 6.0f), vpCenter.y + std::sin(angle + 0.20f) * (radius - 6.0f));
                    ImVec2 arrB2(vpCenter.x + std::cos(angle - 0.20f) * (radius - 6.0f), vpCenter.y + std::sin(angle - 0.20f) * (radius - 6.0f));
                    draw->AddTriangleFilled(arrTip, arrB1, arrB2, blipColor.ToU32());
                } else {
                    if (ent.isEnemy) {
                        // Threat Diamond Pip
                        float s = 4.0f;
                        ImVec2 dTop(blipPos.x, blipPos.y - s);
                        ImVec2 dRight(blipPos.x + s, blipPos.y);
                        ImVec2 dBot(blipPos.x, blipPos.y + s);
                        ImVec2 dLeft(blipPos.x - s, blipPos.y);

                        // Pulsing threat ring for close enemies
                        if (dist < radius * 0.45f) {
                            float pulseR = s + 2.0f + std::sin(s_pulseTimer) * 2.0f;
                            draw->AddCircle(blipPos, pulseR, blipColor.WithAlpha(0.35f).ToU32(), 16, 1.0f);
                        }

                        draw->AddQuadFilled(dTop, dRight, dBot, dLeft, blipColor.ToU32());
                        draw->AddQuad(dTop, dRight, dBot, dLeft, IM_COL32(255, 255, 255, 180), 1.0f);
                    } else {
                        // Friendly Smooth Circle Pip
                        draw->AddCircleFilled(blipPos, 3.8f, blipColor.ToU32(), 16);
                        draw->AddCircle(blipPos, 3.8f, IM_COL32(255, 255, 255, 180), 16, 1.0f);
                    }

                    // Directional Orientation Vector
                    if (settings.showHeadingCones) {
                        float yawRad = (ent.yaw - 90.0f) * (3.14159265f / 180.0f);
                        ImVec2 dirEnd(blipPos.x + std::cos(yawRad) * 8.0f, blipPos.y + std::sin(yawRad) * 8.0f);
                        draw->AddLine(blipPos, dirEnd, blipColor.ToU32(), 1.5f);
                    }

                    // Relative Altitude Micro-Tag
                    if (std::abs(ent.z) > 1.5f) {
                        const char* altChar = ent.z > 0.0f ? "^" : "v";
                        draw->AddText(ImVec2(blipPos.x + 5.0f, blipPos.y - 7.0f), blipColor.ToU32(), altChar);
                    }
                }
            }

            // 7. Minimalist Segmented HUD Footer Status Strip
            ImGui::SetCursorPos(ImVec2(10.0f, headerH + vpSize + 11.0f));
            ImGui::Dummy(ImVec2(vpSize, 22.0f));

            ImVec2 ftMin(winPos.x + 10.0f, winPos.y + headerH + vpSize + 11.0f);
            ImVec2 ftMax(winPos.x + winSize.x - 10.0f, ftMin.y + 22.0f);

            draw->AddRectFilled(ftMin, ftMax, pal.Header.WithAlpha(0.65f).ToU32(), 4.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, ftMin, ftMax, pal.Border.WithAlpha(0.35f).ToU32(), 4.0f, 1.0f);

            // Left: Enemy count in danger pill
            char enmBuf[24];
            std::snprintf(enmBuf, sizeof(enmBuf), "! %d HOSTILE", enemyCount);
            draw->AddText(ImVec2(ftMin.x + 8.0f, ftMin.y + 3.0f), pal.Danger.ToU32(), enmBuf);

            // Center divider
            draw->AddLine(ImVec2(ftMin.x + 85.0f, ftMin.y + 4.0f), ImVec2(ftMin.x + 85.0f, ftMax.y - 4.0f),
                          pal.Border.WithAlpha(0.30f).ToU32(), 1.0f);

            // Right: Friendly count in success / accent
            char frdBuf[24];
            std::snprintf(frdBuf, sizeof(frdBuf), "%d ALLIED", friendlyCount);
            draw->AddText(ImVec2(ftMin.x + 95.0f, ftMin.y + 3.0f), Color(0.18f, 0.82f, 0.55f, 1.0f).ToU32(), frdBuf);

            // Far right: Live radar status
            draw->AddCircleFilled(ImVec2(ftMax.x - 12.0f, ftMin.y + 11.0f), 2.5f, pal.Accent.ToU32(), 10);
            Render::ImGuiExt::RenderResizeGrip(ImVec2(190.0f, 220.0f), "##RadarResize");
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);
    }

} // namespace Solar::Game
