#pragma once
#include "solar/core/types.hpp"
#include <vector>

namespace Solar::Game {

    struct HitboxZone {
        const char* name;
        bool enabled;
        ImVec2 relMin;
        ImVec2 relMax;
    };

    class HitboxPicker {
    public:
        static void Render(std::vector<bool>& hitboxes, const ImVec2& size = ImVec2(0, 220.0f));
    };

} // namespace Solar::Game
