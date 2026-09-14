#pragma once
#include "solar/core/types.hpp"
#include "solar/core/math.hpp"
#include <cmath>

namespace Solar::Anim {

    enum class EasingType {
        Linear = 0,
        InQuad,
        OutQuad,
        InOutQuad,
        InCubic,
        OutCubic,
        InOutCubic,
        OutBack,
        OutExpo
    };

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

    inline f32 ApplyEasing(f32 t, EasingType type) {
        switch (type) {
            case EasingType::Linear: return Linear(t);
            case EasingType::InQuad: return InQuad(t);
            case EasingType::OutQuad: return OutQuad(t);
            case EasingType::InOutQuad: return InOutQuad(t);
            case EasingType::InCubic: return InCubic(t);
            case EasingType::OutCubic: return OutCubic(t);
            case EasingType::InOutCubic: return InOutCubic(t);
            case EasingType::OutBack: return OutBack(t);
            case EasingType::OutExpo: return OutExpo(t);
            default: return t;
        }
    }

} // namespace Solar::Anim
