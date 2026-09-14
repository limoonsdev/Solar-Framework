#pragma once
#include "solar/core/types.hpp"

namespace Solar {

    struct Style {
        f32 WindowRounding = 8.0f;
        f32 CardRounding = 7.0f;
        f32 WidgetRounding = 5.0f;
        f32 PopupRounding = 6.0f;
        f32 ScrollbarRounding = 4.0f;

        f32 BorderSize = 1.0f;
        f32 CardBorderSize = 1.0f;

        ImVec2 WindowPadding = ImVec2(0.0f, 0.0f);
        ImVec2 FramePadding = ImVec2(10.0f, 7.0f);
        ImVec2 ItemSpacing = ImVec2(8.0f, 8.0f);

        bool EnableGlow = true;
        f32 GlowIntensity = 1.0f;
        f32 GlowRadius = 10.0f;

        bool EnableAudio = true;
        f32 AudioVolume = 1.0f;

        bool EnableParticles = true;
        i32 ParticleCount = 45;

        f32 AnimSpeed = 12.0f;
    };

} // namespace Solar
