#include "solar/core/color.hpp"
#include "solar/core/math.hpp"
#include <sstream>
#include <iomanip>

namespace Solar {

    Color::Color(u32 hexU32) {
        a = ((hexU32 >> 24) & 0xFF) / 255.0f;
        b = ((hexU32 >> 16) & 0xFF) / 255.0f;
        g = ((hexU32 >> 8)  & 0xFF) / 255.0f;
        r = ((hexU32)       & 0xFF) / 255.0f;
    }

    u32 Color::ToU32() const {
        return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
    }

    Color Color::FromHex(const std::string& hexStr) {
        std::string clean = hexStr;
        if (!clean.empty() && clean[0] == '#') clean = clean.substr(1);
        if (clean.length() == 6) clean = "FF" + clean;
        u32 val = 0;
        std::stringstream ss;
        ss << std::hex << clean;
        ss >> val;
        f32 alpha = ((val >> 24) & 0xFF) / 255.0f;
        f32 red   = ((val >> 16) & 0xFF) / 255.0f;
        f32 green = ((val >> 8)  & 0xFF) / 255.0f;
        f32 blue  = ((val)       & 0xFF) / 255.0f;
        return Color(red, green, blue, alpha);
    }

    Color Color::FromHSV(f32 h, f32 s, f32 v, f32 a) {
        f32 r1, g1, b1;
        ImGui::ColorConvertHSVtoRGB(h, s, v, r1, g1, b1);
        return Color(r1, g1, b1, a);
    }

    void Color::ToHSV(f32& h, f32& s, f32& v) const {
        ImGui::ColorConvertRGBtoHSV(r, g, b, h, s, v);
    }

    Color Color::Lerp(const Color& c1, const Color& c2, f32 t) {
        return Color(
            Math::Lerp(c1.r, c2.r, t),
            Math::Lerp(c1.g, c2.g, t),
            Math::Lerp(c1.b, c2.b, t),
            Math::Lerp(c1.a, c2.a, t)
        );
    }

} // namespace Solar
