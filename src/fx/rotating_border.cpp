#include "solar/fx/rotating_border.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

namespace Solar::FX {

    namespace {
        constexpr float PI = 3.14159265358979323846f;
        constexpr float TWO_PI = 2.0f * PI;

        Color HSVtoRGB(float h, float s, float v, float a = 1.0f) {
            h = std::fmod(h, 1.0f);
            if (h < 0.0f) h += 1.0f;
            float r = 0, g = 0, b = 0;
            int i = static_cast<int>(h * 6.0f);
            float f = (h * 6.0f) - i;
            float p = v * (1.0f - s);
            float q = v * (1.0f - s * f);
            float t = v * (1.0f - s * (1.0f - f));
            switch (i % 6) {
                case 0: r = v; g = t; b = p; break;
                case 1: r = q; g = v; b = p; break;
                case 2: r = p; g = v; b = t; break;
                case 3: r = p; g = q; b = v; break;
                case 4: r = t; g = p; b = v; break;
                case 5: r = v; g = p; b = q; break;
            }
            return Color(r, g, b, a);
        }

        Color SampleBorderColor(const RotatingBorderConfig& config, float angle, float timeVal) {
            // Normalize angle to [0, 1)
            float normAngle = (angle + PI) / TWO_PI;
            if (!config.clockwise) normAngle = 1.0f - normAngle;

            float phase = std::fmod(normAngle - timeVal, 1.0f);
            if (phase < 0.0f) phase += 1.0f;

            switch (config.mode) {
                case BorderRotationMode::RainbowSpectrum: {
                    return HSVtoRGB(phase, 0.88f, 1.0f, config.colorA.a);
                }
                case BorderRotationMode::NeonPulse: {
                    // Comet beam with exponential falloff tail
                    float dist = phase; // 0 is head, 1 is tail
                    float intensity = 0.0f;
                    if (dist <= config.trailLength && config.trailLength > 0.001f) {
                        float t = 1.0f - (dist / config.trailLength);
                        intensity = std::pow(t, 2.2f);
                    }
                    Color c = Color::Lerp(config.colorB, config.colorA, intensity);
                    return c.WithAlpha(c.a * (0.15f + 0.85f * intensity));
                }
                case BorderRotationMode::CyberGradient: {
                    // Tri-color cyclic gradient A -> B -> C -> A
                    float p = phase * 3.0f;
                    if (p < 1.0f) {
                        return Color::Lerp(config.colorA, config.colorB, p);
                    } else if (p < 2.0f) {
                        return Color::Lerp(config.colorB, config.colorC, p - 1.0f);
                    } else {
                        return Color::Lerp(config.colorC, config.colorA, p - 2.0f);
                    }
                }
                case BorderRotationMode::DualOrbit: {
                    // Two opposing pulses 180 degrees out of phase
                    float d1 = std::abs(phase - 0.25f);
                    float d2 = std::abs(phase - 0.75f);
                    float d = (std::min)(d1, d2) * 4.0f;
                    float t = std::clamp(1.0f - d, 0.0f, 1.0f);
                    t = std::pow(t, 2.0f);
                    Color c = Color::Lerp(config.colorB, config.colorA, t);
                    return c.WithAlpha(c.a * (0.20f + 0.80f * t));
                }
                case BorderRotationMode::TwoColorLerp:
                default: {
                    float factor = 0.5f + 0.5f * std::sin(angle - timeVal * TWO_PI);
                    return Color::Lerp(config.colorA, config.colorB, factor);
                }
            }
        }
    }

    void RotatingBorder::Draw(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding,
                             const RotatingBorderConfig& config) {
        if (!draw || !config.enabled) return;

        float width = max.x - min.x;
        float height = max.y - min.y;
        if (width <= 4.0f || height <= 4.0f) return;

        float timeVal = static_cast<float>(ImGui::GetTime()) * (config.speed * 0.25f);
        ImVec2 center(min.x + width * 0.5f, min.y + height * 0.5f);

        // Clamp rounding
        rounding = (std::min)(rounding, (std::min)(width, height) * 0.5f);

        // Generate smooth polygonal path along the rounded rect boundary
        const int cornerSegs = 7;
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
            addArc(ImVec2(max.x - rounding, min.y + rounding), -PI * 0.5f, 0.0f);
            addArc(ImVec2(max.x - rounding, max.y - rounding), 0.0f, PI * 0.5f);
            addArc(ImVec2(min.x + rounding, max.y - rounding), PI * 0.5f, PI);
            addArc(ImVec2(min.x + rounding, min.y + rounding), PI, PI * 1.5f);
        } else {
            path = { min, ImVec2(max.x, min.y), max, ImVec2(min.x, max.y) };
        }

        size_t n = path.size();
        if (n < 4) return;

        // 1. Multi-pass luxury Gaussian glow aura
        if (config.glowIntensity > 0.05f) {
            int passes = (std::clamp)(config.glowPasses, 1, 5);
            for (int gp = passes; gp >= 1; --gp) {
                float expand = static_cast<float>(gp) * 1.65f;
                float alphaScale = (0.15f / static_cast<float>(gp)) * config.glowIntensity;

                for (size_t i = 0; i < n; ++i) {
                    size_t next = (i + 1) % n;
                    ImVec2 p1 = path[i];
                    ImVec2 p2 = path[next];

                    float midAngle = std::atan2(((p1.y + p2.y) * 0.5f) - center.y, ((p1.x + p2.x) * 0.5f) - center.x);
                    Color segCol = SampleBorderColor(config, midAngle, timeVal);
                    u32 glowCol = segCol.WithAlpha(segCol.a * alphaScale).ToU32();

                    draw->AddLine(p1, p2, glowCol, config.thickness + expand * 2.0f);
                }
            }
        }

        // 2. Razor-sharp core rotating border line
        for (size_t i = 0; i < n; ++i) {
            size_t next = (i + 1) % n;
            ImVec2 p1 = path[i];
            ImVec2 p2 = path[next];

            float midAngle = std::atan2(((p1.y + p2.y) * 0.5f) - center.y, ((p1.x + p2.x) * 0.5f) - center.x);
            Color segCol = SampleBorderColor(config, midAngle, timeVal);
            draw->AddLine(p1, p2, segCol.ToU32(), config.thickness);
        }
    }

    void RotatingBorder::Draw(ImDrawList* draw, const ImVec2& min, const ImVec2& max, float rounding,
                             Color colorA, Color colorB, float speed, float thickness, float glowIntensity) {
        RotatingBorderConfig cfg;
        cfg.colorA = colorA;
        cfg.colorB = colorB;
        cfg.speed = speed;
        cfg.thickness = thickness;
        cfg.glowIntensity = glowIntensity;
        cfg.mode = BorderRotationMode::TwoColorLerp;
        Draw(draw, min, max, rounding, cfg);
    }

} // namespace Solar::FX
