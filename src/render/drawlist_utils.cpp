#include "solar/render/drawlist_utils.hpp"
#include "solar/core/math.hpp"

namespace Solar::Render {

    void DrawUtils::DrawRoundedRect(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                    u32 col, f32 rounding, ImDrawFlags flags) {
        if (!draw) return;
        draw->AddRectFilled(min, max, col, rounding, flags);
    }

    void DrawUtils::DrawRoundedRectBorder(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                          u32 col, f32 rounding, f32 thickness, ImDrawFlags flags) {
        if (!draw) return;
        draw->AddRect(min, max, col, rounding, flags, thickness);
    }

    void DrawUtils::DrawGradientRectV(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                      u32 colTop, u32 colBottom, f32 rounding) {
        if (!draw) return;
        if (rounding <= 0.0f) {
            draw->AddRectFilledMultiColor(min, max, colTop, colTop, colBottom, colBottom);
        } else {
            draw->PathRect(min, max, rounding);
            draw->PathFillConvex(colTop);
            // Overlap subtle bottom gradient
            ImVec2 mid(min.x, min.y + (max.y - min.y) * 0.5f);
            draw->AddRectFilledMultiColor(mid, max, 0x00000000, 0x00000000, colBottom, colBottom);
        }
    }

    void DrawUtils::DrawGradientRectH(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                      u32 colLeft, u32 colRight, f32 rounding) {
        if (!draw) return;
        draw->AddRectFilledMultiColor(min, max, colLeft, colRight, colRight, colLeft);
    }

    void DrawUtils::DrawCapsule(ImDrawList* draw, const ImVec2& p1, const ImVec2& p2,
                                f32 radius, u32 col) {
        if (!draw) return;
        draw->AddCircleFilled(p1, radius, col, 16);
        draw->AddCircleFilled(p2, radius, col, 16);
        draw->AddLine(p1, p2, col, radius * 2.0f);
    }

    void DrawUtils::DrawArc(ImDrawList* draw, const ImVec2& center, f32 radius,
                            f32 aMin, f32 aMax, u32 col, f32 thickness, i32 numSegments) {
        if (!draw) return;
        draw->PathArcTo(center, radius, aMin, aMax, numSegments);
        draw->PathStroke(col, 0, thickness);
    }

} // namespace Solar::Render
