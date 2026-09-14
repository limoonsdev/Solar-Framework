#pragma once
#include "solar/core/types.hpp"
#include <vector>

namespace Solar::Game {

    struct RadarBlip {
        ImVec2 pos;
        float angle = 0.0f;
        bool isEnemy = true;
        bool isDormant = false;
        int altitude = 0; // -1 = below, 0 = same, 1 = above
    };

    class Radar {
    public:
        static void Render(const char* str_id, const ImVec2& size, const std::vector<RadarBlip>& blips);
    };

} // namespace Solar::Game
