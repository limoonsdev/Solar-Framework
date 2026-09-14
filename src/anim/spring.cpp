#include "solar/anim/spring.hpp"
#include <cmath>

namespace Solar::Anim {

    void Spring::Update(f32 dt) {
        if (dt <= 0.0f) return;
        if (dt > 0.064f) dt = 0.064f; // Clamp large hitch deltas for stability

        f32 m = (mass > 0.001f) ? mass : 1.0f;
        f32 k = stiffness / m;
        f32 c = damping / m;

        // Substep 1
        f32 x1 = value - target;
        f32 v1 = velocity;
        f32 a1 = -k * x1 - c * v1;

        // Substep 2
        f32 x2 = x1 + 0.5f * dt * v1;
        f32 v2 = v1 + 0.5f * dt * a1;
        f32 a2 = -k * x2 - c * v2;

        // Substep 3
        f32 x3 = x1 + 0.5f * dt * v2;
        f32 v3 = v1 + 0.5f * dt * a2;
        f32 a3 = -k * x3 - c * v3;

        // Substep 4
        f32 x4 = x1 + dt * v3;
        f32 v4 = v1 + dt * a3;
        f32 a4 = -k * x4 - c * v4;

        // RK4 combination
        f32 dxdt = (v1 + 2.0f * (v2 + v3) + v4) / 6.0f;
        f32 dvdt = (a1 + 2.0f * (a2 + a3) + a4) / 6.0f;

        value += dxdt * dt;
        velocity += dvdt * dt;

        // Micro-deadband snapping
        if (std::abs(value - target) < 0.0005f && std::abs(velocity) < 0.0005f) {
            value = target;
            velocity = 0.0f;
        }
    }

    bool Spring::IsSettled(f32 tolerance) const {
        return (std::abs(value - target) <= tolerance) && (std::abs(velocity) <= tolerance);
    }

} // namespace Solar::Anim
