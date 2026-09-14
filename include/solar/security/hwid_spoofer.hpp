#pragma once
#include "solar/core/types.hpp"
#include <string>

namespace Solar::Security {

    struct SpooferState {
        std::string moboUUID;
        std::string macAddress;
        std::string diskSerial;
        std::string gpuGuid;
        bool spoofMobo = true;
        bool spoofMAC = true;
        bool spoofDisk = true;
        bool spoofGPU = true;
        bool cleanVanguard = true;
        bool cleanEAC = true;
        bool cleanBattlEye = true;
        bool cleanFiveM = true;
        bool cleanDiscord = true;
        bool isSpoofed = false;
    };

    class SpooferPanel {
    public:
        static void Render(SpooferState& state);
    };

} // namespace Solar::Security
