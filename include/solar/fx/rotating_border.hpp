#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <imgui.h>

namespace Solar::FX {

    /**
     * @brief Rotating Glowing Border Engine
     * Generates a smooth, flowing parametric gradient border around any rectangle or card.
     * Features multi-pass Gaussian alpha decay for a refined luxury neon aura.
     */
    class RotatingBorder {
    public:
        static void Draw(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding,
                         Color colorA, Color colorB, float speed = 1.5f,
                         float thickness = 1.5f, float glowIntensity = 1.0f);
    };

    inline void DrawRotatingBorder(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding,
                                  Color colorA, Color colorB, float speed = 1.5f,
                                  float thickness = 1.5f, float glowIntensity = 1.0f) {
        RotatingBorder::Draw(draw, min, max, rounding, colorA, colorB, speed, thickness, glowIntensity);
    }

} // namespace Solar::FX
