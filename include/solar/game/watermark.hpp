#pragma once
#include "solar/core/types.hpp"
#include <string>

namespace Solar::Game {

    enum class WatermarkPosition {
        TopRight = 0,
        TopLeft = 1,
        BottomRight = 2,
        BottomLeft = 3,
        Draggable = 4
    };

    struct WatermarkInfo {
        std::string frameworkName = "SOLAR";
        std::string version = "v1.0.2-dev";
        std::string username = "SolarDev";
        std::string customLink = "discord.gg/solarud";
        int pingMs = 12;

        bool showLink = true;
        bool showPing = true;
        bool showFps = true;
        bool showTime = true;
        bool showUser = true;
        bool showStatusDot = true;
        bool interactive = true; // Hover glow and click-to-copy to clipboard with Toast

        WatermarkPosition position = WatermarkPosition::TopRight;
        ImVec2 customPos = ImVec2(-1.0f, -1.0f);
    };

    class Watermark {
    public:
        static void Render(const WatermarkInfo& info);
    };

} // namespace Solar::Game
