#include "solar/audio/sound_effects.hpp"
#include "solar/audio/wave_synth.hpp"

namespace Solar::Audio {

    SoundBank& SoundBank::Get() {
        static SoundBank instance;
        return instance;
    }

    SoundBank::SoundBank() {
        PrebakeAll();
    }

    void SoundBank::PrebakeAll() {
        // Crisp tactile mechanical click
        WaveTone clickTone;
        clickTone.frequencyStart = 2400.0f;
        clickTone.frequencyEnd = 800.0f;
        clickTone.durationSeconds = 0.022f;
        clickTone.volume = 0.28f;
        clickTone.type = WaveType::Sine;
        clickTone.attack = 0.002f;
        clickTone.decay = 0.016f;
        m_click = WaveSynth::SynthesizeTone(clickTone);

        // Toggle On (ascending pitch)
        WaveTone tonTone;
        tonTone.frequencyStart = 580.0f;
        tonTone.frequencyEnd = 940.0f;
        tonTone.durationSeconds = 0.045f;
        tonTone.volume = 0.30f;
        tonTone.type = WaveType::Sine;
        tonTone.attack = 0.005f;
        tonTone.decay = 0.035f;
        m_toggleOn = WaveSynth::SynthesizeTone(tonTone);

        // Toggle Off (descending pitch)
        WaveTone toffTone;
        toffTone.frequencyStart = 880.0f;
        toffTone.frequencyEnd = 460.0f;
        toffTone.durationSeconds = 0.040f;
        toffTone.volume = 0.25f;
        toffTone.type = WaveType::Sine;
        toffTone.attack = 0.005f;
        toffTone.decay = 0.030f;
        m_toggleOff = WaveSynth::SynthesizeTone(toffTone);

        // Notification chime (Major triad: C6, E6, G6)
        std::vector<WaveTone> notifChord;
        WaveTone c6{ 1046.5f, 1046.5f, 0.18f, 0.22f, WaveType::Sine, 0.01f, 0.15f };
        WaveTone e6{ 1318.5f, 1318.5f, 0.22f, 0.22f, WaveType::Sine, 0.02f, 0.18f };
        WaveTone g6{ 1567.98f, 1567.98f, 0.28f, 0.25f, WaveType::Sine, 0.03f, 0.22f };
        notifChord.push_back(c6);
        notifChord.push_back(e6);
        notifChord.push_back(g6);
        m_notification = WaveSynth::SynthesizeChord(notifChord);

        // Warning buzz
        WaveTone warnTone;
        warnTone.frequencyStart = 420.0f;
        warnTone.frequencyEnd = 380.0f;
        warnTone.durationSeconds = 0.14f;
        warnTone.volume = 0.32f;
        warnTone.type = WaveType::Triangle;
        warnTone.attack = 0.01f;
        warnTone.decay = 0.12f;
        m_warning = WaveSynth::SynthesizeTone(warnTone);

        // Error chord (dissonant tritones)
        std::vector<WaveTone> errChord;
        WaveTone eb{ 311.13f, 300.0f, 0.20f, 0.35f, WaveType::Square, 0.01f, 0.18f };
        WaveTone a{ 440.0f, 420.0f, 0.20f, 0.30f, WaveType::Sine, 0.01f, 0.18f };
        errChord.push_back(eb);
        errChord.push_back(a);
        m_error = WaveSynth::SynthesizeChord(errChord);

        // Slider tick
        WaveTone tickTone;
        tickTone.frequencyStart = 1800.0f;
        tickTone.frequencyEnd = 1600.0f;
        tickTone.durationSeconds = 0.008f;
        tickTone.volume = 0.15f;
        tickTone.type = WaveType::Sine;
        tickTone.attack = 0.001f;
        tickTone.decay = 0.006f;
        m_sliderTick = WaveSynth::SynthesizeTone(tickTone);

        // Tab switch swoosh
        WaveTone tabTone;
        tabTone.frequencyStart = 720.0f;
        tabTone.frequencyEnd = 1100.0f;
        tabTone.durationSeconds = 0.032f;
        tabTone.volume = 0.22f;
        tabTone.type = WaveType::Sine;
        tabTone.attack = 0.004f;
        tabTone.decay = 0.025f;
        m_tabSwitch = WaveSynth::SynthesizeTone(tabTone);
    }

    const std::vector<u8>& SoundBank::GetWaveData(SfxId id) {
        switch (id) {
        case SfxId::Click: return m_click;
        case SfxId::ToggleOn: return m_toggleOn;
        case SfxId::ToggleOff: return m_toggleOff;
        case SfxId::Notification: return m_notification;
        case SfxId::Warning: return m_warning;
        case SfxId::Error: return m_error;
        case SfxId::SliderTick: return m_sliderTick;
        case SfxId::TabSwitch: return m_tabSwitch;
        }
        return m_click;
    }

} // namespace Solar::Audio
