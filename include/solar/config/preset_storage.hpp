#pragma once
#include "solar/core/types.hpp"
#include <string>
#include <vector>

namespace Solar::Config {

    class PresetStorage {
    public:
        static bool SaveToFile(const std::string& filename, const std::string& content);
        static bool LoadFromFile(const std::string& filename, std::string& outContent);
        static std::vector<std::string> ListPresets();
    };

} // namespace Solar::Config
