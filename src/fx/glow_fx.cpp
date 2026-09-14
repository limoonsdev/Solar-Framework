#include "solar/fx/glow_fx.hpp"
#include <cmath>

namespace Solar::FX {

    void GlowFX::RenderAmbientBackgroundGlow(ImDrawList* draw, const ImVec2& min, const ImVec2& max, Color accent) {
        if (!draw) return;

        float time = static_cast<float>(ImGui::GetTime());
        float breath = 0.85f + 0.15f * std::sin(time * 1.8f);

        // 1. Primary top-center ambient bloom
        ImVec2 topCenter(min.x + (max.x - min.x) * 0.5f, min.y);
        f32 radius = (max.x - min.x) * 0.50f;

        draw->AddCircleFilled(topCenter, radius * 1.2f, accent.WithAlpha(0.020f * breath).ToU32(), 48);
        draw->AddCircleFilled(topCenter, radius * 0.8f, accent.WithAlpha(0.038f * breath).ToU32(), 40);
        draw->AddCircleFilled(topCenter, radius * 0.4f, accent.WithAlpha(0.060f * breath).ToU32(), 32);

        // 2. Corner specular atmospheric haze
        ImVec2 topLeft(min.x, min.y);
        draw->AddCircleFilled(topLeft, 120.0f, accent.WithAlpha(0.018f * breath).ToU32(), 24);

        ImVec2 topRight(max.x, min.y);
        draw->AddCircleFilled(topRight, 120.0f, accent.WithAlpha(0.018f * breath).ToU32(), 24);
    }

} // namespace Solar::FX
