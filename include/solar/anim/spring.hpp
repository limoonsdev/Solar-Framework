#pragma once
#include "solar/core/types.hpp"

namespace Solar::Anim {

    class Spring {
    public:
        f32 value = 0.0f;
        f32 target = 0.0f;
        f32 velocity = 0.0f;
        f32 stiffness = 220.0f;
        f32 damping = 22.0f;
        f32 mass = 1.0f;

        Spring(f32 initial = 0.0f) : value(initial), target(initial) {}
        void Update(f32 dt);
        void SetTarget(f32 newTarget) { target = newTarget; }
        void SnapTo(f32 val) { value = val; target = val; velocity = 0.0f; }
        bool IsSettled(f32 tolerance = 0.001f) const;
    };

} // namespace Solar::Anim
