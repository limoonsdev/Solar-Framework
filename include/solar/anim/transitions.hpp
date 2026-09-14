#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"

namespace Solar::Anim {

    struct TransitionState {
        f32 value = 0.0f;
        f32 target = 0.0f;
        f32 speed = 10.0f;

        void Update(f32 dt, f32 targetValue, f32 transitionSpeed = 12.0f);
        f32 Get() const { return value; }
    };

} // namespace Solar::Anim
