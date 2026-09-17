#pragma once
#include "solar/core/types.hpp"
#include "solar/render/font_manager.hpp"
#include <string>
#include <imgui.h>

namespace Solar::Game {

    enum class ScreenWatermarkLayout {
        CenterDiagonal = 0,   // Classic tilted security / streamer watermark across screen
        CenterHorizontal = 1, // Horizontal centered text
        BottomBanner = 2,     // Streamer HUD lower bar
        TopBanner = 3,        // Upper header text
        TiledMatrix = 4       // Fullscreen repeating anti-leak grid
    };

    enum class ScreenWatermarkEffect {
        SolidAlpha = 0,       // Clean low-opacity typography
        OutlinedGlow = 1,     // Crisp outline with subtle atmospheric glow
        DualFacetGradient = 2 // Dual-facet vertical theme gradient
    };

    struct ScreenWatermarkSettings {
        bool enabled = false;
        std::string text = ".gg/solarud";
        Render::GamingFontPreset fontPreset = Render::GamingFontPreset::ValorantTactical;
        float opacity = 0.12f;      // 0.05f to 0.50f
        float scale = 1.0f;        // 0.5f to 2.5f
        ScreenWatermarkLayout layout = ScreenWatermarkLayout::CenterDiagonal;
        ScreenWatermarkEffect effect = ScreenWatermarkEffect::SolidAlpha;
        bool useThemeColor = true; // Match active theme accent
        ImVec4 customColor = ImVec4(1.0f, 1.0f, 1.0f, 0.15f);
        float angleDegrees = -25.0f; // For diagonal layout
        bool animatedPulse = true;  // Subtle breathing pulse (+/- 0.02)
    };

    class ScreenWatermark {
    public:
        static void Render(const ScreenWatermarkSettings& settings);
    };

} // namespace Solar::Game
