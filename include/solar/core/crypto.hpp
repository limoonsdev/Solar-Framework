#pragma once
#include "types.hpp"
#include <string>

namespace Solar::Crypto {

    std::string GenerateRandomGUID();
    std::string GenerateRandomHex(size_t bytes);
    std::string GenerateMACAddress();
    std::string GenerateDiskSerial();
    u32 HashString(const std::string& str);

    // Simple compile-time XOR string obfuscation template
    template<size_t N, char K>
    struct XorString {
        char data[N];
        constexpr XorString(const char(&str)[N]) {
            for (size_t i = 0; i < N; ++i) data[i] = str[i] ^ K;
        }
        std::string Decrypt() const {
            std::string s;
            s.resize(N - 1);
            for (size_t i = 0; i < N - 1; ++i) s[i] = data[i] ^ K;
            return s;
        }
    };

} // namespace Solar::Crypto
