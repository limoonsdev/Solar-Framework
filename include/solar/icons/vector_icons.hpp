#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include "solar/icons.hpp"
#include <imgui.h>

namespace Solar::Icons {

    /**
     * @brief Procedural Vector Icon Engine
     * Directly rasterizes 40+ high-precision vector icons using ImDrawList.
     * Guarantees zero missing glyphs, zero font file dependencies, zero '?' in octagons,
     * and crisp scaling at any DPI.
     */
    class VectorIconRenderer {
    public:
        // Render by IconType enum
        static void Draw(ImDrawList* draw, IconType icon, const ImVec2& center, float size, Color color);

        // Render by name string or FontAwesome glyph string fallback
        static bool DrawByGlyph(ImDrawList* draw, const char* glyphOrName, const ImVec2& center, float size, Color color);

        // Individual procedural primitives
        static void DrawCrosshairs(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawEye(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawSliders(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawPalette(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawFolder(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawUser(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawBell(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawShield(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawGear(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawKey(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawLock(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawFloppy(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawFire(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawSun(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawWand(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawTerminal(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawCheck(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawXmark(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawFingerprint(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawExpand(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawSkull(ImDrawList* draw, const ImVec2& c, float s, u32 col);
        static void DrawCube(ImDrawList* draw, const ImVec2& c, float s, u32 col);
    };

} // namespace Solar::Icons
