#pragma once
#include "solar/core/types.hpp"
#include <vector>

namespace Solar::Audio {

    enum class WaveType {
        Sine = 0,
        Triangle,
        Square,
        Saw,
        Noise
    };

    struct WaveTone {
        f32 frequencyStart = 440.0f;
        f32 frequencyEnd = 440.0f;
        f32 durationSeconds = 0.05f;
        f32 volume = 0.5f;
        WaveType type = WaveType::Sine;
        f32 attack = 0.005f;
        f32 decay = 0.02f;
    };

    class WaveSynth {
    public:
        // Generates a self-contained RIFF WAVE 16-bit 44.1kHz mono byte buffer in memory
        static std::vector<u8> SynthesizeTone(const WaveTone& tone);
        static std::vector<u8> SynthesizeChord(const std::vector<WaveTone>& tones);
    };

} // namespace Solar::Audio
