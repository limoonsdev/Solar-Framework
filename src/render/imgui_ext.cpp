#include "solar/render/imgui_ext.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui_internal.h>
#include <cmath>
#include <algorithm>

namespace Solar::Render {

    void ImGuiExt::ReserveDrawlistBuffers(ImDrawList* draw, int vtxCount, int idxCount) {
        if (!draw) return;
        if (draw->VtxBuffer.Capacity < draw->VtxBuffer.Size + vtxCount) {
            draw->VtxBuffer.reserve(draw->VtxBuffer.Size + vtxCount);
        }
        if (draw->IdxBuffer.Capacity < draw->IdxBuffer.Size + idxCount) {
            draw->IdxBuffer.reserve(draw->IdxBuffer.Size + idxCount);
        }
    }

    void ImGuiExt::SafeText(ImDrawList* draw, const ImVec2& pos, u32 color, const char* text, const char* textEnd) {
        if (!draw || !text || text[0] == '\0') return;
        draw->AddText(pos, color, text, textEnd);
    }

    void ImGuiExt::AddPill(ImDrawList* draw, const ImVec2& min, const ImVec2& max, u32 fillColor, u32 borderColor, float borderThickness) {
        if (!draw) return;
        float h = max.y - min.y;
        float rounding = h * 0.5f;

        draw->AddRectFilled(min, max, fillColor, rounding);
        if (borderColor != 0 && borderThickness > 0.0f) {
            draw->AddRect(min, max, borderColor, rounding, 0, borderThickness);
        }
    }

    void ImGuiExt::AddGlowCapsule(ImDrawList* draw, const ImVec2& p1, const ImVec2& p2, float radius, Color coreColor, Color glowColor, float glowIntensity) {
        if (!draw || radius <= 0.5f) return;

        // Multi-tier soft expansion glow
        if (glowIntensity > 0.05f) {
            for (int i = 3; i >= 1; --i) {
                float expand = static_cast<float>(i) * 2.2f;
                float alpha = (glowColor.a * 0.18f / static_cast<float>(i)) * glowIntensity;
                u32 gCol = glowColor.WithAlpha(alpha).ToU32();
                draw->AddLine(p1, p2, gCol, (radius + expand) * 2.0f);
            }
        }

        // Underline shadow for contrast
        draw->AddLine(p1, p2, IM_COL32(0, 0, 0, 200), radius * 2.0f + 1.6f);

        // Core capsule line
        draw->AddLine(p1, p2, coreColor.ToU32(), radius * 2.0f);
    }

    void ImGuiExt::AddConicGradientRect(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding, Color col1, Color col2, float angleOffset, float thickness) {
        if (!draw) return;

        float width = max.x - min.x;
        float height = max.y - min.y;
        if (width <= 2.0f || height <= 2.0f) return;

        ImVec2 center(min.x + width * 0.5f, min.y + height * 0.5f);
        const int numSegments = 32;

        for (int i = 0; i < numSegments; ++i) {
            float a1 = static_cast<float>(i) / static_cast<float>(numSegments) * 6.2831853f;
            float a2 = static_cast<float>(i + 1) / static_cast<float>(numSegments) * 6.2831853f;

            float t1 = 0.5f + 0.5f * std::sin(a1 - angleOffset);
            Color segCol = Color::Lerp(col1, col2, t1);

            // Interpolate points on rectangle perimeter
            auto getRectPoint = [&](float angle) -> ImVec2 {
                float cosA = std::cos(angle);
                float sinA = std::sin(angle);
                float halfW = width * 0.5f;
                float halfH = height * 0.5f;

                float scaleX = (cosA != 0.0f) ? (halfW / std::abs(cosA)) : halfW;
                float scaleY = (sinA != 0.0f) ? (halfH / std::abs(sinA)) : halfH;
                float scale = (std::min)(scaleX, scaleY);

                return ImVec2(center.x + cosA * scale, center.y + sinA * scale);
            };

            ImVec2 p1 = getRectPoint(a1);
            ImVec2 p2 = getRectPoint(a2);

            draw->AddLine(p1, p2, segCol.ToU32(), thickness);
        }
    }

    void ImGuiExt::DrawSpecularEdge(ImDrawList* draw, const ImVec2& min, const ImVec2& max, u32 peakColor, float insetX, float height) {
        if (!draw) return;
        float w = max.x - min.x;
        if (w <= insetX * 2.0f) return;

        float startX = min.x + insetX;
        float endX = max.x - insetX;
        float midX = startX + (endX - startX) * 0.5f;
        float y = min.y;

        u32 transparent = peakColor & 0x00FFFFFF; // Alpha 0

        // Left half: transparent -> peakColor
        draw->AddRectFilledMultiColor(
            ImVec2(startX, y), ImVec2(midX, y + height),
            transparent, peakColor, peakColor, transparent
        );
        // Right half: peakColor -> transparent
        draw->AddRectFilledMultiColor(
            ImVec2(midX, y), ImVec2(endX, y + height),
            peakColor, transparent, transparent, peakColor
        );
    }

    void ImGuiExt::DrawHierarchicalRail(ImDrawList* draw, float railX, float topY, float bottomY, u32 railColor) {
        if (!draw || bottomY <= topY) return;
        draw->AddLine(ImVec2(railX, topY), ImVec2(railX, bottomY), railColor, 1.5f);
    }

    void ImGuiExt::AddSmoothBorder(ImDrawList* draw, const ImVec2& min, const ImVec2& max, u32 borderColor, float rounding, float thickness) {
        if (!draw || borderColor == 0 || thickness <= 0.0f) return;
        draw->AddRect(min, max, borderColor, rounding, 0, thickness);
    }

    void ImGuiExt::RenderResizeGrip(const ImVec2& minSize, const char* idStr) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (!window || window->SkipItems) return;

        ImGuiIO& io = ImGui::GetIO();
        float gripSize = 16.0f;
        ImVec2 winPos = window->Pos;
        ImVec2 winSize = window->Size;
        ImVec2 gripPos(winPos.x + winSize.x - gripSize - 3.0f, winPos.y + winSize.y - gripSize - 3.0f);
        ImRect gripRect(gripPos, ImVec2(gripPos.x + gripSize, gripPos.y + gripSize));

        ImGuiID id = window->GetID(idStr ? idStr : "##WindowResizeGrip");
        bool hovered = false, held = false;
        ImGui::ButtonBehavior(gripRect, id, &hovered, &held);

        if (held && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 newSize(winSize.x + io.MouseDelta.x, winSize.y + io.MouseDelta.y);
            newSize.x = (std::max)(newSize.x, minSize.x);
            newSize.y = (std::max)(newSize.y, minSize.y);
            ImGui::SetWindowSize(newSize);
        }

        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();
        u32 gripCol = (held || hovered) ? pal.Accent.ToU32() : pal.TextDisabled.WithAlpha(0.38f).ToU32();

        for (int i = 0; i < 3; ++i) {
            float offset = static_cast<float>(i) * 4.5f;
            ImVec2 p1(gripPos.x + gripSize - offset, gripPos.y + gripSize);
            ImVec2 p2(gripPos.x + gripSize, gripPos.y + gripSize - offset);
            draw->AddLine(p1, p2, gripCol, 1.4f);
        }

        if (hovered || held) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
        }
    }

} // namespace Solar::Render

