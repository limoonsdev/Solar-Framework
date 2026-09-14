#pragma once
#include "sound_effects.hpp"

namespace Solar::Audio {

    class AudioEngine {
    public:
        static AudioEngine& Get();

        void Play(SfxId id);
        void PlayClick() { Play(SfxId::Click); }
        void PlayToggle(bool on) { Play(on ? SfxId::ToggleOn : SfxId::ToggleOff); }
        void PlayNotification() { Play(SfxId::Notification); }
        void PlayWarning() { Play(SfxId::Warning); }
        void PlayError() { Play(SfxId::Error); }
        void PlaySliderTick() { Play(SfxId::SliderTick); }
        void PlayTabSwitch() { Play(SfxId::TabSwitch); }

        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        void SetVolume(f32 volume) { m_volume = volume; }
        f32 GetVolume() const { return m_volume; }

    private:
        AudioEngine() = default;
        bool m_enabled = true;
        f32 m_volume = 1.0f;
    };

    // Quick helpers
    inline void PlayClick() { AudioEngine::Get().PlayClick(); }
    inline void PlayToggle(bool on) { AudioEngine::Get().PlayToggle(on); }
    inline void PlayNotification() { AudioEngine::Get().PlayNotification(); }
    inline void PlayWarning() { AudioEngine::Get().PlayWarning(); }
    inline void PlayError() { AudioEngine::Get().PlayError(); }
    inline void PlaySliderTick() { AudioEngine::Get().PlaySliderTick(); }
    inline void PlayTabSwitch() { AudioEngine::Get().PlayTabSwitch(); }

} // namespace Solar::Audio
