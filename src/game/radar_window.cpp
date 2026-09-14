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

        ImGui::SetNextWindowSize(ImVec2(240.0f, 290.0f), ImGuiCond_FirstUseEver);
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

            // 1. Soft Ambient Shadow & Specular Top Sheen
            Render::ShadowCaster::DrawShadow(draw, winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
                                            16.0f, 10.0f, Color(0, 0, 0, 0.50f), ImVec2(0, 4.0f));
            Render::ImGuiExt::DrawSpecularEdge(draw, winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
                                              IM_COL32(255, 255, 255, 30), 10.0f, 1.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
                                             pal.Border.ToU32(), 10.0f, 1.0f);

            // 2. Custom Window Header (Title + Icon + Close Button)
            float headerH = 28.0f;
            ImVec2 iconPos(winPos.x + 12.0f, winPos.y + 14.0f);
            draw->AddText(iconPos, pal.Accent.ToU32(), ICON_FA_CROSSHAIRS);

            ImVec2 titlePos(winPos.x + 32.0f, winPos.y + 12.0f);
            draw->AddText(titlePos, pal.TextPrimary.ToU32(), "Tactical Radar");

            // Close Button [x] on top right
            ImVec2 closePos(winPos.x + winSize.x - 24.0f, winPos.y + 8.0f);
            ImGui::SetCursorScreenPos(closePos);
            if (ImGui::InvisibleButton("##radar_close", ImVec2(18.0f, 18.0f))) {
                *p_open = false;
                Audio::PlayClick();
            }
            bool hoverClose = ImGui::IsItemHovered();
            draw->AddText(ImVec2(closePos.x + 3.0f, closePos.y + 2.0f),
                          hoverClose ? pal.Danger.ToU32() : pal.TextDisabled.ToU32(),
                          ICON_FA_XMARK);

            ImGui::SetCursorPosY(headerH + 6.0f);

            // 3. Square Tactical Radar Viewport
            float vpSize = winSize.x - 20.0f;
            ImVec2 vpMin = ImGui::GetCursorScreenPos();
            ImVec2 vpMax(vpMin.x + vpSize, vpMin.y + vpSize);
            ImVec2 vpCenter(vpMin.x + vpSize * 0.5f, vpMin.y + vpSize * 0.5f);
            float radius = vpSize * 0.46f;

            ImGui::Dummy(ImVec2(vpSize, vpSize));

            // Viewport Background
            draw->AddRectFilled(vpMin, vpMax, pal.Background.WithAlpha(0.95f).ToU32(), 6.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, vpMin, vpMax, pal.Border.WithAlpha(0.70f).ToU32(), 6.0f, 1.0f);

            // Square Grid lines
            float gridStep = vpSize / 4.0f;
            for (int i = 1; i <= 3; ++i) {
                float gx = vpMin.x + i * gridStep;
                float gy = vpMin.y + i * gridStep;
                u32 gridCol = (i == 2) ? pal.Border.WithAlpha(0.40f).ToU32() : pal.Border.WithAlpha(0.18f).ToU32();
                draw->AddLine(ImVec2(gx, vpMin.y), ImVec2(gx, vpMax.y), gridCol, 1.0f);
                draw->AddLine(ImVec2(vpMin.x, gy), ImVec2(vpMax.x, gy), gridCol, 1.0f);
            }

            // Concentric Range Rings
            draw->AddCircle(vpCenter, radius * 0.5f, pal.Border.WithAlpha(0.25f).ToU32(), 48, 1.0f);
            draw->AddCircle(vpCenter, radius, pal.Border.WithAlpha(0.35f).ToU32(), 48, 1.0f);

            // Range indicator badge inside viewport
            char rangeBuf[16];
            std::snprintf(rangeBuf, sizeof(rangeBuf), "%.0fm", settings.rangeMeters);
            draw->AddText(ImVec2(vpMax.x - 30.0f, vpMin.y + 4.0f), pal.TextDisabled.WithAlpha(0.60f).ToU32(), rangeBuf);

            // North indicator
            draw->AddText(ImVec2(vpCenter.x - 4.0f, vpMin.y + 3.0f), pal.Accent.ToU32(), "N");

            // 4. Rotating 360-Degree Sweep Beam
            if (settings.showSweep) {
                static float s_winSweepAngle = 0.0f;
                s_winSweepAngle += ImGui::GetIO().DeltaTime * settings.sweepSpeed;
                if (s_winSweepAngle >= 360.0f) s_winSweepAngle -= 360.0f;

                float sweepRad = s_winSweepAngle * (3.14159265f / 180.0f);
                const int trailSegments = 12;
                const float trailArc = 40.0f * (3.14159265f / 180.0f);

                for (int t = 0; t < trailSegments; ++t) {
                    float a0 = sweepRad - trailArc * (static_cast<float>(t + 1) / static_cast<float>(trailSegments));
                    float a1 = sweepRad - trailArc * (static_cast<float>(t) / static_cast<float>(trailSegments));

                    float alpha = (1.0f - static_cast<float>(t) / static_cast<float>(trailSegments)) * 0.16f;
                    u32 wedgeCol = pal.Accent.WithAlpha(alpha).ToU32();

                    ImVec2 p0(vpCenter.x + std::cos(a0) * radius, vpCenter.y + std::sin(a0) * radius);
                    ImVec2 p1(vpCenter.x + std::cos(a1) * radius, vpCenter.y + std::sin(a1) * radius);

                    draw->AddTriangleFilled(vpCenter, p0, p1, wedgeCol);
                }

                ImVec2 beamEnd(vpCenter.x + std::cos(sweepRad) * radius, vpCenter.y + std::sin(sweepRad) * radius);
                draw->AddLine(vpCenter, beamEnd, pal.Accent.WithAlpha(0.75f).ToU32(), 1.5f);
            }

            // 5. Center Local Player Blip & Forward View Cone
            draw->AddCircleFilled(vpCenter, 4.0f, IM_COL32(255, 255, 255, 255), 16);
            draw->AddCircle(vpCenter, 4.0f, pal.Accent.ToU32(), 16, 1.5f);

            float fovLen = 14.0f;
            float fovAngle = 35.0f * (3.14159265f / 180.0f);
            ImVec2 fovLeft(vpCenter.x - std::sin(fovAngle) * fovLen, vpCenter.y - std::cos(fovAngle) * fovLen);
            ImVec2 fovRight(vpCenter.x + std::sin(fovAngle) * fovLen, vpCenter.y - std::cos(fovAngle) * fovLen);
            draw->AddLine(vpCenter, fovLeft, pal.Accent.WithAlpha(0.60f).ToU32(), 1.0f);
            draw->AddLine(vpCenter, fovRight, pal.Accent.WithAlpha(0.60f).ToU32(), 1.0f);

            // 6. Entity Blips in Square Radar
            float meterToPix = radius / (settings.rangeMeters > 0.001f ? settings.rangeMeters : 1.0f);
            int enemyCount = 0;
            int friendlyCount = 0;

            for (const auto& ent : entities) {
                if (ent.isEnemy) enemyCount++; else friendlyCount++;

                float relX = ent.x * meterToPix;
                float relY = -ent.y * meterToPix;
                float dist = std::sqrt(relX * relX + relY * relY);

                ImVec2 blipPos;
                bool clamped = false;

                if (dist > radius - 3.0f) {
                    float norm = (radius - 3.0f) / dist;
                    blipPos = ImVec2(vpCenter.x + relX * norm, vpCenter.y + relY * norm);
                    clamped = true;
                } else {
                    blipPos = ImVec2(vpCenter.x + relX, vpCenter.y + relY);
                }

                Color blipColor = ent.isEnemy ? pal.Danger : Color(0.18f, 0.78f, 0.52f, 1.0f);
                if (ent.isDormant) blipColor = blipColor.WithAlpha(0.40f);

                if (clamped) {
                    float angle = std::atan2(relY, relX);
                    ImVec2 arrTip(vpCenter.x + std::cos(angle) * (radius - 1.0f), vpCenter.y + std::sin(angle) * (radius - 1.0f));
                    ImVec2 arrB1(vpCenter.x + std::cos(angle + 0.22f) * (radius - 6.0f), vpCenter.y + std::sin(angle + 0.22f) * (radius - 6.0f));
                    ImVec2 arrB2(vpCenter.x + std::cos(angle - 0.22f) * (radius - 6.0f), vpCenter.y + std::sin(angle - 0.22f) * (radius - 6.0f));
                    draw->AddTriangleFilled(arrTip, arrB1, arrB2, blipColor.ToU32());
                } else {
                    float blipRadius = 3.5f;
                    draw->AddCircleFilled(blipPos, blipRadius, blipColor.ToU32(), 16);
                    draw->AddCircle(blipPos, blipRadius, IM_COL32(255, 255, 255, 130), 16, 1.0f);

                    if (settings.showHeadingCones) {
                        float yawRad = (ent.yaw - 90.0f) * (3.14159265f / 180.0f);
                        ImVec2 dirEnd(blipPos.x + std::cos(yawRad) * 7.0f, blipPos.y + std::sin(yawRad) * 7.0f);
                        draw->AddLine(blipPos, dirEnd, blipColor.ToU32(), 1.5f);
                    }

                    if (std::abs(ent.z) > 1.8f) {
                        const char* altChar = ent.z > 0.0f ? "^" : "v";
                        draw->AddText(ImVec2(blipPos.x + 4.0f, blipPos.y - 7.0f), blipColor.ToU32(), altChar);
                    }
                }
            }

            // 7. Compact Footer Status Chip
            ImGui::SetCursorPosY(headerH + vpSize + 12.0f);
            ImGui::Dummy(ImVec2(winSize.x - 20.0f, 20.0f));
            char statusBuf[48];
            std::snprintf(statusBuf, sizeof(statusBuf), "%d Enemies  |  %d Friendly", enemyCount, friendlyCount);
            ImVec2 stSize = ImGui::CalcTextSize(statusBuf);
            ImVec2 stMin(winPos.x + 10.0f, winPos.y + winSize.y - 24.0f);
            ImVec2 stMax(winPos.x + winSize.x - 10.0f, winPos.y + winSize.y - 6.0f);

            draw->AddRectFilled(stMin, stMax, pal.Header.WithAlpha(0.60f).ToU32(), 4.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, stMin, stMax, pal.Border.WithAlpha(0.40f).ToU32(), 4.0f, 1.0f);
            draw->AddText(ImVec2(stMin.x + (winSize.x - 20.0f - stSize.x) * 0.5f, stMin.y + 1.0f),
                          pal.TextSecondary.ToU32(), statusBuf);
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);
    }

} // namespace Solar::Game
