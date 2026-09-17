#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shellapi.h>
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
#include <algorithm>

namespace Solar::Game {

    static void OpenUrlInBrowser(const std::string& rawUrl) {
        if (rawUrl.empty()) return;
        std::string target = rawUrl;
        if (target.rfind("http://", 0) != 0 && target.rfind("https://", 0) != 0) {
            target = "https://" + target;
        }
        ShellExecuteA(NULL, "open", target.c_str(), NULL, NULL, SW_SHOWNORMAL);
        ImGui::SetClipboardText(rawUrl.c_str());
        Audio::PlayClick();
        std::string notifMsg = "Opening: " + target;
        Notify::Success("Link Opened", notifMsg.c_str());
    }

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

        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        // -------------------------------------------------------------
        // STYLE 1: MinimalPill
        // -------------------------------------------------------------
        if (info.style == WatermarkStyle::MinimalPill) {
            float pillW = 16.0f + titleSize.x + 10.0f + (info.showLink && !info.customLink.empty() ? linkSize.x + 18.0f : 0.0f) + 14.0f;
            float pillH = 26.0f;
            float px = (info.position == WatermarkPosition::TopLeft || info.position == WatermarkPosition::BottomLeft) ? 20.0f : io.DisplaySize.x - pillW - 20.0f;
            float py = (info.position == WatermarkPosition::BottomLeft || info.position == WatermarkPosition::BottomRight) ? io.DisplaySize.y - pillH - 20.0f : 16.0f;
            ImVec2 pMin(px, py), pMax(px + pillW, py + pillH);

            draw->AddRectFilled(pMin, pMax, IM_COL32(11, 13, 18, 230), pillH * 0.5f);
            Render::ImGuiExt::AddSmoothBorder(draw, pMin, pMax, pal.Accent.WithAlpha(0.65f).ToU32(), pillH * 0.5f, 1.0f);

            // Glowing dot
            float dotPulse = 0.7f + 0.3f * std::sin(timeSec * 4.0f);
            draw->AddCircleFilled(ImVec2(px + 12.0f, py + pillH * 0.5f), 3.0f, pal.Accent.WithAlpha(dotPulse).ToU32(), 12);
            draw->AddText(ImVec2(px + 20.0f, py + (pillH - titleSize.y) * 0.5f), pal.TextPrimary.ToU32(), info.frameworkName.c_str());

            if (info.showLink && !info.customLink.empty()) {
                float linkX = px + 24.0f + titleSize.x;
                ImVec2 lMin(linkX, py + 3.0f), lMax(linkX + linkSize.x + 12.0f, py + pillH - 3.0f);
                bool lHover = io.MousePos.x >= lMin.x && io.MousePos.x <= lMax.x && io.MousePos.y >= lMin.y && io.MousePos.y <= lMax.y;
                draw->AddRectFilled(lMin, lMax, pal.Accent.WithAlpha(lHover ? 0.30f : 0.15f).ToU32(), 4.0f);
                draw->AddText(ImVec2(linkX + 6.0f, py + (pillH - linkSize.y) * 0.5f), lHover ? pal.AccentHover.ToU32() : pal.Accent.ToU32(), linkDisplayBuf);

                if (lHover && info.interactive) {
                    ImGui::SetTooltip("Click to open link: %s", info.customLink.c_str());
                    if (ImGui::IsMouseClicked(0)) {
                        OpenUrlInBrowser(info.customLink);
                    }
                }
            }
            return;
        }

        // -------------------------------------------------------------
        // STYLE 2: NeonTerminal ([ SOLAR // 144 FPS // 12 MS ])
        // -------------------------------------------------------------
        if (info.style == WatermarkStyle::NeonTerminal) {
            char termBuf[180];
            snprintf(termBuf, sizeof(termBuf), "[ %s // %s // %s%s%s ]",
                     info.frameworkName.c_str(), fpsBuf, pingBuf,
                     (!info.customLink.empty() && info.showLink) ? " // " : "",
                     (!info.customLink.empty() && info.showLink) ? info.customLink.c_str() : "");
            ImVec2 tSz = ImGui::CalcTextSize(termBuf);
            float tW = tSz.x + 24.0f;
            float tH = 26.0f;
            float px = (info.position == WatermarkPosition::TopLeft || info.position == WatermarkPosition::BottomLeft) ? 20.0f : io.DisplaySize.x - tW - 20.0f;
            float py = (info.position == WatermarkPosition::BottomLeft || info.position == WatermarkPosition::BottomRight) ? io.DisplaySize.y - tH - 20.0f : 16.0f;
            ImVec2 pMin(px, py), pMax(px + tW, py + tH);

            draw->AddRectFilled(pMin, pMax, IM_COL32(8, 10, 15, 235), 4.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, pMin, pMax, IM_COL32(35, 220, 180, 140), 4.0f, 1.0f);
            draw->AddText(ImVec2(px + 12.0f, py + (tH - tSz.y) * 0.5f), IM_COL32(40, 240, 195, 255), termBuf);

            bool hovered = io.MousePos.x >= pMin.x && io.MousePos.x <= pMax.x && io.MousePos.y >= pMin.y && io.MousePos.y <= pMax.y;
            if (hovered && info.interactive && !info.customLink.empty()) {
                ImGui::SetTooltip("Click to open terminal link: %s", info.customLink.c_str());
                if (ImGui::IsMouseClicked(0)) {
                    OpenUrlInBrowser(info.customLink);
                }
            }
            return;
        }

        // -------------------------------------------------------------
        // STYLE 3: DiscreteCorner
        // -------------------------------------------------------------
        if (info.style == WatermarkStyle::DiscreteCorner) {
            char dBuf[128];
            snprintf(dBuf, sizeof(dBuf), "%s | %s | %s", info.frameworkName.c_str(), fpsBuf, info.customLink.c_str());
            ImVec2 dSz = ImGui::CalcTextSize(dBuf);
            float px = 18.0f;
            float py = 16.0f;
            draw->AddRectFilled(ImVec2(px - 4.0f, py - 2.0f), ImVec2(px - 1.0f, py + dSz.y + 2.0f), pal.Accent.ToU32());
            draw->AddText(ImVec2(px + 4.0f, py), pal.TextPrimary.WithAlpha(0.85f).ToU32(), dBuf);
            ImVec2 pMin(px, py), pMax(px + dSz.x, py + dSz.y);
            bool hovered = io.MousePos.x >= pMin.x && io.MousePos.x <= pMax.x && io.MousePos.y >= pMin.y && io.MousePos.y <= pMax.y;
            if (hovered && info.interactive && !info.customLink.empty()) {
                ImGui::SetTooltip("Click to open: %s", info.customLink.c_str());
                if (ImGui::IsMouseClicked(0)) {
                    OpenUrlInBrowser(info.customLink);
                }
            }
            return;
        }

        // -------------------------------------------------------------
        // STYLE 0 (Default) & 3 (HoloBadge): CyberBar
        // -------------------------------------------------------------
        float padX = 14.0f;
        float sunBoxW = 20.0f;
        float sepW = 12.0f;

        float contentWidth = padX * 2.0f + sunBoxW + titleSize.x + 6.0f + verSize.x;
        if (info.showLink && !info.customLink.empty()) {
            contentWidth += sepW + linkSize.x + 14.0f;
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

        ImVec2 min(posX, posY);
        ImVec2 max(posX + contentWidth, posY + totalH);

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

        // Elevation shadow
        for (int i = 1; i <= 3; ++i) {
            float expand = static_cast<float>(i) * 1.5f;
            draw->AddRectFilled(
                ImVec2(min.x - expand, min.y - expand + 2.0f),
                ImVec2(max.x + expand, max.y + expand + 2.0f),
                IM_COL32(0, 0, 0, 18 / i),
                7.0f + expand
            );
        }

        // Obsidian Glass Container
        draw->AddRectFilled(min, max, IM_COL32(11, 12, 16, 242), 7.0f);
        Render::ImGuiExt::DrawSpecularEdge(draw, min, max, IM_COL32(255, 255, 255, hovered ? 38 : 22), 7.0f, 1.0f);
        u32 borderCol = hovered ? pal.Accent.WithAlpha(0.85f).ToU32() : pal.Border.WithAlpha(0.60f).ToU32();
        Render::ImGuiExt::AddSmoothBorder(draw, min, max, borderCol, 7.0f, 1.0f);

        // Cyber terminal icon badge
        float centerY = min.y + totalH * 0.5f;
        float curX = min.x + padX;
        ImVec2 markC(curX + 6.0f, centerY);
        draw->AddRectFilled(ImVec2(curX, centerY - 8.0f), ImVec2(curX + 16.0f, centerY + 8.0f), pal.Accent.WithAlpha(0.20f).ToU32(), 3.0f);
        draw->AddText(ImVec2(curX + 3.0f, centerY - 6.0f), pal.Accent.ToU32(), ICON_FA_TERMINAL);
        curX += sunBoxW;

        // Title
        draw->AddText(ImVec2(curX, centerY - titleSize.y * 0.5f), pal.TextPrimary.ToU32(), info.frameworkName.c_str());
        curX += titleSize.x + 6.0f;

        // Version Pill
        float vPillW = verSize.x + 10.0f;
        float vPillH = 18.0f;
        ImVec2 vMin(curX, centerY - vPillH * 0.5f);
        ImVec2 vMax(curX + vPillW, centerY + vPillH * 0.5f);
        draw->AddRectFilled(vMin, vMax, pal.Header.WithAlpha(0.85f).ToU32(), 4.0f);
        draw->AddRect(vMin, vMax, pal.Border.ToU32(), 4.0f, 0, 1.0f);
        draw->AddText(ImVec2(curX + 5.0f, centerY - verSize.y * 0.5f), pal.TextDisabled.ToU32(), info.version.c_str());
        curX += vPillW + sepW * 0.5f;

        auto drawDivider = [&](float x) {
            draw->AddLine(ImVec2(x, centerY - 6.0f), ImVec2(x, centerY + 6.0f), pal.Border.WithAlpha(0.40f).ToU32(), 1.0f);
            curX += sepW * 0.5f;
        };

        // Custom Link Button (Interactive: Click to Open URL in default browser)
        if (info.showLink && !info.customLink.empty()) {
            drawDivider(curX);

            float pillW = linkSize.x + 12.0f;
            float pillH = 20.0f;
            ImVec2 pillMin(curX, centerY - pillH * 0.5f);
            ImVec2 pillMax(curX + pillW, centerY + pillH * 0.5f);

            bool linkHovered = io.MousePos.x >= pillMin.x && io.MousePos.x <= pillMax.x &&
                               io.MousePos.y >= pillMin.y && io.MousePos.y <= pillMax.y;

            if (linkHovered) {
                draw->AddRectFilled(pillMin, pillMax, pal.Accent.WithAlpha(0.25f).ToU32(), 4.0f);
                draw->AddRect(pillMin, pillMax, pal.Accent.WithAlpha(0.70f).ToU32(), 4.0f, 0, 1.0f);

                if (info.interactive) {
                    ImGui::SetTooltip("Click to open in browser: %s", info.customLink.c_str());
                    if (ImGui::IsMouseClicked(0)) {
                        OpenUrlInBrowser(info.customLink);
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

        // Online Status Dot & Username
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

        // Ping
        if (info.showPing) {
            drawDivider(curX);
            u32 pingCol = (info.pingMs < 45) ? IM_COL32(60, 220, 120, 255) :
                          (info.pingMs < 90) ? IM_COL32(255, 190, 40, 255) : IM_COL32(255, 70, 70, 255);
            draw->AddText(ImVec2(curX, centerY - pingSize.y * 0.5f), pingCol, pingBuf);
            curX += pingSize.x + sepW * 0.5f;
        }

        // FPS
        if (info.showFps) {
            drawDivider(curX);
            u32 fpsCol = (io.Framerate >= 100.0f) ? IM_COL32(60, 220, 120, 255) :
                         (io.Framerate >= 55.0f)  ? IM_COL32(255, 190, 40, 255) : IM_COL32(255, 70, 70, 255);
            draw->AddText(ImVec2(curX, centerY - fpsSize.y * 0.5f), fpsCol, fpsBuf);
            curX += fpsSize.x + sepW * 0.5f;
        }

        // Clock
        if (info.showTime) {
            drawDivider(curX);
            draw->AddText(ImVec2(curX, centerY - timeSize.y * 0.5f), pal.TextDisabled.ToU32(), timeStr);
        }
    }

} // namespace Solar::Game
