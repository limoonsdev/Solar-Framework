#include "solar/render/glow_engine.hpp"

namespace Solar::Render {

    void GlowEngine::DrawGlowRect(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                  Color glowColor, f32 radius, f32 rounding, f32 intensity) {
        if (!draw || radius <= 0.0f) return;

        const int passes = 4;
        f32 baseAlpha = (glowColor.a * 0.28f * intensity) / static_cast<f32>(passes);

        for (int i = 1; i <= passes; i++) {
            f32 expand = (radius / static_cast<f32>(passes)) * i;
            f32 passAlpha = baseAlpha * (1.0f - (static_cast<f32>(i) / static_cast<f32>(passes + 1)));

            ImVec2 gMin(min.x - expand, min.y - expand);
            ImVec2 gMax(max.x + expand, max.y + expand);
            u32 col = glowColor.WithAlpha(passAlpha).ToU32();
            draw->AddRect(gMin, gMax, col, rounding + expand, 0, 1.5f);
        }
    }

    void GlowEngine::DrawGlowCircle(ImDrawList* draw, const ImVec2& center, f32 baseRadius,
                                    Color glowColor, f32 glowRadius, f32 intensity) {
        if (!draw) return;
        const int rings = 4;
        f32 baseAlpha = (glowColor.a * 0.35f * intensity) / static_cast<f32>(rings);

        for (int i = rings; i >= 1; i--) {
            f32 r = baseRadius + (glowRadius / static_cast<f32>(rings)) * i;
            f32 alpha = baseAlpha * (1.0f - (static_cast<f32>(i) / static_cast<f32>(rings + 1)));
            draw->AddCircleFilled(center, r, glowColor.WithAlpha(alpha).ToU32(), 24);
        }
    }

} // namespace Solar::Render
