#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Solar {

    class ConfigManager {
    public:
        static ConfigManager& Get();

        void SetBool(const std::string& key, bool value);
        bool GetBool(const std::string& key, bool defaultVal = false);

        void SetInt(const std::string& key, int value);
        int GetInt(const std::string& key, int defaultVal = 0);

        void SetFloat(const std::string& key, float value);
        float GetFloat(const std::string& key, float defaultVal = 0.0f);

        void SetString(const std::string& key, const std::string& value);
        std::string GetString(const std::string& key, const std::string& defaultVal = "");

        void SetColor(const std::string& key, float col[4]);
        void GetColor(const std::string& key, float out[4], float defR = 1.0f, float defG = 1.0f, float defB = 1.0f, float defA = 1.0f);

        bool SaveToFile(const std::string& filePath);
        bool LoadFromFile(const std::string& filePath);

        std::string ExportToString();
        bool ImportFromString(const std::string& data);

        const std::vector<std::string>& GetSavedPresetNames() const { return m_presetNames; }
        void RefreshPresetList();

    private:
        ConfigManager();
        std::unordered_map<std::string, std::string> m_data;
        std::vector<std::string> m_presetNames;
    };

} // namespace Solar
