#include "solar/config/preset_storage.hpp"
#include <fstream>
#include <sstream>

namespace Solar::Config {

    bool PresetStorage::SaveToFile(const std::string& filename, const std::string& content) {
        std::ofstream f(filename);
        if (!f.is_open()) return false;
        f << content;
        return true;
    }

    bool PresetStorage::LoadFromFile(const std::string& filename, std::string& outContent) {
        std::ifstream f(filename);
        if (!f.is_open()) return false;
        std::stringstream ss;
        ss << f.rdbuf();
        outContent = ss.str();
        return true;
    }

    std::vector<std::string> PresetStorage::ListPresets() {
        return { "Legit_MM.cfg", "SemiRage_HVH.cfg", "Competitive_Lobby.cfg", "Casual_Skins.cfg" };
    }

} // namespace Solar::Config
