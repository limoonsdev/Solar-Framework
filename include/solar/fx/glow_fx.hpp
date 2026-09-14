#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"

namespace Solar::FX {

    class GlowFX {
    public:
        static void RenderAmbientBackgroundGlow(ImDrawList* draw, const ImVec2& min, const ImVec2& max, Color accent);
    };

} // namespace Solar::FX
