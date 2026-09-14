#include "solar/config/config_manager.hpp"

namespace Solar {

    ConfigManager& ConfigManager::Get() {
        static ConfigManager instance;
        return instance;
    }

    void ConfigManager::SaveConfig(const std::string& name) {
        std::string serialized = m_json.Serialize();
        Config::PresetStorage::SaveToFile(name + ".cfg", serialized);
    }

    void ConfigManager::LoadConfig(const std::string& name) {
        std::string content;
        if (Config::PresetStorage::LoadFromFile(name + ".cfg", content)) {
            m_json.Deserialize(content);
        }
    }

    std::vector<std::string> ConfigManager::GetSavedPresetNames() {
        return Config::PresetStorage::ListPresets();
    }

} // namespace Solar
