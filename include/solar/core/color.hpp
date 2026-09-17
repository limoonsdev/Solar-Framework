#pragma once
#include "types.hpp"
#include <string>

namespace Solar {

    class Color {
    public:
        f32 r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

        constexpr Color() = default;
        constexpr Color(f32 _r, f32 _g, f32 _b, f32 _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}
        constexpr Color(const ImVec4& v) : r(v.x), g(v.y), b(v.z), a(v.w) {}
        Color(u32 hexU32);

        constexpr operator ImVec4() const { return ImVec4(r, g, b, a); }
        constexpr ImVec4 ToVec4() const { return ImVec4(r, g, b, a); }
        u32 ToU32() const;

        Color WithAlpha(f32 newAlpha) const { return Color(r, g, b, newAlpha); }
        Color Multiplied(f32 factor) const { return Color(r * factor, g * factor, b * factor, a); }

        static Color FromHex(const std::string& hexStr);
        static Color FromHSV(f32 h, f32 s, f32 v, f32 a = 1.0f);
        void ToHSV(f32& h, f32& s, f32& v) const;

        static Color Lerp(const Color& c1, const Color& c2, f32 t);
    };

} // namespace Solar
