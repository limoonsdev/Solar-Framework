#pragma once
#include "solar/core/types.hpp"
#include <string>

namespace Solar::Auth {

    class HWIDGenerator {
    public:
        static std::string GetMachineFingerprint();
    };

} // namespace Solar::Auth
