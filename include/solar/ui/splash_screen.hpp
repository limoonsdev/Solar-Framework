#pragma once
#include "solar/core/types.hpp"
#include <string>

namespace Solar::UI {

    class SplashScreen {
    public:
        static SplashScreen& Get();

        void Start(float durationSeconds = 2.8f);
        bool Render();
        bool IsFinished() const { return m_finished; }

    private:
        SplashScreen() = default;
        float m_duration = 2.8f;
        float m_elapsed = 0.0f;
        bool m_active = true;
        bool m_finished = false;
        float m_alpha = 1.0f;
    };

} // namespace Solar::UI
