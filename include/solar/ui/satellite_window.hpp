#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include "solar/fx/rotating_border.hpp"
#include <imgui.h>
#include <string>

namespace Solar::UI {

    /**
     * @brief Anchor positions for satellite / detached floating windows
     */
    enum class SatelliteAnchor {
        FreeFloating = 0,   // Independent free drag anywhere on screen
        DockRight,          // Magnetically anchored to right edge of parent
        DockLeft,           // Magnetically anchored to left edge of parent
        DockTop,            // Magnetically anchored above parent
        DockBottom,         // Magnetically anchored below parent
        DockTopRight,       // Top-right corner attachment
        DockBottomRight     // Bottom-right corner attachment
    };

    /**
     * @brief Comprehensive configuration for satellite / floating dock elements
     */
    struct SatelliteConfig {
        SatelliteAnchor anchor = SatelliteAnchor::DockRight;
        float offsetGap = 14.0f;            // Distance in pixels between parent and satellite
        bool followParent = true;           // Automatically follows parent when dragged
        bool magneticSnap = true;          // Snaps to parent dock when dragged close
        float snapThreshold = 38.0f;       // Snapping radius in pixels
        bool smoothSpring = true;          // Subtle spring lag physics during movement
        bool drawConnectorBeam = true;     // Glowing luxury neon connector bracket to parent
        Color connectorColor = Color(0.22f, 0.58f, 1.0f, 0.65f);
        bool enableRotatingBorder = false; // Rotating gradient border
        FX::RotatingBorderConfig borderConfig;
        bool isPinned = true;              // Pin status (can be toggled in titlebar)
        ImVec2 currentPos = ImVec2(-1, -1);
        ImVec2 velocity = ImVec2(0, 0);
    };

    /**
     * @brief Satellite / Detached-Attached Floating Window System
     * Creates auxiliary floating windows visually separated from the main GUI window,
     * but magnetically attached, tracking its position with physics, neon connectors,
     * and independent glassmorphic cards.
     */
    class SatelliteWindow {
    public:
        /**
         * @brief Begins a satellite window bound to a parent window
         * @param name Unique window identifier
         * @param title Display title in header
         * @param size Window dimensions
         * @param config Satellite behavior and anchor configuration
         * @param p_open Optional pointer to boolean visibility
         * @param parentWindowName Identifier of the parent window to anchor to
         * @return true if window is visible and content should be rendered
         */
        static bool Begin(const char* name, const char* title, const ImVec2& size,
                          SatelliteConfig& config, bool* p_open = nullptr,
                          const char* parentWindowName = "##MainWindow");

        /**
         * @brief Ends the satellite window
         */
        static void End();
    };

    inline bool BeginSatellite(const char* name, const char* title, const ImVec2& size,
                               SatelliteConfig& config, bool* p_open = nullptr,
                               const char* parentWindowName = "##MainWindow") {
        return SatelliteWindow::Begin(name, title, size, config, p_open, parentWindowName);
    }

    inline void EndSatellite() {
        SatelliteWindow::End();
    }

} // namespace Solar::UI
