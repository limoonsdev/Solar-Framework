#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <imgui.h>
#include <vector>
#include <string>

namespace Solar::Game {

    enum class RecoilWeapon : u8 {
        AK47 = 0,
        M4A4,
        Vandal,
        Phantom
    };

    /**
     * @brief Recoil Pattern Visualizer & Spray Compensation Engine
     * Renders exact ballistic recoil patterns, bullet sequence tracks,
     * and mouse compensation paths.
     */
    class RecoilVisualizer {
    public:
        static void Render(const char* str_id, const ImVec2& size, RecoilWeapon weapon,
                           int activeBullet = -1, bool showCompensation = true,
                           Color bulletColor = Color(1.0f, 0.72f, 0.15f, 1.0f));

        static const char* GetWeaponName(RecoilWeapon w);
    };

} // namespace Solar::Game
