#include "solar/anim/transitions.hpp"
#include "solar/core/math.hpp"

namespace Solar::Anim {

    void TransitionState::Update(f32 dt, f32 targetValue, f32 transitionSpeed) {
        target = targetValue;
        speed = transitionSpeed;
        f32 diff = target - value;
        if (std::abs(diff) < 0.001f) {
            value = target;
        } else {
            value += diff * Math::Clamp(dt * speed, 0.0f, 1.0f);
        }
    }

} // namespace Solar::Anim
