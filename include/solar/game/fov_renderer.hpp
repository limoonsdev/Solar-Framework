#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>
#include <string>

namespace Solar::Game {

    struct FOVSettings {
        bool enabled = true;
        float radius = 120.0f;          // Radius in pixels
        float thickness = 1.8f;         // Border line thickness
        int segments = 80;              // Circle smoothness
        ImVec4 color = ImVec4(0.22f, 0.65f, 1.00f, 0.90f); // Primary accent color
        ImVec4 lockColor = ImVec4(1.00f, 0.28f, 0.28f, 0.95f); // Target locked color
        
        bool enableGlow = true;         // Outer/inner ambient glow
        float glowRadius = 14.0f;
        float glowIntensity = 0.40f;
        
        bool enableRadialGradient = true;// Subtle soft radial gradient inside the circle
        float innerOpacity = 0.08f;

        bool dynamicPulse = true;       // Breathing / pulsing animation
        float pulseSpeed = 2.4f;
        float pulseAmplitude = 4.0f;

        bool enableCyberMarks = true;   // Tactical cyber tick notches at cardinal angles
        float markLength = 8.0f;
        bool enableDegreeText = true;   // Display current FOV degrees / radius

        bool enableCenterDot = true;    // Precision center reticle dot
        float dotRadius = 2.2f;

        bool isTargetLocked = false;    // Target acquired state (simulated or live)
        float targetDistance = 42.5f;   // Distance in meters
    };

    class FOVRenderer {
    public:
        // Draw the FOV reticle onto an ImDrawList at a specific center position
        static void DrawFOV(ImDrawList* drawList, const ImVec2& center, const FOVSettings& settings, float animTime = 0.0f);

        // Render an interactive preview card with simulated crosshairs and targets
        static void RenderPreview(const char* str_id, const ImVec2& size, FOVSettings& settings);

        // Render full UI control settings panel for configuring the FOV engine
        static void RenderControls(FOVSettings& settings);
    };

} // namespace Solar::Game
