#include "solar/render/shadow_caster.hpp"
#include "solar/core/math.hpp"

namespace Solar::Render {

    void ShadowCaster::DrawShadow(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                  f32 radius, f32 rounding, Color shadowColor, ImVec2 offset) {
        if (!draw || radius <= 0.0f || shadowColor.a <= 0.001f) return;

        const int layers = 10;
        const float baseAlpha = shadowColor.a;
        const float step = radius / static_cast<float>(layers);

        for (int i = layers; i >= 1; --i) {
            float t = static_cast<float>(i) / static_cast<float>(layers);
            float expand = static_cast<float>(layers - i) * step;
            float layerAlpha = (t * t) * (baseAlpha / static_cast<float>(layers)) * 1.85f;
            if (layerAlpha > 1.0f) layerAlpha = 1.0f;

            ImVec2 sMin(min.x + offset.x - expand, min.y + offset.y - expand);
            ImVec2 sMax(max.x + offset.x + expand, max.y + offset.y + expand);

            u32 col = shadowColor.WithAlpha(layerAlpha).ToU32();
            draw->AddRectFilled(sMin, sMax, col, rounding + expand);
        }
    }

} // namespace Solar::Render
