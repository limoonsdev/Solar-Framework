#include "solar/game/watermark.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui.h>
#include <cstdio>
#include <ctime>

namespace Solar::Game {

    void Watermark::Render(const WatermarkInfo& info) {
        ImGuiIO& io = ImGui::GetIO();

        time_t rawtime;
        time(&rawtime);
        struct tm* ti = localtime(&rawtime);
        char timeStr[16];
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", ti);

        char text[128];
        snprintf(text, sizeof(text), "%s %s | %s | %d ms | %d fps | %s",
                 info.frameworkName.c_str(),
                 info.version.c_str(),
                 info.username.c_str(),
                 info.pingMs,
                 static_cast<int>(io.Framerate),
                 timeStr);

        ImVec2 ts = ImGui::CalcTextSize(text);
        float pad = 8.0f;
        float w = ts.x + pad * 2.0f + 12.0f;
        float h = ts.y + pad * 2.0f;

        float posX = io.DisplaySize.x - w - 16.0f;
        float posY = 16.0f;

        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        ImVec2 min(posX, posY);
        ImVec2 max(posX + w, posY + h);

        draw->AddRectFilled(min, max, pal.Header.WithAlpha(0.92f).ToU32(), 5.0f);
        draw->AddRect(min, max, pal.Border.ToU32(), 5.0f);

        // Pulsing amber dot
        ImVec2 dot(min.x + 12.0f, min.y + h * 0.5f);
        draw->AddCircleFilled(dot, 3.5f, pal.Accent.ToU32(), 16);

        draw->AddText(ImVec2(min.x + 22.0f, min.y + pad), pal.TextPrimary.ToU32(), text);
    }

} // namespace Solar::Game
