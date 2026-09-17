#include "solar/game/fov_renderer.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/glow_engine.hpp"
#include "solar/widgets/toggle.hpp"
#include "solar/widgets/slider.hpp"
#include "solar/widgets/color_picker.hpp"
#include "solar/widgets/separator.hpp"
#include <imgui_internal.h>
#include <cmath>
#include <cstdio>

namespace Solar::Game {

    void FOVRenderer::DrawFOV(ImDrawList* drawList, const ImVec2& center, const FOVSettings& settings, float animTime) {
        if (!settings.enabled || !drawList) return;

        float pulse = 0.0f;
        if (settings.dynamicPulse) {
            pulse = std::sin(animTime * settings.pulseSpeed) * settings.pulseAmplitude;
        }

        float r = std::max(4.0f, settings.radius + pulse);
        ImVec4 activeCol = settings.isTargetLocked ? settings.lockColor : settings.color;
        u32 col32 = ImGui::ColorConvertFloat4ToU32(activeCol);

        // 1. Subtle Inner Radial Gradient Disk
        if (settings.enableRadialGradient && settings.innerOpacity > 0.001f) {
            int innerSteps = 6;
            for (int i = innerSteps; i >= 1; --i) {
                float stepR = r * (static_cast<float>(i) / innerSteps);
                float stepAlpha = activeCol.w * settings.innerOpacity * (static_cast<float>(i) / innerSteps);
                u32 stepCol = ImGui::ColorConvertFloat4ToU32(ImVec4(activeCol.x, activeCol.y, activeCol.z, stepAlpha));
                drawList->AddCircleFilled(center, stepR, stepCol, settings.segments);
            }
        }

        // 2. Multi-pass Gaussian Glow Bloom Ring
        if (settings.enableGlow && settings.glowIntensity > 0.01f) {
            int glowPasses = 4;
            for (int i = 1; i <= glowPasses; ++i) {
                float spread = (settings.glowRadius / glowPasses) * i;
                float glowAlpha = (activeCol.w * settings.glowIntensity * 0.25f) / i;
                u32 glowCol = ImGui::ColorConvertFloat4ToU32(ImVec4(activeCol.x, activeCol.y, activeCol.z, glowAlpha));
                drawList->AddCircle(center, r + spread, glowCol, settings.segments, settings.thickness + spread * 0.8f);
                if (r > spread) {
                    drawList->AddCircle(center, r - spread, glowCol, settings.segments, settings.thickness + spread * 0.8f);
                }
            }
        }

        // 3. Primary Antialiased Perimeter Ring
        drawList->AddCircle(center, r, col32, settings.segments, settings.thickness);

        // 4. Tactical Cyber Cardinal & Diagonal Tick Marks
        if (settings.enableCyberMarks) {
            float angles[8] = { 0.0f, 45.0f, 90.0f, 135.0f, 180.0f, 225.0f, 270.0f, 315.0f };
            float degToRad = 3.14159265f / 180.0f;

            for (int i = 0; i < 8; ++i) {
                bool isCardinal = (i % 2 == 0);
                float len = isCardinal ? settings.markLength : (settings.markLength * 0.55f);
                float rad = angles[i] * degToRad;
                float cosA = std::cos(rad);
                float sinA = std::sin(rad);

                ImVec2 p1(center.x + (r - 2.0f) * cosA, center.y + (r - 2.0f) * sinA);
                ImVec2 p2(center.x + (r + len) * cosA, center.y + (r + len) * sinA);

                u32 tickCol = isCardinal ? col32 : ImGui::ColorConvertFloat4ToU32(ImVec4(activeCol.x, activeCol.y, activeCol.z, activeCol.w * 0.6f));
                drawList->AddLine(p1, p2, tickCol, isCardinal ? (settings.thickness + 0.5f) : settings.thickness);
            }

            // Degree / Radius Tag
            if (settings.enableDegreeText) {
                char tagBuf[32];
                if (settings.isTargetLocked) {
                    snprintf(tagBuf, sizeof(tagBuf), "LOCK [%.1fm]", settings.targetDistance);
                } else {
                    snprintf(tagBuf, sizeof(tagBuf), "FOV %.0fpx", settings.radius);
                }
                ImVec2 textSize = ImGui::CalcTextSize(tagBuf);
                ImVec2 textPos(center.x - textSize.x * 0.5f, center.y + r + settings.markLength + 4.0f);
                
                // Text background capsule
                drawList->AddRectFilled(
                    ImVec2(textPos.x - 6.0f, textPos.y - 2.0f),
                    ImVec2(textPos.x + textSize.x + 6.0f, textPos.y + textSize.y + 2.0f),
                    IM_COL32(10, 13, 18, 200), 4.0f
                );
                drawList->AddRect(
                    ImVec2(textPos.x - 6.0f, textPos.y - 2.0f),
                    ImVec2(textPos.x + textSize.x + 6.0f, textPos.y + textSize.y + 2.0f),
                    col32, 4.0f, 0, 1.0f
                );
                drawList->AddText(textPos, col32, tagBuf);
            }
        }

        // 5. Precision Center Crosshair Dot
        if (settings.enableCenterDot) {
            drawList->AddCircleFilled(center, settings.dotRadius + 1.0f, IM_COL32(0, 0, 0, 160), 16);
            drawList->AddCircleFilled(center, settings.dotRadius, col32, 16);
        }
    }

    void FOVRenderer::RenderPreview(const char* str_id, const ImVec2& size, FOVSettings& settings) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 avail = ImGui::GetContentRegionAvail();
        float w = (size.x > 0.0f) ? size.x : avail.x;
        float h = (size.y > 0.0f) ? size.y : 320.0f;

        ImGui::BeginChild(str_id, ImVec2(w, h), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 childMin = ImGui::GetCursorScreenPos();
        ImVec2 childMax = ImVec2(childMin.x + w, childMin.y + h);
        ImVec2 center = ImVec2(childMin.x + w * 0.5f, childMin.y + h * 0.5f);

        // Dark Canvas Background
        draw->AddRectFilled(childMin, childMax, IM_COL32(10, 13, 18, 255), 8.0f);

        // Tactical Grid
        float gridSize = 24.0f;
        u32 gridCol = IM_COL32(255, 255, 255, 12);
        for (float gx = childMin.x; gx <= childMax.x; gx += gridSize) {
            draw->AddLine(ImVec2(gx, childMin.y), ImVec2(gx, childMax.y), gridCol, 1.0f);
        }
        for (float gy = childMin.y; gy <= childMax.y; gy += gridSize) {
            draw->AddLine(ImVec2(childMin.x, gy), ImVec2(childMax.x, gy), gridCol, 1.0f);
        }

        // Subtly simulated enemy target blip
        float t = static_cast<float>(ImGui::GetTime());
        float dummyDist = settings.isTargetLocked ? 35.0f : 85.0f;
        ImVec2 targetPos(center.x + std::cos(t * 0.8f) * dummyDist, center.y + std::sin(t * 0.8f) * dummyDist * 0.6f);

        // Target diamond
        u32 dummyCol = settings.isTargetLocked ? IM_COL32(255, 75, 75, 230) : IM_COL32(120, 160, 200, 180);
        float dSize = 6.0f;
        draw->AddQuadFilled(
            ImVec2(targetPos.x, targetPos.y - dSize),
            ImVec2(targetPos.x + dSize, targetPos.y),
            ImVec2(targetPos.x, targetPos.y + dSize),
            ImVec2(targetPos.x - dSize, targetPos.y),
            dummyCol
        );
        draw->AddText(ImVec2(targetPos.x + 8.0f, targetPos.y - 6.0f), dummyCol, settings.isTargetLocked ? "LOCKED" : "Target");

        // FOV Circle render
        DrawFOV(draw, center, settings, t);

        // Border & specular edge
        Render::ImGuiExt::AddSmoothBorder(draw, childMin, childMax, ThemeManager::Get().GetPalette().Border.ToU32(), 8.0f, 1.0f);

        // Interactive click on canvas to toggle target acquisition
        ImRect clickBB(childMin, childMax);
        if (ImGui::ItemAdd(clickBB, ImGui::GetID("##fov_click_area"))) {
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                settings.isTargetLocked = !settings.isTargetLocked;
            }
        }

        ImGui::EndChild();
    }

    void FOVRenderer::RenderControls(FOVSettings& settings) {
        Widgets::Toggle("Enable Dynamic FOV Circle", &settings.enabled, "Renders vector field of view ring");
        Widgets::Separator();

        Widgets::SliderFloat("FOV Radius (px)", &settings.radius, 10.0f, 380.0f, "%.0f", "px");
        Widgets::SliderFloat("Line Thickness", &settings.thickness, 0.5f, 5.0f, "%.1f", "px");
        Widgets::SliderInt("Circle Smoothness", &settings.segments, 24, 128, "%d seg");

        Widgets::Separator();
        Widgets::Toggle("Outer Glow Bloom", &settings.enableGlow, "Multi-pass soft radiant bloom");
        if (settings.enableGlow) {
            Widgets::SliderFloat("Glow Intensity", &settings.glowIntensity, 0.05f, 1.0f, "%.2f");
            Widgets::SliderFloat("Glow Radius", &settings.glowRadius, 4.0f, 30.0f, "%.0f", "px");
        }

        Widgets::Toggle("Inner Radial Gradient", &settings.enableRadialGradient, "Soft colored volumetric falloff");
        if (settings.enableRadialGradient) {
            Widgets::SliderFloat("Inner Opacity", &settings.innerOpacity, 0.01f, 0.40f, "%.2f");
        }

        Widgets::Separator();
        Widgets::Toggle("Dynamic Breathing Pulse", &settings.dynamicPulse, "Animated rhythmic sine-wave expansion");
        if (settings.dynamicPulse) {
            Widgets::SliderFloat("Pulse Speed", &settings.pulseSpeed, 0.5f, 6.0f, "%.1f", "x");
            Widgets::SliderFloat("Pulse Amplitude", &settings.pulseAmplitude, 1.0f, 15.0f, "%.1f", "px");
        }

        Widgets::Toggle("Cyber Reticle Tick Notches", &settings.enableCyberMarks, "Cardinal and diagonal precision notches");
        Widgets::Toggle("Display FOV / Lock Degree Text", &settings.enableDegreeText);
        Widgets::Toggle("Center Precision Dot", &settings.enableCenterDot);

        Widgets::Separator();
        float c1[4] = { settings.color.x, settings.color.y, settings.color.z, settings.color.w };
        if (Widgets::ColorPicker("Perimeter Color", c1)) {
            settings.color = ImVec4(c1[0], c1[1], c1[2], c1[3]);
        }
        float c2[4] = { settings.lockColor.x, settings.lockColor.y, settings.lockColor.z, settings.lockColor.w };
        if (Widgets::ColorPicker("Target Locked Color", c2)) {
            settings.lockColor = ImVec4(c2[0], c2[1], c2[2], c2[3]);
        }
        Widgets::Toggle("Simulate Target Lock", &settings.isTargetLocked, "Interactive test state for lock-on");
    }

} // namespace Solar::Game
