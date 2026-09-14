#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"

namespace Solar::Game {

    enum class ChamsMaterial {
        Flat = 0,
        Metallic,
        GlowOutline,
        Glass,
        Wireframe
    };

    class ChamsPreview {
    public:
        static void Render(const char* str_id, const ImVec2& size, ChamsMaterial mat, Color col);
    };

} // namespace Solar::Game
