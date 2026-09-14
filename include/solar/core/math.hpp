#pragma once
#include "types.hpp"
#include <cmath>
#include <algorithm>

namespace Solar::Math {

    constexpr f32 PI = 3.14159265358979323846f;
    constexpr f32 TWO_PI = 6.28318530717958647692f;
    constexpr f32 DEG_TO_RAD = PI / 180.0f;
    constexpr f32 RAD_TO_DEG = 180.0f / PI;

    inline f32 Clamp(f32 val, f32 minVal, f32 maxVal) {
        return std::max(minVal, std::min(maxVal, val));
    }

    inline f32 Lerp(f32 a, f32 b, f32 t) {
        return a + (b - a) * t;
    }

    inline ImVec2 Lerp(const ImVec2& a, const ImVec2& b, f32 t) {
        return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
    }

    inline ImVec4 Lerp(const ImVec4& a, const ImVec4& b, f32 t) {
        return ImVec4(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t
        );
    }

    inline f32 SmoothStep(f32 edge0, f32 edge1, f32 x) {
        f32 t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    inline f32 Distance(const ImVec2& a, const ImVec2& b) {
        f32 dx = b.x - a.x;
        f32 dy = b.y - a.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    inline f32 NormalizeAngle(f32 angle) {
        while (angle > 180.0f) angle -= 360.0f;
        while (angle < -180.0f) angle += 360.0f;
        return angle;
    }

} // namespace Solar::Math
