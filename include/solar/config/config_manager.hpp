#pragma once
#include "json_serializer.hpp"
#include "preset_storage.hpp"
#include <vector>
#include <string>

namespace Solar {

    class ConfigManager {
    public:
        static ConfigManager& Get();

        void SaveConfig(const std::string& name);
        void LoadConfig(const std::string& name);
        std::vector<std::string> GetSavedPresetNames();

        Config::SimpleJson& GetJson() { return m_json; }

    private:
        ConfigManager() = default;
        Config::SimpleJson m_json;
    };

} // namespace Solar
