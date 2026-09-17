#include "solar/solar.hpp"
#include <iostream>

namespace Solar {

    static FrameworkMode s_frameworkMode = FrameworkMode::External;

    void SetMode(FrameworkMode mode) {
        s_frameworkMode = mode;
    }

    FrameworkMode GetMode() {
        return s_frameworkMode;
    }

    void Initialize() {
        // Initialize Theme System & Palette
        ThemeManager::Get().Initialize();

        // Initialize Background Particle Dynamics
        FX::ParticleSystem::Get().Initialize(ThemeManager::Get().GetStyle().ParticleCount);

        // Prebake Audio Procedural Tones
        Audio::SoundBank::Get();

        // Welcome Notification
        Notify::Success("Solar Framework v1.0.1", "Industrial modular engine & procedural audio ready.");
    }

    void NewFrame() {
        // Update High-Resolution Frame Timing
        FrameTimeTracker::Get().Update();

        // Render Active Floating Notification Stack
        NotificationSystem::Get().Render();
    }

    void Shutdown() {
        Anim::AnimationManager::Get().Reset();
    }

} // namespace Solar
