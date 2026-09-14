#pragma once
#include "solar/core/types.hpp"

namespace Solar::Auth {

    class LicenseScreen {
    public:
        static void Render(char* licenseBuffer, size_t bufferSize, const char* hwid, bool* rememberMe, bool* loggedIn);
    };

} // namespace Solar::Auth
