#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <imgui.h>
#include <vector>

namespace Solar::Widgets {

    struct RadarEntity {
        float x = 0.0f;          // Relative X distance (meters, right = positive)
        float y = 0.0f;          // Relative Y distance (meters, forward = positive)
        float z = 0.0f;          // Relative Z distance (meters, up = positive)
        float yaw = 0.0f;        // Facing angle in degrees (0 = north/forward, 90 = east)
        bool  isEnemy = true;    // True for enemy, false for teammate
        bool  isDormant = false;  // Faded blip for stale network updates
        float healthFrac = 1.0f; // 0.0f to 1.0f
    };

    enum class RadarShape {
        Circle = 0,
        RoundedSquare,
        Square
    };

    struct RadarSettings {
        float rangeMeters = 40.0f;      // Radar radius in meters
        bool  showSweep = true;         // Continuous rotating radar sweep
        bool  showHeadingCones = true;  // Entity directional view cones
        bool  showCardinal = true;      // N, S, E, W markers
        bool  showRings = true;         // Range concentric circles
        float sweepSpeed = 90.0f;       // Degrees per second
        RadarShape shape = RadarShape::Circle;
        float zoom = 1.0f;              // Zoom scale
        float blipSize = 3.5f;          // Blip radius in pixels
        bool  showGrid = true;          // Tactical Cartesian grid lines
    };

    /**
     * @brief Luxury tactical circular radar display with sweep beam and directional blips
     * @param label Unique ID and optional title
     * @param size Dimensions of widget (uses min(size.x, size.y) for diameter)
     * @param settings Radar configuration parameters
     * @param entities List of tracked entity blips
     */
    void Radar(const char* label, const ImVec2& size,
               const RadarSettings& settings,
               const std::vector<RadarEntity>& entities);

} // namespace Solar::Widgets
