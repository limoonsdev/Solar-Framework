#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"

namespace Solar::Render {

    class GlowEngine {
    public:
        static void DrawGlowRect(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                 Color glowColor, f32 radius = 12.0f, f32 rounding = 6.0f,
                                 f32 intensity = 1.0f);

        static void DrawGlowCircle(ImDrawList* draw, const ImVec2& center, f32 baseRadius,
                                   Color glowColor, f32 glowRadius = 14.0f, f32 intensity = 1.0f);
    };

} // namespace Solar::Render
