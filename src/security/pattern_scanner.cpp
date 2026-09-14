#include "solar/security/pattern_scanner.hpp"
#include <psapi.h>
#include <sstream>
#include <algorithm>

namespace Solar::Security {

    static std::vector<int> ParseSignature(const char* signature) {
        std::vector<int> bytes;
        std::istringstream stream(signature);
        std::string word;

        while (stream >> word) {
            if (word == "?" || word == "??") {
                bytes.push_back(-1); // Wildcard
            } else {
                bytes.push_back(static_cast<int>(std::strtoul(word.c_str(), nullptr, 16)));
            }
        }
        return bytes;
    }

    bool PatternScanner::GetModuleBounds(const char* moduleName, uintptr_t& baseOut, size_t& sizeOut) {
        HMODULE hMod = moduleName ? GetModuleHandleA(moduleName) : GetModuleHandleA(nullptr);
        if (!hMod) return false;

        MODULEINFO modInfo = {};
        if (!GetModuleInformation(GetCurrentProcess(), hMod, &modInfo, sizeof(modInfo))) {
            return false;
        }

        baseOut = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
        sizeOut = static_cast<size_t>(modInfo.SizeOfImage);
        return true;
    }

    uintptr_t PatternScanner::ScanRange(uintptr_t start, size_t size, const char* signature) {
        if (!start || size == 0 || !signature) return 0;

        std::vector<int> pattern = ParseSignature(signature);
        if (pattern.empty()) return 0;

        const uint8_t* scanBytes = reinterpret_cast<const uint8_t*>(start);
        size_t patternSize = pattern.size();

        for (size_t i = 0; i <= size - patternSize; ++i) {
            bool found = true;
            for (size_t j = 0; j < patternSize; ++j) {
                if (pattern[j] != -1 && scanBytes[i + j] != static_cast<uint8_t>(pattern[j])) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return start + i;
            }
        }
        return 0;
    }

    uintptr_t PatternScanner::FindPattern(const char* moduleName, const char* signature) {
        uintptr_t base = 0;
        size_t size = 0;
        if (!GetModuleBounds(moduleName, base, size)) return 0;
        return ScanRange(base, size, signature);
    }

    uintptr_t PatternScanner::ResolveRelative(uintptr_t instructionAddress, int offsetPosition, int nextInstructionOffset) {
        if (!instructionAddress) return 0;

        int32_t relativeOffset = *reinterpret_cast<int32_t*>(instructionAddress + offsetPosition);
        return instructionAddress + nextInstructionOffset + relativeOffset;
    }

} // namespace Solar::Security
