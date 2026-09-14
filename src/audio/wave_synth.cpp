#include "solar/audio/wave_synth.hpp"
#include "solar/core/math.hpp"
#include <cmath>
#include <cstring>
#include <random>

namespace Solar::Audio {

    #pragma pack(push, 1)
    struct WaveHeader {
        char chunkId[4] = {'R', 'I', 'F', 'F'};
        u32 chunkSize = 0;
        char format[4] = {'W', 'A', 'V', 'E'};
        char subchunk1Id[4] = {'f', 'm', 't', ' '};
        u32 subchunk1Size = 16;
        u16 audioFormat = 1; // PCM
        u16 numChannels = 1; // Mono
        u32 sampleRate = 44100;
        u32 byteRate = 44100 * 2;
        u16 blockAlign = 2;
        u16 bitsPerSample = 16;
        char subchunk2Id[4] = {'d', 'a', 't', 'a'};
        u32 subchunk2Size = 0;
    };
    #pragma pack(pop)

    std::vector<u8> WaveSynth::SynthesizeTone(const WaveTone& tone) {
        const u32 sampleRate = 44100;
        u32 totalSamples = static_cast<u32>(tone.durationSeconds * sampleRate);
        if (totalSamples == 0) totalSamples = 1;

        u32 dataBytes = totalSamples * sizeof(i16);
        std::vector<u8> buffer(sizeof(WaveHeader) + dataBytes);

        WaveHeader header;
        header.chunkSize = 36 + dataBytes;
        header.subchunk2Size = dataBytes;
        std::memcpy(buffer.data(), &header, sizeof(WaveHeader));

        i16* samples = reinterpret_cast<i16*>(buffer.data() + sizeof(WaveHeader));
        f32 phase = 0.0f;
        std::mt19937 rng(1337);
        std::uniform_real_distribution<f32> noiseDist(-1.0f, 1.0f);

        for (u32 i = 0; i < totalSamples; i++) {
            f32 t = static_cast<f32>(i) / sampleRate;
            f32 progress = static_cast<f32>(i) / totalSamples;
            f32 currentFreq = Math::Lerp(tone.frequencyStart, tone.frequencyEnd, progress);

            // Envelope (Attack - Sustain - Decay)
            f32 env = 1.0f;
            if (t < tone.attack && tone.attack > 0.0f) {
                env = t / tone.attack;
            } else if (t > (tone.durationSeconds - tone.decay) && tone.decay > 0.0f) {
                env = (tone.durationSeconds - t) / tone.decay;
                if (env < 0.0f) env = 0.0f;
            }

            f32 sampleVal = 0.0f;
            switch (tone.type) {
            case WaveType::Sine:
                sampleVal = std::sin(phase);
                break;
            case WaveType::Triangle:
                sampleVal = 2.0f * std::abs(2.0f * (phase / Math::TWO_PI - std::floor(phase / Math::TWO_PI + 0.5f))) - 1.0f;
                break;
            case WaveType::Square:
                sampleVal = std::sin(phase) >= 0.0f ? 1.0f : -1.0f;
                break;
            case WaveType::Saw:
                sampleVal = 2.0f * (phase / Math::TWO_PI - std::floor(phase / Math::TWO_PI + 0.5f));
                break;
            case WaveType::Noise:
                sampleVal = noiseDist(rng);
                break;
            }

            phase += Math::TWO_PI * currentFreq / sampleRate;
            if (phase > Math::TWO_PI) phase -= Math::TWO_PI;

            f32 finalVal = sampleVal * env * tone.volume;
            finalVal = Math::Clamp(finalVal, -1.0f, 1.0f);
            samples[i] = static_cast<i16>(finalVal * 32767.0f);
        }

        return buffer;
    }

    std::vector<u8> WaveSynth::SynthesizeChord(const std::vector<WaveTone>& tones) {
        if (tones.empty()) return {};
        f32 maxDuration = 0.0f;
        for (const auto& t : tones) {
            if (t.durationSeconds > maxDuration) maxDuration = t.durationSeconds;
        }

        const u32 sampleRate = 44100;
        u32 totalSamples = static_cast<u32>(maxDuration * sampleRate);
        u32 dataBytes = totalSamples * sizeof(i16);
        std::vector<u8> buffer(sizeof(WaveHeader) + dataBytes);

        WaveHeader header;
        header.chunkSize = 36 + dataBytes;
        header.subchunk2Size = dataBytes;
        std::memcpy(buffer.data(), &header, sizeof(WaveHeader));

        i16* samples = reinterpret_cast<i16*>(buffer.data() + sizeof(WaveHeader));
        std::vector<f32> mixed(totalSamples, 0.0f);

        for (const auto& tone : tones) {
            u32 toneSamples = static_cast<u32>(tone.durationSeconds * sampleRate);
            f32 phase = 0.0f;
            for (u32 i = 0; i < toneSamples && i < totalSamples; i++) {
                f32 t = static_cast<f32>(i) / sampleRate;
                f32 progress = static_cast<f32>(i) / toneSamples;
                f32 currentFreq = Math::Lerp(tone.frequencyStart, tone.frequencyEnd, progress);

                f32 env = 1.0f;
                if (t < tone.attack && tone.attack > 0.0f) env = t / tone.attack;
                else if (t > (tone.durationSeconds - tone.decay) && tone.decay > 0.0f) {
                    env = (tone.durationSeconds - t) / tone.decay;
                    if (env < 0.0f) env = 0.0f;
                }

                f32 s = std::sin(phase) * env * tone.volume;
                phase += Math::TWO_PI * currentFreq / sampleRate;
                mixed[i] += s;
            }
        }

        for (u32 i = 0; i < totalSamples; i++) {
            f32 v = Math::Clamp(mixed[i], -1.0f, 1.0f);
            samples[i] = static_cast<i16>(v * 32767.0f);
        }

        return buffer;
    }

} // namespace Solar::Audio
