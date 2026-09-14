#include "solar/config/json_serializer.hpp"
#include <sstream>

namespace Solar::Config {

    void SimpleJson::SetBool(const std::string& key, bool val) {
        m_data[key] = val ? "true" : "false";
    }

    void SimpleJson::SetFloat(const std::string& key, float val) {
        m_data[key] = std::to_string(val);
    }

    void SimpleJson::SetInt(const std::string& key, int val) {
        m_data[key] = std::to_string(val);
    }

    void SimpleJson::SetString(const std::string& key, const std::string& val) {
        m_data[key] = val;
    }

    bool SimpleJson::GetBool(const std::string& key, bool def) const {
        auto it = m_data.find(key);
        if (it == m_data.end()) return def;
        return it->second == "true" || it->second == "1";
    }

    float SimpleJson::GetFloat(const std::string& key, float def) const {
        auto it = m_data.find(key);
        if (it == m_data.end()) return def;
        try { return std::stof(it->second); } catch (...) { return def; }
    }

    int SimpleJson::GetInt(const std::string& key, int def) const {
        auto it = m_data.find(key);
        if (it == m_data.end()) return def;
        try { return std::stoi(it->second); } catch (...) { return def; }
    }

    std::string SimpleJson::GetString(const std::string& key, const std::string& def) const {
        auto it = m_data.find(key);
        if (it == m_data.end()) return def;
        return it->second;
    }

    std::string SimpleJson::Serialize() const {
        std::stringstream ss;
        ss << "{\n";
        size_t count = 0;
        for (const auto& kv : m_data) {
            ss << "  \"" << kv.first << "\": \"" << kv.second << "\"";
            if (++count < m_data.size()) ss << ",";
            ss << "\n";
        }
        ss << "}\n";
        return ss.str();
    }

    bool SimpleJson::Deserialize(const std::string& json) {
        m_data.clear();
        std::stringstream ss(json);
        std::string line;
        while (std::getline(ss, line)) {
            size_t q1 = line.find('"');
            if (q1 == std::string::npos) continue;
            size_t q2 = line.find('"', q1 + 1);
            if (q2 == std::string::npos) continue;
            std::string key = line.substr(q1 + 1, q2 - q1 - 1);

            size_t q3 = line.find('"', q2 + 1);
            if (q3 == std::string::npos) continue;
            size_t q4 = line.find('"', q3 + 1);
            if (q4 == std::string::npos) continue;
            std::string val = line.substr(q3 + 1, q4 - q3 - 1);

            m_data[key] = val;
        }
        return true;
    }

} // namespace Solar::Config
