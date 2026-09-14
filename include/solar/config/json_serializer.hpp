#pragma once
#include "solar/core/types.hpp"
#include <string>
#include <unordered_map>

namespace Solar::Config {

    class SimpleJson {
    public:
        void SetBool(const std::string& key, bool val);
        void SetFloat(const std::string& key, float val);
        void SetInt(const std::string& key, int val);
        void SetString(const std::string& key, const std::string& val);

        bool GetBool(const std::string& key, bool def = false) const;
        float GetFloat(const std::string& key, float def = 0.0f) const;
        int GetInt(const std::string& key, int def = 0) const;
        std::string GetString(const std::string& key, const std::string& def = "") const;

        std::string Serialize() const;
        bool Deserialize(const std::string& json);

    private:
        std::unordered_map<std::string, std::string> m_data;
    };

} // namespace Solar::Config
