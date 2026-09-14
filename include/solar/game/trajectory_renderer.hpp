#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <imgui.h>
#include <vector>

namespace Solar::Game {

    enum class ProjectileType : u8 {
        HighExplosive = 0,
        SmokeGrenade,
        Flashbang,
        Molotov
    };

    struct TrajectoryPoint {
        ImVec2 pos;
        bool isBounce = false;
        bool isDetonation = false;
    };

    /**
     * @brief Ballistic Trajectory & Grenade Prediction Engine
     * Simulates 2D/3D grenade throw arcs, bounce physics, and blast radius.
     */
    class TrajectoryRenderer {
    public:
        static void Render(const char* str_id, const ImVec2& size, ProjectileType type,
                           float throwAngleDeg = 45.0f, float throwPower = 1.0f,
                           Color arcColor = Color(0.25f, 0.72f, 1.0f, 0.9f));
    };

} // namespace Solar::Game
