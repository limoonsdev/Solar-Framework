#pragma once
#include "solar/widgets/radar.hpp"
#include <vector>

namespace Solar::Game {

    /**
     * @brief Floating external square Tactical Radar HUD window (matches Spectator & Keybind HUD boxes)
     */
    class RadarWindow {
    public:
        static void Render(bool* p_open,
                           const Widgets::RadarSettings& settings,
                           const std::vector<Widgets::RadarEntity>& entities);
    };

} // namespace Solar::Game
