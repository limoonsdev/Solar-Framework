#include "solar/game/trajectory_renderer.hpp"
#include "solar/theme/theme_manager.hpp"
#include <cmath>
#include <cstdio>

namespace Solar::Game {

    void TrajectoryRenderer::Render(const char* str_id, const ImVec2& size, ProjectileType type,
                                   float throwAngleDeg, float throwPower, Color arcColor) {
        ImGui::BeginChild(str_id, size, false, ImGuiWindowFlags_NoScrollbar);
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 sz = ImGui::GetWindowSize();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        // 1. Dark Viewport Canvas
        draw->AddRectFilled(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), IM_COL32(11, 12, 16, 245), 6.0f);
        draw->AddRect(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), IM_COL32(255, 255, 255, 18), 6.0f);

        // Ground line
        float groundY = pos.y + sz.y * 0.82f;
        draw->AddLine(ImVec2(pos.x, groundY), ImVec2(pos.x + sz.x, groundY), IM_COL32(255, 255, 255, 25), 1.4f);

        // Simulation parameters
        float rad = throwAngleDeg * (3.14159265f / 180.0f);
        float velocity = 380.0f * throwPower;
        float vx = velocity * std::cos(rad);
        float vy = -velocity * std::sin(rad);
        float gravity = 420.0f;

        ImVec2 curPos(pos.x + 28.0f, groundY - 60.0f);
        std::vector<TrajectoryPoint> points;
        points.push_back({ curPos, false, false });

        float dt = 0.035f;
        int bounces = 0;
        int maxBounces = 2;

        for (int step = 0; step < 120; ++step) {
            curPos.x += vx * dt;
            curPos.y += vy * dt;
            vy += gravity * dt;

            // Collision with ground
            if (curPos.y >= groundY) {
                curPos.y = groundY;
                points.push_back({ curPos, true, (bounces == maxBounces) });
                bounces++;
                if (bounces > maxBounces) break;

                // Bounce elasticity
                vy = -vy * 0.42f;
                vx = vx * 0.65f;
            } else {
                points.push_back({ curPos, false, false });
            }

            if (curPos.x > pos.x + sz.x - 20.0f) break;
        }

        // 2. Draw Trajectory Line
        size_t n = points.size();
        for (size_t i = 1; i < n; ++i) {
            draw->AddLine(points[i - 1].pos, points[i].pos, IM_COL32(0, 0, 0, 180), 2.8f);
            draw->AddLine(points[i - 1].pos, points[i].pos, arcColor.ToU32(), 1.6f);
        }

        // 3. Draw Bounce & Detonation Markers
        for (size_t i = 0; i < n; ++i) {
            if (points[i].isBounce && !points[i].isDetonation) {
                // Diamond bounce icon
                ImVec2 bp = points[i].pos;
                float d = 4.0f;
                draw->AddQuadFilled(
                    ImVec2(bp.x, bp.y - d), ImVec2(bp.x + d, bp.y),
                    ImVec2(bp.x, bp.y + d), ImVec2(bp.x - d, bp.y),
                    IM_COL32(255, 200, 40, 240)
                );
            } else if (points[i].isDetonation) {
                // Landing Blast Radius Circle
                ImVec2 dp = points[i].pos;
                float blastRadius = 32.0f;
                u32 blastCol = (type == ProjectileType::HighExplosive) ? IM_COL32(255, 60, 40, 60) :
                               (type == ProjectileType::SmokeGrenade)  ? IM_COL32(180, 180, 210, 60) :
                               (type == ProjectileType::Molotov)       ? IM_COL32(255, 120, 20, 60) :
                                                                         IM_COL32(255, 230, 80, 60);

                draw->AddCircleFilled(dp, blastRadius, blastCol, 32);
                draw->AddCircle(dp, blastRadius, arcColor.ToU32(), 32, 1.4f);
                draw->AddCircleFilled(dp, 4.0f, IM_COL32(255, 255, 255, 255), 12);
            }
        }

        // Header and projectile type
        const char* typeName = (type == ProjectileType::HighExplosive) ? "HE Grenade (Blast Area)" :
                               (type == ProjectileType::SmokeGrenade)  ? "Smoke Grenade (Screening Sphere)" :
                               (type == ProjectileType::Molotov)       ? "Molotov (Incendiary Field)" : "Flashbang (Blind Radius)";
        draw->AddText(ImVec2(pos.x + 10.0f, pos.y + 8.0f), pal.Accent.ToU32(), typeName);

        char angleBuf[48];
        snprintf(angleBuf, sizeof(angleBuf), "Angle: %.0f deg  |  Velocity: %.0f m/s", throwAngleDeg, velocity);
        draw->AddText(ImVec2(pos.x + 10.0f, pos.y + 24.0f), pal.TextSecondary.ToU32(), angleBuf);

        ImGui::EndChild();
    }

} // namespace Solar::Game
