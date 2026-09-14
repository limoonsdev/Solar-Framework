#pragma once
#include <string>

namespace Solar::UI {

    /**
     * @brief Luxury Animated Welcome Screen
     * Displays on first launch or via developer toggle.
     * Features rotating chromatic borders, user entitlement status,
     * hardware signature verification badges, and fluid spring animations.
     */
    class WelcomeScreen {
    public:
        static WelcomeScreen& Get();

        void Show() { m_visible = true; }
        void Hide() { m_visible = false; }
        bool IsVisible() const { return m_visible; }

        // Returns true while the welcome modal is active
        bool Render();

    private:
        WelcomeScreen() = default;

        bool m_visible = false;
        float m_alpha = 0.0f;
    };

} // namespace Solar::UI
