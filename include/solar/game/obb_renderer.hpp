#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <imgui.h>
#include <array>

namespace Solar::Game {

    struct OBB3D {
        ImVec2 screenCenter;
        float  width = 50.0f;
        float  height = 130.0f;
        float  depth = 45.0f;
        float  yawDeg = 0.0f;
        float  pitchDeg = 0.0f;
        float  rollDeg = 0.0f;
    };

    /**
     * @brief 3D Oriented Bounding Box (OBB) Wireframe & Planar Face Renderer
     */
    class OBBRenderer {
    public:
        // Compute 8 projected screen vertices from 3D orientation
        static std::array<ImVec2, 8> CalculateVertices(const OBB3D& box);

        // Draw 3D wireframe box with optional translucent face fills
        static void Render(ImDrawList* draw, const OBB3D& box,
                           Color edgeColor = Color(0.24f, 0.70f, 1.0f, 0.95f),
                           Color faceColor = Color(0.24f, 0.70f, 1.0f, 0.12f),
                           float thickness = 1.5f, bool fillFaces = true);
    };

} // namespace Solar::Game
