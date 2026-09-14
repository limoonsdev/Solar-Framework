#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"

namespace Solar::Render {

    class DrawUtils {
    public:
        static void DrawRoundedRect(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                    u32 col, f32 rounding, ImDrawFlags flags = 0);

        static void DrawRoundedRectBorder(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                          u32 col, f32 rounding, f32 thickness = 1.0f, ImDrawFlags flags = 0);

        static void DrawGradientRectV(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                      u32 colTop, u32 colBottom, f32 rounding = 0.0f);

        static void DrawGradientRectH(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                      u32 colLeft, u32 colRight, f32 rounding = 0.0f);

        static void DrawCapsule(ImDrawList* draw, const ImVec2& p1, const ImVec2& p2,
                                f32 radius, u32 col);

        static void DrawArc(ImDrawList* draw, const ImVec2& center, f32 radius,
                            f32 aMin, f32 aMax, u32 col, f32 thickness = 1.0f, i32 numSegments = 32);
    };

} // namespace Solar::Render
