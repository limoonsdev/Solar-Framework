#include "solar/render/shadow_caster.hpp"
#include "solar/core/math.hpp"

namespace Solar::Render {

    void ShadowCaster::DrawShadow(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                  f32 radius, f32 rounding, Color shadowColor, ImVec2 offset) {
        if (!draw || radius <= 0.0f) return;

        const int passes = 6;
        f32 baseAlpha = shadowColor.a / static_cast<f32>(passes);

        for (int i = 1; i <= passes; i++) {
            f32 expand = (radius / static_cast<f32>(passes)) * i;
            f32 falloff = 1.0f - (static_cast<f32>(i) / static_cast<f32>(passes + 1));
            f32 passAlpha = baseAlpha * (falloff * falloff);

            ImVec2 sMin(min.x + offset.x - expand, min.y + offset.y - expand);
            ImVec2 sMax(max.x + offset.x + expand, max.y + offset.y + expand);

            u32 col = shadowColor.WithAlpha(passAlpha).ToU32();
            draw->AddRect(sMin, sMax, col, rounding + expand, 0, expand);
        }
    }

} // namespace Solar::Render
