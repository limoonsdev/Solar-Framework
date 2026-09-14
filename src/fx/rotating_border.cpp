#include "solar/fx/rotating_border.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

namespace Solar::FX {

    void RotatingBorder::Draw(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding,
                             Color colorA, Color colorB, float speed, float thickness, float glowIntensity) {
        if (!draw) return;

        float width = max.x - min.x;
        float height = max.y - min.y;
        if (width <= 4.0f || height <= 4.0f) return;

        float timeSec = static_cast<float>(ImGui::GetTime()) * speed;
        ImVec2 center(min.x + width * 0.5f, min.y + height * 0.5f);

        // Clamp rounding
        rounding = (std::min)(rounding, (std::min)(width, height) * 0.5f);

        // Generate polygonal path along the rounded rect boundary
        const int cornerSegs = 6;
        std::vector<ImVec2> path;
        path.reserve(cornerSegs * 4 + 4);

        auto addArc = [&](const ImVec2& arcCenter, float startAngle, float endAngle) {
            for (int i = 0; i <= cornerSegs; ++i) {
                float t = static_cast<float>(i) / static_cast<float>(cornerSegs);
                float a = startAngle + (endAngle - startAngle) * t;
                path.emplace_back(arcCenter.x + std::cos(a) * rounding, arcCenter.y + std::sin(a) * rounding);
            }
        };

        if (rounding > 0.5f) {
            // Top-Right
            addArc(ImVec2(max.x - rounding, min.y + rounding), -1.5707963f, 0.0f);
            // Bottom-Right
            addArc(ImVec2(max.x - rounding, max.y - rounding), 0.0f, 1.5707963f);
            // Bottom-Left
            addArc(ImVec2(min.x + rounding, max.y - rounding), 1.5707963f, 3.14159265f);
            // Top-Left
            addArc(ImVec2(min.x + rounding, min.y + rounding), 3.14159265f, 4.71238898f);
        } else {
            path = { min, ImVec2(max.x, min.y), max, ImVec2(min.x, max.y) };
        }

        size_t n = path.size();
        if (n < 4) return;

        // 1. Multi-pass luxury glow (Tight Gaussian alpha decay, zero blur/muddiness)
        if (glowIntensity > 0.05f) {
            const int glowPasses = 3;
            for (int gp = glowPasses; gp >= 1; --gp) {
                float expand = static_cast<float>(gp) * 1.6f;
                float alphaScale = (0.16f / static_cast<float>(gp)) * glowIntensity;

                for (size_t i = 0; i < n; ++i) {
                    size_t next = (i + 1) % n;
                    ImVec2 p1 = path[i];
                    ImVec2 p2 = path[next];

                    // Angle relative to center
                    float midAngle = std::atan2(((p1.y + p2.y) * 0.5f) - center.y, ((p1.x + p2.x) * 0.5f) - center.x);
                    float factor = 0.5f + 0.5f * std::sin(midAngle - timeSec);

                    Color segCol = Color::Lerp(colorA, colorB, factor);
                    u32 glowCol = segCol.WithAlpha(segCol.a * alphaScale).ToU32();

                    draw->AddLine(p1, p2, glowCol, thickness + expand * 2.0f);
                }
            }
        }

        // 2. Razor-sharp core rotating line
        for (size_t i = 0; i < n; ++i) {
            size_t next = (i + 1) % n;
            ImVec2 p1 = path[i];
            ImVec2 p2 = path[next];

            float midAngle = std::atan2(((p1.y + p2.y) * 0.5f) - center.y, ((p1.x + p2.x) * 0.5f) - center.x);
            float factor = 0.5f + 0.5f * std::sin(midAngle - timeSec);

            Color segCol = Color::Lerp(colorA, colorB, factor);
            draw->AddLine(p1, p2, segCol.ToU32(), thickness);
        }
    }

} // namespace Solar::FX
