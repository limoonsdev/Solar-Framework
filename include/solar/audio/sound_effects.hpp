#pragma once
#include "solar/core/types.hpp"
#include <vector>

namespace Solar::Audio {

    enum class SfxId {
        Click = 0,
        ToggleOn,
        ToggleOff,
        Notification,
        Warning,
        Error,
        SliderTick,
        TabSwitch
    };

    class SoundBank {
    public:
        static SoundBank& Get();
        const std::vector<u8>& GetWaveData(SfxId id);

    private:
        SoundBank();
        void PrebakeAll();

        std::vector<u8> m_click;
        std::vector<u8> m_toggleOn;
        std::vector<u8> m_toggleOff;
        std::vector<u8> m_notification;
        std::vector<u8> m_warning;
        std::vector<u8> m_error;
        std::vector<u8> m_sliderTick;
        std::vector<u8> m_tabSwitch;
    };

} // namespace Solar::Audio
