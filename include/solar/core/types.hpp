#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <imgui.h>

namespace Solar {

    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using i8  = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;
    using f32 = float;
    using f64 = double;

    struct Vec2 {
        f32 x = 0.0f;
        f32 y = 0.0f;
        constexpr Vec2() = default;
        constexpr Vec2(f32 _x, f32 _y) : x(_x), y(_y) {}
        constexpr Vec2(const ImVec2& v) : x(v.x), y(v.y) {}
        constexpr operator ImVec2() const { return ImVec2(x, y); }
    };

    struct Rect {
        Vec2 min;
        Vec2 max;
        constexpr Rect() = default;
        constexpr Rect(Vec2 _min, Vec2 _max) : min(_min), max(_max) {}
        constexpr Rect(f32 x1, f32 y1, f32 x2, f32 y2) : min(x1, y1), max(x2, y2) {}
        f32 Width() const { return max.x - min.x; }
        f32 Height() const { return max.y - min.y; }
        Vec2 Center() const { return Vec2(min.x + Width() * 0.5f, min.y + Height() * 0.5f); }
        bool Contains(const Vec2& pt) const {
            return pt.x >= min.x && pt.x <= max.x && pt.y >= min.y && pt.y <= max.y;
        }
    };

    enum class KeyMode : i32 {
        Hold = 0,
        Toggle,
        AlwaysOn,
        Release
    };

    enum class Alignment : u8 {
        Left = 0,
        Center,
        Right
    };

} // namespace Solar
