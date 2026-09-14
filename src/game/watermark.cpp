#include "solar/game/watermark.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/notifications/notification_system.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/font_awesome.hpp"
#include <imgui.h>
#include <cstdio>
#include <ctime>
#include <cmath>

namespace Solar::Game {

    void Watermark::Render(const WatermarkInfo& info) {
        ImGuiIO& io = ImGui::GetIO();
        float timeSec = static_cast<float>(ImGui::GetTime());

        time_t rawtime;
        time(&rawtime);
        struct tm* ti = localtime(&rawtime);
        char timeStr[16];
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", ti);

        char pingBuf[32];
        snprintf(pingBuf, sizeof(pingBuf), "%d ms", info.pingMs);

        char fpsBuf[32];
        snprintf(fpsBuf, sizeof(fpsBuf), "%d fps", static_cast<int>(io.Framerate));

        // Typography sizes
        ImVec2 titleSize = ImGui::CalcTextSize(info.frameworkName.c_str());
        ImVec2 verSize = ImGui::CalcTextSize(info.version.c_str());
        ImVec2 userSize = info.showUser ? ImGui::CalcTextSize(info.username.c_str()) : ImVec2(0, 0);
        ImVec2 pingSize = info.showPing ? ImGui::CalcTextSize(pingBuf) : ImVec2(0, 0);
        ImVec2 fpsSize = info.showFps ? ImGui::CalcTextSize(fpsBuf) : ImVec2(0, 0);
        ImVec2 timeSize = info.showTime ? ImGui::CalcTextSize(timeStr) : ImVec2(0, 0);

        char linkDisplayBuf[140];
        ImVec2 linkSize(0, 0);
        if (info.showLink && !info.customLink.empty()) {
            snprintf(linkDisplayBuf, sizeof(linkDisplayBuf), "%s %s", ICON_FA_LINK, info.customLink.c_str());
            linkSize = ImGui::CalcTextSize(linkDisplayBuf);
        }

        float padX = 14.0f;
        float sunRadius = 6.0f;
        float sunBoxW = 22.0f;
        float sepW = 12.0f;

        float contentWidth = padX * 2.0f + sunBoxW + titleSize.x + 6.0f + verSize.x;

        if (info.showLink && !info.customLink.empty()) {
            contentWidth += sepW + linkSize.x + 14.0f; // Extra pill padding
        }
        if (info.showUser) {
            contentWidth += sepW + 16.0f + userSize.x;
        }
        if (info.showPing) {
            contentWidth += sepW + pingSize.x;
        }
        if (info.showFps) {
            contentWidth += sepW + fpsSize.x;
        }
        if (info.showTime) {
            contentWidth += sepW + timeSize.x;
        }

        float totalH = 34.0f;
        float posX = 18.0f;
        float posY = 18.0f;

        static ImVec2 s_dragOffset(18.0f, 18.0f);
        static bool s_isDragging = false;

        switch (info.position) {
        case WatermarkPosition::TopRight:
            posX = io.DisplaySize.x - contentWidth - 18.0f;
            posY = 18.0f;
            break;
        case WatermarkPosition::TopLeft:
            posX = 18.0f;
            posY = 18.0f;
            break;
        case WatermarkPosition::BottomRight:
            posX = io.DisplaySize.x - contentWidth - 18.0f;
            posY = io.DisplaySize.y - totalH - 18.0f;
            break;
        case WatermarkPosition::BottomLeft:
            posX = 18.0f;
            posY = io.DisplaySize.y - totalH - 18.0f;
            break;
        case WatermarkPosition::Draggable:
            posX = s_dragOffset.x;
            posY = s_dragOffset.y;
            break;
        }

        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        ImVec2 min(posX, posY);
        ImVec2 max(posX + contentWidth, posY + totalH);

        // Check overall hovering and draggable behavior
        bool hovered = io.MousePos.x >= min.x && io.MousePos.x <= max.x &&
                       io.MousePos.y >= min.y && io.MousePos.y <= max.y;

        if (info.position == WatermarkPosition::Draggable) {
            if (hovered && io.MouseDown[0] && !s_isDragging) {
                s_isDragging = true;
            }
            if (s_isDragging) {
                if (io.MouseDown[0]) {
                    s_dragOffset.x += io.MouseDelta.x;
                    s_dragOffset.y += io.MouseDelta.y;
                    if (s_dragOffset.x < 5.0f) s_dragOffset.x = 5.0f;
                    if (s_dragOffset.y < 5.0f) s_dragOffset.y = 5.0f;
                    if (s_dragOffset.x + contentWidth > io.DisplaySize.x - 5.0f) s_dragOffset.x = io.DisplaySize.x - contentWidth - 5.0f;
                    if (s_dragOffset.y + totalH > io.DisplaySize.y - 5.0f) s_dragOffset.y = io.DisplaySize.y - totalH - 5.0f;
                    min = ImVec2(s_dragOffset.x, s_dragOffset.y);
                    max = ImVec2(s_dragOffset.x + contentWidth, s_dragOffset.y + totalH);
                } else {
                    s_isDragging = false;
                }
            }
        }

        // 1. Multi-layered subtle elevation drop shadow
        for (int i = 1; i <= 4; ++i) {
            float expand = static_cast<float>(i) * 1.5f;
            draw->AddRectFilled(
                ImVec2(min.x - expand, min.y - expand + 2.0f),
                ImVec2(max.x + expand, max.y + expand + 2.0f),
                IM_COL32(0, 0, 0, 18 / i),
                7.0f + expand
            );
        }

        // 2. Obsidian Glass Container
        draw->AddRectFilled(min, max, IM_COL32(11, 12, 16, 242), 7.0f);
        Render::ImGuiExt::AddSmoothBorder(draw, min, max, IM_COL32(255, 255, 255, 20), 7.0f, 1.0f);

        // 3. Top Specular Accent Edge (Fluid gradient)
        draw->AddLine(
            ImVec2(min.x + 10.0f, min.y + 0.5f),
            ImVec2(min.x + 65.0f, min.y + 0.5f),
            pal.Accent.ToU32(),
            1.5f
        );

        float curX = min.x + padX;
        float centerY = min.y + totalH * 0.5f;

        // 4. Custom Vector Solar Sun Insignia
        {
            ImVec2 sunCenter(curX + sunRadius + 2.0f, centerY);
            float pulse = 0.85f + 0.15f * std::sin(timeSec * 3.0f);

            // Corona Glow
            draw->AddCircleFilled(sunCenter, sunRadius * 1.6f, pal.Accent.WithAlpha(0.22f * pulse).ToU32(), 16);

            // Radiating Solar Rays (8 vector spikes)
            float rayInner = sunRadius + 1.2f;
            float rayOuter = sunRadius + 3.8f * pulse;
            for (int r = 0; r < 8; ++r) {
                float angle = static_cast<float>(r) * (3.14159265f / 4.0f) + (timeSec * 0.4f);
                ImVec2 r1(sunCenter.x + std::cos(angle) * rayInner, sunCenter.y + std::sin(angle) * rayInner);
                ImVec2 r2(sunCenter.x + std::cos(angle) * rayOuter, sunCenter.y + std::sin(angle) * rayOuter);
                draw->AddLine(r1, r2, pal.Accent.WithAlpha(0.80f).ToU32(), 1.2f);
            }

            // Core Sun Sphere
            draw->AddCircleFilled(sunCenter, sunRadius, pal.Accent.ToU32(), 16);
            draw->AddCircleFilled(sunCenter, sunRadius * 0.5f, IM_COL32(255, 255, 255, 230), 12);

            curX += sunBoxW;
        }

        // 5. Framework Brand Title
        draw->AddText(ImVec2(curX, centerY - titleSize.y * 0.5f), pal.Accent.ToU32(), info.frameworkName.c_str());
        curX += titleSize.x + 6.0f;

        // Version badge tag
        draw->AddText(ImVec2(curX, centerY - verSize.y * 0.5f), pal.TextDisabled.ToU32(), info.version.c_str());
        curX += verSize.x + sepW * 0.5f;

        auto drawDivider = [&](float& x) {
            draw->AddLine(ImVec2(x, min.y + 8.0f), ImVec2(x, max.y - 8.0f), IM_COL32(255, 255, 255, 24), 1.0f);
            x += sepW * 0.5f;
        };

        // 6. Custom Link Badge (Interactive click to copy with toast)
        if (info.showLink && !info.customLink.empty()) {
            drawDivider(curX);

            float pillW = linkSize.x + 12.0f;
            float pillH = 20.0f;
            ImVec2 pillMin(curX, centerY - pillH * 0.5f);
            ImVec2 pillMax(curX + pillW, centerY + pillH * 0.5f);

            bool linkHovered = io.MousePos.x >= pillMin.x && io.MousePos.x <= pillMax.x &&
                               io.MousePos.y >= pillMin.y && io.MousePos.y <= pillMax.y;

            if (linkHovered) {
                draw->AddRectFilled(pillMin, pillMax, pal.Accent.WithAlpha(0.20f).ToU32(), 4.0f);
                draw->AddRect(pillMin, pillMax, pal.Accent.WithAlpha(0.60f).ToU32(), 4.0f, 0, 1.0f);

                if (info.interactive) {
                    ImGui::SetTooltip("Click to copy link: %s", info.customLink.c_str());
                    if (ImGui::IsMouseClicked(0)) {
                        ImGui::SetClipboardText(info.customLink.c_str());
                        Audio::PlayClick();
                        std::string notifMsg = info.customLink + " copied to clipboard!";
                        Notify::Success("Link Copied", notifMsg.c_str());
                    }
                }
            } else {
                draw->AddRectFilled(pillMin, pillMax, IM_COL32(255, 255, 255, 10), 4.0f);
                draw->AddRect(pillMin, pillMax, IM_COL32(255, 255, 255, 20), 4.0f, 0, 1.0f);
            }

            ImU32 linkCol = linkHovered ? pal.AccentHover.ToU32() : pal.Accent.ToU32();
            draw->AddText(ImVec2(curX + 6.0f, centerY - linkSize.y * 0.5f), linkCol, linkDisplayBuf);

            curX += pillW + sepW * 0.5f;
        }

        // 7. Online Status Dot & Username
        if (info.showUser) {
            drawDivider(curX);

            if (info.showStatusDot) {
                float dotRadius = 3.0f;
                ImVec2 dotPos(curX + dotRadius, centerY);
                float dotPulse = 0.7f + 0.3f * std::sin(timeSec * 4.0f);
                draw->AddCircleFilled(dotPos, dotRadius + 2.0f, IM_COL32(35, 210, 110, static_cast<int>(90 * dotPulse)), 12);
                draw->AddCircleFilled(dotPos, dotRadius, IM_COL32(45, 235, 130, 255), 12);
                curX += dotRadius * 2.0f + 6.0f;
            }

            draw->AddText(ImVec2(curX, centerY - userSize.y * 0.5f), pal.TextPrimary.ToU32(), info.username.c_str());
            curX += userSize.x + sepW * 0.5f;
        }

        // 8. Latency Indicator (Color-coded)
        if (info.showPing) {
            drawDivider(curX);

            u32 pingCol = (info.pingMs < 45) ? IM_COL32(60, 220, 120, 255) :
                          (info.pingMs < 90) ? IM_COL32(255, 190, 40, 255) : IM_COL32(255, 70, 70, 255);
            draw->AddText(ImVec2(curX, centerY - pingSize.y * 0.5f), pingCol, pingBuf);
            curX += pingSize.x + sepW * 0.5f;
        }

        // 9. FPS Meter
        if (info.showFps) {
            drawDivider(curX);
            draw->AddText(ImVec2(curX, centerY - fpsSize.y * 0.5f), pal.TextSecondary.ToU32(), fpsBuf);
            curX += fpsSize.x + sepW * 0.5f;
        }

        // 10. Clock Time
        if (info.showTime) {
            drawDivider(curX);
            draw->AddText(ImVec2(curX, centerY - timeSize.y * 0.5f), pal.TextDisabled.ToU32(), timeStr);
        }
    }

} // namespace Solar::Game
