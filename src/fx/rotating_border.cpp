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

        Color SampleBorderColor(const RotatingBorderConfig& config, float s, float timeVal) {
            // s in [0, 1) represents exact normalized distance along entire perimeter
            float normPos = s;
            if (!config.clockwise) normPos = 1.0f - normPos;

            float phase = std::fmod(normPos - timeVal, 1.0f);
            if (phase < 0.0f) phase += 1.0f;

            switch (config.mode) {
                case BorderRotationMode::RainbowSpectrum: {
                    return HSVtoRGB(phase, 0.90f, 1.0f, config.colorA.a);
                }
                case BorderRotationMode::NeonPulse: {
                    // Comet beam with exponential falloff tail along perimeter
                    float dist = phase; // 0 is comet head, moving to tail
                    float intensity = 0.0f;
                    float trail = (std::max)(config.trailLength, 0.05f);
                    if (dist <= trail) {
                        float t = 1.0f - (dist / trail);
                        intensity = std::pow(t, 2.0f);
                    }
                    Color c = Color::Lerp(config.colorB, config.colorA, intensity);
                    // Ensure baseline visibility so border is always visible around all 4 sides
                    return c.WithAlpha(c.a * (0.22f + 0.78f * intensity));
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
                    // Two opposing light beams 180 degrees out of phase
                    float d1 = std::abs(phase - 0.25f);
                    float d2 = std::abs(phase - 0.75f);
                    float d = (std::min)(d1, d2) * 4.0f;
                    float t = std::clamp(1.0f - d, 0.0f, 1.0f);
                    t = std::pow(t, 2.0f);
                    Color c = Color::Lerp(config.colorB, config.colorA, t);
                    return c.WithAlpha(c.a * (0.25f + 0.75f * t));
                }
                case BorderRotationMode::TwoColorLerp:
                default: {
                    float factor = 0.5f + 0.5f * std::sin(phase * TWO_PI);
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

        float timeVal = static_cast<float>(ImGui::GetTime()) * (config.speed * 0.22f);

        // Clamp rounding
        rounding = (std::clamp)(rounding, 0.0f, (std::min)(width, height) * 0.5f);

        // Subdivide straight edges so that colors vary smoothly across long sides
        int hSubdiv = (std::clamp)(static_cast<int>((width - 2.0f * rounding) / 32.0f), 1, 30);
        int vSubdiv = (std::clamp)(static_cast<int>((height - 2.0f * rounding) / 32.0f), 1, 20);
        const int cornerSegs = 6;

        std::vector<ImVec2> rawPts;
        rawPts.reserve(cornerSegs * 4 + hSubdiv * 2 + vSubdiv * 2 + 10);

        // 1. Top Edge: Left to Right
        for (int i = 0; i <= hSubdiv; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(hSubdiv);
            rawPts.emplace_back(min.x + rounding + t * (width - 2.0f * rounding), min.y);
        }

        // 2. Top-Right Corner Arc (-PI/2 to 0)
        ImVec2 trCenter(max.x - rounding, min.y + rounding);
        for (int i = 1; i <= cornerSegs; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(cornerSegs);
            float a = -PI * 0.5f + t * (PI * 0.5f);
            rawPts.emplace_back(trCenter.x + std::cos(a) * rounding, trCenter.y + std::sin(a) * rounding);
        }

        // 3. Right Edge: Top to Bottom
        for (int i = 1; i <= vSubdiv; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(vSubdiv);
            rawPts.emplace_back(max.x, min.y + rounding + t * (height - 2.0f * rounding));
        }

        // 4. Bottom-Right Corner Arc (0 to PI/2)
        ImVec2 brCenter(max.x - rounding, max.y - rounding);
        for (int i = 1; i <= cornerSegs; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(cornerSegs);
            float a = t * (PI * 0.5f);
            rawPts.emplace_back(brCenter.x + std::cos(a) * rounding, brCenter.y + std::sin(a) * rounding);
        }

        // 5. Bottom Edge: Right to Left
        for (int i = 1; i <= hSubdiv; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(hSubdiv);
            rawPts.emplace_back(max.x - rounding - t * (width - 2.0f * rounding), max.y);
        }

        // 6. Bottom-Left Corner Arc (PI/2 to PI)
        ImVec2 blCenter(min.x + rounding, max.y - rounding);
        for (int i = 1; i <= cornerSegs; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(cornerSegs);
            float a = PI * 0.5f + t * (PI * 0.5f);
            rawPts.emplace_back(blCenter.x + std::cos(a) * rounding, blCenter.y + std::sin(a) * rounding);
        }

        // 7. Left Edge: Bottom to Top
        for (int i = 1; i <= vSubdiv; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(vSubdiv);
            rawPts.emplace_back(min.x, max.y - rounding - t * (height - 2.0f * rounding));
        }

        // 8. Top-Left Corner Arc (PI to 3*PI/2)
        ImVec2 tlCenter(min.x + rounding, min.y + rounding);
        for (int i = 1; i < cornerSegs; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(cornerSegs);
            float a = PI + t * (PI * 0.5f);
            rawPts.emplace_back(tlCenter.x + std::cos(a) * rounding, tlCenter.y + std::sin(a) * rounding);
        }

        size_t n = rawPts.size();
        if (n < 4) return;

        // Compute cumulative arc length along perimeter
        std::vector<float> dists(n, 0.0f);
        float totalLen = 0.0f;
        for (size_t i = 0; i < n; ++i) {
            size_t next = (i + 1) % n;
            float dx = rawPts[next].x - rawPts[i].x;
            float dy = rawPts[next].y - rawPts[i].y;
            totalLen += std::sqrt(dx * dx + dy * dy);
            dists[next] = totalLen;
        }

        if (totalLen <= 0.001f) return;

        // 1. Multi-pass luxury Gaussian glow aura
        if (config.glowIntensity > 0.05f) {
            int passes = (std::clamp)(config.glowPasses, 1, 4);
            for (int gp = passes; gp >= 1; --gp) {
                float expand = static_cast<float>(gp) * 1.5f;
                float alphaScale = (0.16f / static_cast<float>(gp)) * config.glowIntensity;

                for (size_t i = 0; i < n; ++i) {
                    size_t next = (i + 1) % n;
                    float sMid = (dists[i] + (dists[next] > dists[i] ? dists[next] : dists[i] + 1.0f) * 0.5f) / totalLen;
                    Color segCol = SampleBorderColor(config, sMid, timeVal);
                    u32 glowCol = segCol.WithAlpha(segCol.a * alphaScale).ToU32();
                    draw->AddLine(rawPts[i], rawPts[next], glowCol, config.thickness + expand * 2.0f);
                }
            }
        }

        // 2. Razor-sharp core rotating border line completely closed around all 4 sides
        for (size_t i = 0; i < n; ++i) {
            size_t next = (i + 1) % n;
            float sMid = (dists[i] + (dists[next] > dists[i] ? dists[next] : dists[i] + 1.0f) * 0.5f) / totalLen;
            Color segCol = SampleBorderColor(config, sMid, timeVal);
            draw->AddLine(rawPts[i], rawPts[next], segCol.ToU32(), config.thickness);
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
