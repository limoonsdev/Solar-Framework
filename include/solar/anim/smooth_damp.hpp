#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>

namespace Solar::Anim {

    /**
     * @brief Critically damped spring-damper (SmoothDamp)
     * Smoothly accelerates and decelerates toward target with zero overshoot.
     */
    class SmoothDamp {
    public:
        static f32 Calculate(f32 current, f32 target, f32& currentVelocity,
                             f32 smoothTime, f32 maxSpeed = 10000.0f, f32 dt = 0.016f);

        static ImVec2 Calculate2D(const ImVec2& current, const ImVec2& target, ImVec2& currentVelocity,
                                 f32 smoothTime, f32 maxSpeed = 10000.0f, f32 dt = 0.016f);
    };

} // namespace Solar::Anim
