#include "solar/auth/hwid_generator.hpp"
#include <windows.h>
#include <sstream>
#include <iomanip>

namespace Solar::Auth {

    std::string HWIDGenerator::GetMachineFingerprint() {
        DWORD volSerial = 0;
        GetVolumeInformationA("C:\\", NULL, 0, &volSerial, NULL, NULL, NULL, 0);

        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');
        ss << "SOLAR-" << std::setw(8) << volSerial << "-B702-89AF";
        return ss.str();
    }

} // namespace Solar::Auth
