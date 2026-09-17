#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <imgui.h>

namespace Solar::FX {

    /**
     * @brief Rotation algorithm and color mode for rotating borders
     */
    enum class BorderRotationMode {
        TwoColorLerp = 0,   // Smooth oscillating wave between Color A and Color B
        RainbowSpectrum,    // Full 360-degree flowing HSV chroma cycle
        NeonPulse,          // Travelling intense neon energy comet with trailing fade
        CyberGradient,      // Tri-color cyber gradient (Cyan -> Purple -> Amber)
        DualOrbit           // Two opposing light beams orbiting simultaneously
    };

    /**
     * @brief Comprehensive configuration for rotating borders
     */
    struct RotatingBorderConfig {
        bool enabled = true;
        BorderRotationMode mode = BorderRotationMode::TwoColorLerp;
        Color colorA = Color(0.20f, 0.55f, 1.00f, 0.95f);  // Primary neon (Cyan/Blue)
        Color colorB = Color(0.70f, 0.20f, 1.00f, 0.95f);  // Secondary neon (Magenta/Purple)
        Color colorC = Color(1.00f, 0.40f, 0.10f, 0.95f);  // Accent neon (Amber/Orange)
        float speed = 1.6f;                                 // Rotation speed multiplier
        float thickness = 1.8f;                             // Core border thickness in pixels
        float glowIntensity = 1.0f;                         // Multi-pass glow aura multiplier (0.0f = crisp, 2.0f = intense)
        float trailLength = 0.35f;                          // Segment length for comet/pulse modes (0.1f - 1.0f)
        bool clockwise = true;                              // Rotation direction
        int glowPasses = 3;                                 // Number of Gaussian alpha decay passes
    };

    /**
     * @brief Rotating Glowing Border Engine
     * Generates a flowing parametric gradient border around any rectangle or card.
     * Features multi-pass Gaussian alpha decay for a refined luxury neon aura.
     */
    class RotatingBorder {
    public:
        // Comprehensive configurable draw
        static void Draw(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding,
                         const RotatingBorderConfig& config);

        // Backward-compatible two-color draw
        static void Draw(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding,
                         Color colorA, Color colorB, float speed = 1.5f,
                         float thickness = 1.5f, float glowIntensity = 1.0f);
    };

    inline void DrawRotatingBorder(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding,
                                  const RotatingBorderConfig& config) {
        RotatingBorder::Draw(draw, min, max, rounding, config);
    }

    inline void DrawRotatingBorder(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding,
                                  Color colorA, Color colorB, float speed = 1.5f,
                                  float thickness = 1.5f, float glowIntensity = 1.0f) {
        RotatingBorder::Draw(draw, min, max, rounding, colorA, colorB, speed, thickness, glowIntensity);
    }

} // namespace Solar::FX
