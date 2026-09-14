#include "solar/audio/audio_engine.hpp"
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

namespace Solar::Audio {

    AudioEngine& AudioEngine::Get() {
        static AudioEngine instance;
        return instance;
    }

    void AudioEngine::Play(SfxId id) {
        if (!m_enabled) return;
        const auto& wav = SoundBank::Get().GetWaveData(id);
        if (wav.empty()) return;

        ::PlaySoundA(reinterpret_cast<LPCSTR>(wav.data()), NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
    }

} // namespace Solar::Audio
