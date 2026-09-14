#include "solar/game/obb_renderer.hpp"
#include "solar/theme/theme_manager.hpp"
#include <cmath>

namespace Solar::Game {

    static constexpr float DEG2RAD = 0.01745329251f;

    std::array<ImVec2, 8> OBBRenderer::CalculateVertices(const OBB3D& box) {
        std::array<ImVec2, 8> out;

        float hw = box.width * 0.5f;
        float hh = box.height * 0.5f;
        float hd = box.depth * 0.5f;

        float yaw = box.yawDeg * DEG2RAD;
        float pitch = box.pitchDeg * DEG2RAD;
        float roll = box.rollDeg * DEG2RAD;

        float cy = std::cos(yaw), sy = std::sin(yaw);
        float cp = std::cos(pitch), sp = std::sin(pitch);
        float cr = std::cos(roll), sr = std::sin(roll);

        // Local 3D corners
        float localPts[8][3] = {
            { -hw, -hh, -hd },
            {  hw, -hh, -hd },
            {  hw,  hh, -hd },
            { -hw,  hh, -hd },
            { -hw, -hh,  hd },
            {  hw, -hh,  hd },
            {  hw,  hh,  hd },
            { -hw,  hh,  hd }
        };

        for (int i = 0; i < 8; ++i) {
            float lx = localPts[i][0];
            float ly = localPts[i][1];
            float lz = localPts[i][2];

            // 1. Yaw (Y-axis)
            float x1 = lx * cy + lz * sy;
            float y1 = ly;
            float z1 = -lx * sy + lz * cy;

            // 2. Pitch (X-axis)
            float x2 = x1;
            float y2 = y1 * cp - z1 * sp;
            float z2 = y1 * sp + z1 * cp;

            // 3. Roll (Z-axis)
            float x3 = x2 * cr - y2 * sr;
            float y3 = x2 * sr + y2 * cr;
            float z3 = z2;

            // Perspective scale
            float fovDist = 320.0f;
            float scale = fovDist / (fovDist + z3);
            if (scale < 0.1f) scale = 0.1f;

            out[i] = ImVec2(box.screenCenter.x + x3 * scale, box.screenCenter.y + y3 * scale);
        }

        return out;
    }

    void OBBRenderer::Render(ImDrawList* draw, const OBB3D& box,
                             Color edgeColor, Color faceColor,
                             float thickness, bool fillFaces) {
        if (!draw) return;

        auto v = CalculateVertices(box);
        u32 colEdge = ThemeManager::ToU32(edgeColor);
        u32 colFace = ThemeManager::ToU32(faceColor);

        // Optional Translucent Planar Face Fills
        if (fillFaces && faceColor.a > 0.001f) {
            // Front & Back faces
            draw->AddQuadFilled(v[0], v[1], v[2], v[3], colFace);
            draw->AddQuadFilled(v[4], v[5], v[6], v[7], colFace);

            // Left & Right faces
            draw->AddQuadFilled(v[0], v[3], v[7], v[4], colFace);
            draw->AddQuadFilled(v[1], v[2], v[6], v[5], colFace);

            // Top & Bottom faces
            draw->AddQuadFilled(v[0], v[1], v[5], v[4], colFace);
            draw->AddQuadFilled(v[3], v[2], v[6], v[7], colFace);
        }

        // 12 Wireframe Edges
        // Front ring
        draw->AddLine(v[0], v[1], colEdge, thickness);
        draw->AddLine(v[1], v[2], colEdge, thickness);
        draw->AddLine(v[2], v[3], colEdge, thickness);
        draw->AddLine(v[3], v[0], colEdge, thickness);

        // Back ring
        draw->AddLine(v[4], v[5], colEdge, thickness);
        draw->AddLine(v[5], v[6], colEdge, thickness);
        draw->AddLine(v[6], v[7], colEdge, thickness);
        draw->AddLine(v[7], v[4], colEdge, thickness);

        // Connecting pillars
        draw->AddLine(v[0], v[4], colEdge, thickness);
        draw->AddLine(v[1], v[5], colEdge, thickness);
        draw->AddLine(v[2], v[6], colEdge, thickness);
        draw->AddLine(v[3], v[7], colEdge, thickness);
    }

} // namespace Solar::Game
