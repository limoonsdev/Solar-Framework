#pragma once
#include "types.hpp"
#include <string>
#include <vector>
#include <string_view>

namespace Solar::StringUtils {

    std::string Format(const char* fmt, ...);
    std::vector<std::string> Split(const std::string& str, char delimiter);
    std::string Trim(const std::string& str);
    std::string ToUpper(const std::string& str);
    std::string ToLower(const std::string& str);
    u32 Fnv1aHash(std::string_view str);

    std::string Base64Encode(const std::string& in);
    std::string Base64Decode(const std::string& in);

} // namespace Solar::StringUtils
