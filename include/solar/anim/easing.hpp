#pragma once
#include "solar/core/types.hpp"
#include "solar/core/math.hpp"

namespace Solar::Anim {

    inline f32 Linear(f32 t) { return t; }
    inline f32 InQuad(f32 t) { return t * t; }
    inline f32 OutQuad(f32 t) { return t * (2.0f - t); }
    inline f32 InOutQuad(f32 t) {
        return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
    }

    inline f32 InCubic(f32 t) { return t * t * t; }
    inline f32 OutCubic(f32 t) { f32 f = t - 1.0f; return f * f * f + 1.0f; }
    inline f32 InOutCubic(f32 t) {
        return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
    }

    inline f32 OutBack(f32 t, f32 c1 = 1.70158f) {
        f32 c3 = c1 + 1.0f;
        f32 f = t - 1.0f;
        return 1.0f + c3 * f * f * f + c1 * f * f;
    }

    inline f32 OutExpo(f32 t) {
        return (t == 1.0f) ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
    }

} // namespace Solar::Anim
