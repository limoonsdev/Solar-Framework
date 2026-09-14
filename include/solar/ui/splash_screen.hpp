#pragma once
#include "solar/core/types.hpp"
#include <string>
#include <vector>

namespace Solar::UI {

    enum class SplashStage {
        CoreInit = 0,
        DirectXInit,
        FontRasterization,
        AudioSynthesis,
        ComputeParticles,
        Ready,
        Count
    };

    struct StageTelemetry {
        SplashStage stage;
        const char* name;
        const char* detail;
        float progressThreshold;
    };

    class SplashScreen {
    public:
        static SplashScreen& Get();

        void Start(float durationSeconds = 2.5f);
        bool Render();
        bool IsFinished() const { return m_finished; }
        SplashStage GetCurrentStage() const { return m_currentStage; }

    private:
        SplashScreen() = default;
        float m_duration = 2.5f;
        float m_elapsed = 0.0f;
        bool m_active = true;
        bool m_finished = false;
        float m_alpha = 1.0f;
        SplashStage m_currentStage = SplashStage::CoreInit;
    };

} // namespace Solar::UI
