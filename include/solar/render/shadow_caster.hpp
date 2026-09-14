#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"

namespace Solar::Render {

    class ShadowCaster {
    public:
        // High fidelity multi-pass drop shadow without bilinear blur pixelation
        static void DrawShadow(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                               f32 radius = 18.0f, f32 rounding = 8.0f,
                               Color shadowColor = Color(0.0f, 0.0f, 0.0f, 0.45f),
                               ImVec2 offset = ImVec2(0.0f, 6.0f));
    };

} // namespace Solar::Render
