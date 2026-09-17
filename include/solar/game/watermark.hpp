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

    enum class WatermarkStyle {
        CyberBar = 0,       // Full telemetry glass bar with brand, ping, fps, link, clock
        MinimalPill = 1,    // Compact rounded capsule with glowing dot and brand
        NeonTerminal = 2,   // Tactical monospace bracketed badge [ SOLAR // 144 FPS ]
        HoloBadge = 3,      // Esports angled chamfered badge with holographic sheen
        DiscreteCorner = 4  // Subtle minimal text tag with accent bar
    };

    struct WatermarkInfo {
        std::string frameworkName = "SOLAR";
        std::string version = "v1.0.4-dev";
        std::string username = "SolarDev";
        std::string customLink = "discord.gg/solarud";
        int pingMs = 12;

        bool showLink = true;
        bool showPing = true;
        bool showFps = true;
        bool showTime = true;
        bool showUser = true;
        bool showStatusDot = true;
        bool interactive = true; // Interactive clicks, hover glow, link opening

        WatermarkPosition position = WatermarkPosition::TopRight;
        WatermarkStyle style = WatermarkStyle::CyberBar;
        ImVec2 customPos = ImVec2(-1.0f, -1.0f);
    };

    class Watermark {
    public:
        static void Render(const WatermarkInfo& info);
    };

} // namespace Solar::Game
