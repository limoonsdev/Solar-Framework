#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <imgui.h>
#include <string>

namespace Solar::Render {

    /**
     * @brief Sanitized Label View (strips Dear ImGui '##' unique IDs from visual display)
     */
    struct LabelView {
        const char* textBegin;
        const char* textEnd;
        ImVec2      size;
    };

    inline LabelView CleanLabel(const char* label) {
        if (!label || label[0] == '\0') {
            return { "", "", ImVec2(0.0f, 0.0f) };
        }
        const char* textEnd = label;
        while (*textEnd != '\0') {
            if (textEnd[0] == '#' && textEnd[1] == '#') {
                break;
            }
            textEnd++;
        }
        ImVec2 size = ImGui::CalcTextSize(label, textEnd);
        return { label, textEnd, size };
    }

    /**
     * @brief High-Performance Modded ImGui Drawlist Extensions
     * Provides pre-allocated vertex batching, custom gradient primitives,
     * and crash-safe drawing utilities.
     */
    class ImGuiExt {
    public:
        // Reserve buffer memory ahead of frame to eliminate heap fragmentation during ESP rendering
        static void ReserveDrawlistBuffers(ImDrawList* draw, int vtxCount = 4096, int idxCount = 6144);

        // Crash-proof text rendering (validates strings, prevents null dereferencing)
        static void SafeText(ImDrawList* draw, const ImVec2& pos, u32 color, const char* text, const char* textEnd = nullptr);

        // Render rounded pill capsule with anti-aliasing
        static void AddPill(ImDrawList* draw, const ImVec2& min, const ImVec2& max, u32 fillColor, u32 borderColor = 0, float borderThickness = 1.0f);

        // Render glowing tactical bone capsule
        static void AddGlowCapsule(ImDrawList* draw, const ImVec2& p1, const ImVec2& p2, float radius, Color coreColor, Color glowColor, float glowIntensity = 1.0f);

        // Fast Conic Rotating Gradient around rectangle
        static void AddConicGradientRect(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding, Color col1, Color col2, float angleOffset, float thickness = 1.5f);

        // Specular top-edge horizontal gradient sheen (Peach-Framework technique)
        static void DrawSpecularEdge(ImDrawList* draw, const ImVec2& min, const ImVec2& max, u32 peakColor = IM_COL32(255, 255, 255, 22), float insetX = 10.0f, float height = 1.0f);

        // Hierarchical vertical connecting tree rail for nested sub-navigation
        static void DrawHierarchicalRail(ImDrawList* draw, float railX, float topY, float bottomY, u32 railColor);

        // Pixel-perfect, anti-aliased inset border (half-pixel offset eliminates blur and jagged stair-stepping)
        static void AddSmoothBorder(ImDrawList* draw, const ImVec2& min, const ImVec2& max, u32 borderColor, float rounding = 0.0f, float thickness = 1.0f);

        // Interactive luxury corner resize grip for ANY window with custom minimum dimensions
        static void RenderResizeGrip(const ImVec2& minSize = ImVec2(140.0f, 90.0f), const char* idStr = "##WindowResizeGrip");
    };

} // namespace Solar::Render
