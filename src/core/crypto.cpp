#include "solar/core/crypto.hpp"
#include <random>
#include <sstream>
#include <iomanip>

namespace Solar::Crypto {

    static std::mt19937& GetEngine() {
        static std::random_device rd;
        static std::mt19937 eng(rd());
        return eng;
    }

    std::string GenerateRandomGUID() {
        std::uniform_int_distribution<u32> dist(0, 0xFFFFFFFF);
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');
        ss << std::setw(8) << dist(GetEngine()) << "-"
           << std::setw(4) << (dist(GetEngine()) & 0xFFFF) << "-"
           << std::setw(4) << ((dist(GetEngine()) & 0x0FFF) | 0x4000) << "-"
           << std::setw(4) << ((dist(GetEngine()) & 0x3FFF) | 0x8000) << "-"
           << std::setw(8) << dist(GetEngine()) << std::setw(4) << (dist(GetEngine()) & 0xFFFF);
        return ss.str();
    }

    std::string GenerateRandomHex(size_t bytes) {
        std::uniform_int_distribution<u32> dist(0, 255);
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');
        for (size_t i = 0; i < bytes; ++i) {
            ss << std::setw(2) << dist(GetEngine());
        }
        return ss.str();
    }

    std::string GenerateMACAddress() {
        std::uniform_int_distribution<u32> dist(0, 255);
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');
        for (int i = 0; i < 6; ++i) {
            ss << std::setw(2) << dist(GetEngine());
            if (i != 5) ss << ":";
        }
        return ss.str();
    }

    std::string GenerateDiskSerial() {
        return "WD-WCC" + GenerateRandomHex(6);
    }

    u32 HashString(const std::string& str) {
        u32 hash = 0x811c9dc5;
        for (char c : str) {
            hash ^= static_cast<u8>(c);
            hash *= 0x01000193;
        }
        return hash;
    }

} // namespace Solar::Crypto
