#include "solar/game/watermark.hpp"
#include "solar/theme/theme_manager.hpp"
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
        ImVec2 userSize = ImGui::CalcTextSize(info.username.c_str());
        ImVec2 pingSize = ImGui::CalcTextSize(pingBuf);
        ImVec2 fpsSize = ImGui::CalcTextSize(fpsBuf);
        ImVec2 timeSize = ImGui::CalcTextSize(timeStr);

        float padX = 12.0f;
        float padY = 8.0f;
        float sunRadius = 6.0f;
        float sunBoxW = 22.0f;
        float sepW = 12.0f;

        float contentWidth = sunBoxW + titleSize.x + 6.0f + verSize.x + sepW
                           + 14.0f + userSize.x + sepW
                           + pingSize.x + sepW
                           + fpsSize.x + sepW
                           + timeSize.x + padX * 2.0f;

        float totalH = 32.0f;
        float posX = io.DisplaySize.x - contentWidth - 18.0f;
        float posY = 18.0f;

        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        ImVec2 min(posX, posY);
        ImVec2 max(posX + contentWidth, posY + totalH);

        // 1. Multi-layered subtle drop shadow
        for (int i = 1; i <= 4; ++i) {
            float expand = static_cast<float>(i) * 1.5f;
            draw->AddRectFilled(
                ImVec2(min.x - expand, min.y - expand + 2.0f),
                ImVec2(max.x + expand, max.y + expand + 2.0f),
                IM_COL32(0, 0, 0, 18 / i),
                6.0f + expand
            );
        }

        // 2. Obsidian Glass Container
        draw->AddRectFilled(min, max, IM_COL32(13, 14, 18, 238), 6.0f);
        draw->AddRect(min, max, IM_COL32(255, 255, 255, 18), 6.0f, 0, 1.0f);

        // 3. Top Amber Accent Edge (Fluid gradient)
        draw->AddLine(
            ImVec2(min.x + 8.0f, min.y + 0.5f),
            ImVec2(min.x + 60.0f, min.y + 0.5f),
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
            draw->AddCircleFilled(sunCenter, sunRadius * 1.6f, pal.Accent.WithAlpha(0.20f * pulse).ToU32(), 16);

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
            draw->AddCircleFilled(sunCenter, sunRadius * 0.5f, IM_COL32(255, 255, 255, 220), 12);

            curX += sunBoxW;
        }

        // 5. Framework Brand Title
        draw->AddText(ImVec2(curX, centerY - titleSize.y * 0.5f), pal.Accent.ToU32(), info.frameworkName.c_str());
        curX += titleSize.x + 6.0f;

        // Version badge tag
        draw->AddText(ImVec2(curX, centerY - verSize.y * 0.5f), pal.TextDisabled.ToU32(), info.version.c_str());
        curX += verSize.x + sepW * 0.5f;

        auto drawDivider = [&](float& x) {
            draw->AddLine(ImVec2(x, min.y + 8.0f), ImVec2(x, max.y - 8.0f), IM_COL32(255, 255, 255, 26), 1.0f);
            x += sepW * 0.5f;
        };

        drawDivider(curX);

        // 6. Online Status Dot & Username
        {
            float dotRadius = 3.0f;
            ImVec2 dotPos(curX + dotRadius, centerY);
            draw->AddCircleFilled(dotPos, dotRadius + 1.5f, IM_COL32(35, 210, 110, 80), 12);
            draw->AddCircleFilled(dotPos, dotRadius, IM_COL32(40, 225, 120, 255), 12);
            curX += dotRadius * 2.0f + 6.0f;

            draw->AddText(ImVec2(curX, centerY - userSize.y * 0.5f), pal.TextPrimary.ToU32(), info.username.c_str());
            curX += userSize.x + sepW * 0.5f;
        }

        drawDivider(curX);

        // 7. Latency Indicator (Color-coded)
        {
            u32 pingCol = (info.pingMs < 45) ? IM_COL32(60, 220, 120, 255) :
                          (info.pingMs < 90) ? IM_COL32(255, 190, 40, 255) : IM_COL32(255, 70, 70, 255);
            draw->AddText(ImVec2(curX, centerY - pingSize.y * 0.5f), pingCol, pingBuf);
            curX += pingSize.x + sepW * 0.5f;
        }

        drawDivider(curX);

        // 8. FPS Meter
        draw->AddText(ImVec2(curX, centerY - fpsSize.y * 0.5f), pal.TextSecondary.ToU32(), fpsBuf);
        curX += fpsSize.x + sepW * 0.5f;

        drawDivider(curX);

        // 9. Clock Time
        draw->AddText(ImVec2(curX, centerY - timeSize.y * 0.5f), pal.TextDisabled.ToU32(), timeStr);
    }

} // namespace Solar::Game
