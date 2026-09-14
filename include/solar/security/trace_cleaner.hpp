#pragma once
#include "solar/core/types.hpp"
#include <string>
#include <vector>

namespace Solar::Security {

    class TraceCleaner {
    public:
        static bool CleanRegistryKey(const std::string& keyPath);
        static bool CleanFileDirectory(const std::string& dirPath);
        static void WipeAllKnownTraces();
    };

} // namespace Solar::Security
