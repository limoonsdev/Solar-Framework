#include "solar/fx/glow_fx.hpp"

namespace Solar::FX {

    void GlowFX::RenderAmbientBackgroundGlow(ImDrawList* draw, const ImVec2& min, const ImVec2& max, Color accent) {
        if (!draw) return;
        // Subtle top radial glow
        ImVec2 topCenter(min.x + (max.x - min.x) * 0.5f, min.y);
        f32 radius = (max.x - min.x) * 0.45f;
        draw->AddCircleFilled(topCenter, radius, accent.WithAlpha(0.035f).ToU32(), 36);
        draw->AddCircleFilled(topCenter, radius * 0.5f, accent.WithAlpha(0.045f).ToU32(), 36);
    }

} // namespace Solar::FX
