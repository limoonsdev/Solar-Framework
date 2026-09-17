#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>

namespace Solar::UI {

    /**
     * @brief Selectable custom cursor styles
     */
    enum class CursorStyle {
        CyberArrow = 0,     // High-tech luxury chevron pointer that morphs into reticle on hover
        CrosshairDot,       // 4-axis tactical esports crosshair with center dot
        CyberDot,           // Clean glowing laser dot with halo
        PrecisionTriangle,  // Inverted precision chevron with glowing tip
        MinimalRing         // Minimalist glowing neon ring with contraction physics
    };

    class CustomCursor {
    public:
        static CustomCursor& Get();

        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        void SetStyle(CursorStyle style) { m_style = style; }
        CursorStyle GetStyle() const { return m_style; }

        void SetClickRipple(bool enabled) { m_enableRipple = enabled; }
        void SetTrailing(bool enabled) { m_enableTrailing = enabled; }

        void Render();

    private:
        CustomCursor() = default;

        bool        m_enabled = true;
        CursorStyle m_style = CursorStyle::CyberArrow;
        bool        m_enableRipple = true;
        bool        m_enableTrailing = true;
        ImVec2      m_smoothPos = ImVec2(-100.0f, -100.0f);
        float       m_clickRipple = 0.0f;
        float       m_hoverAnim = 0.0f;
        float       m_clickAnim = 0.0f;
    };

} // namespace Solar::UI
