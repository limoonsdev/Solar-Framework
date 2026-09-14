#pragma once

#include <imgui.h>
#include <string>

namespace Solar {

    struct WatermarkInfo {
        std::string frameworkName = "SOLAR";
        std::string version = "v1.0";
        std::string username = "SolarDev";
        int pingMs = 14;
        bool showFps = true;
        bool showTime = true;
        bool showPing = true;
    };

    class Watermark {
    public:
        static void Render(const WatermarkInfo& info = WatermarkInfo());
    };

} // namespace Solar
