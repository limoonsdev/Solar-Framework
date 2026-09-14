#include "solar/anim/spring.hpp"

namespace Solar::Anim {

    void Spring::Update(f32 dt) {
        if (dt <= 0.0f) return;
        f32 force = -stiffness * (value - target) - damping * velocity;
        velocity += force * dt;
        value += velocity * dt;
    }

} // namespace Solar::Anim
