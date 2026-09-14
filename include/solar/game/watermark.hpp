#pragma once
#include "solar/core/types.hpp"
#include <string>

namespace Solar::Game {

    struct WatermarkInfo {
        std::string frameworkName = "SOLAR";
        std::string version = "v1.0.1";
        std::string username = "User";
        int pingMs = 15;
    };

    class Watermark {
    public:
        static void Render(const WatermarkInfo& info);
    };

} // namespace Solar::Game
