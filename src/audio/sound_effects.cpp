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
        // Creamy tactile mechanical switch click (lubed linear bottom-out pop)
        WaveTone clickTone;
        clickTone.frequencyStart = 440.0f;
        clickTone.frequencyEnd = 240.0f;
        clickTone.durationSeconds = 0.024f;
        clickTone.volume = 0.22f;
        clickTone.type = WaveType::Sine;
        clickTone.attack = 0.002f;
        clickTone.decay = 0.018f;
        m_click = WaveSynth::SynthesizeTone(clickTone);

        // Toggle On (warm rising switch slide)
        WaveTone tonTone;
        tonTone.frequencyStart = 340.0f;
        tonTone.frequencyEnd = 520.0f;
        tonTone.durationSeconds = 0.035f;
        tonTone.volume = 0.22f;
        tonTone.type = WaveType::Sine;
        tonTone.attack = 0.003f;
        tonTone.decay = 0.028f;
        m_toggleOn = WaveSynth::SynthesizeTone(tonTone);

        // Toggle Off (muted warm bottom-out thock)
        WaveTone toffTone;
        toffTone.frequencyStart = 480.0f;
        toffTone.frequencyEnd = 260.0f;
        toffTone.durationSeconds = 0.032f;
        toffTone.volume = 0.20f;
        toffTone.type = WaveType::Sine;
        toffTone.attack = 0.002f;
        toffTone.decay = 0.026f;
        m_toggleOff = WaveSynth::SynthesizeTone(toffTone);

        // Notification chime (Warm luxurious harmonic triad: Ab4, C5, Eb5)
        std::vector<WaveTone> notifChord;
        WaveTone ab4{ 415.30f, 415.30f, 0.22f, 0.18f, WaveType::Sine, 0.015f, 0.18f };
        WaveTone c5 { 523.25f, 523.25f, 0.26f, 0.18f, WaveType::Sine, 0.020f, 0.20f };
        WaveTone eb5{ 622.25f, 622.25f, 0.32f, 0.20f, WaveType::Sine, 0.025f, 0.24f };
        notifChord.push_back(ab4);
        notifChord.push_back(c5);
        notifChord.push_back(eb5);
        m_notification = WaveSynth::SynthesizeChord(notifChord);

        // Warning chime (soft rounded double low pulse)
        WaveTone warnTone;
        warnTone.frequencyStart = 280.0f;
        warnTone.frequencyEnd = 240.0f;
        warnTone.durationSeconds = 0.10f;
        warnTone.volume = 0.24f;
        warnTone.type = WaveType::Sine;
        warnTone.attack = 0.01f;
        warnTone.decay = 0.08f;
        m_warning = WaveSynth::SynthesizeTone(warnTone);

        // Error chord (subtle velvet low tension)
        std::vector<WaveTone> errChord;
        WaveTone f3 { 174.61f, 165.0f, 0.14f, 0.24f, WaveType::Triangle, 0.01f, 0.11f };
        WaveTone b3 { 246.94f, 235.0f, 0.14f, 0.20f, WaveType::Sine,     0.01f, 0.11f };
        errChord.push_back(f3);
        errChord.push_back(b3);
        m_error = WaveSynth::SynthesizeChord(errChord);

        // Slider creamy micro-tick (tactile buttery tap, zero harsh treble)
        WaveTone tickTone;
        tickTone.frequencyStart = 380.0f;
        tickTone.frequencyEnd = 320.0f;
        tickTone.durationSeconds = 0.007f;
        tickTone.volume = 0.10f;
        tickTone.type = WaveType::Sine;
        tickTone.attack = 0.001f;
        tickTone.decay = 0.005f;
        m_sliderTick = WaveSynth::SynthesizeTone(tickTone);

        // Tab switch creamy glide
        WaveTone tabTone;
        tabTone.frequencyStart = 320.0f;
        tabTone.frequencyEnd = 460.0f;
        tabTone.durationSeconds = 0.028f;
        tabTone.volume = 0.18f;
        tabTone.type = WaveType::Sine;
        tabTone.attack = 0.003f;
        tabTone.decay = 0.022f;
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
