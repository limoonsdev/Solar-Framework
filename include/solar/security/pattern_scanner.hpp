#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <cstdint>

namespace Solar::Security {

    /**
     * @brief High-Speed AOB & Signature Pattern Scanner
     * Locates unexported engine functions, vtables, and offsets in game modules.
     * Supports IDA signature formats ("48 8B ? ? 74") and relative instruction resolution.
     */
    class PatternScanner {
    public:
        // Find pattern by IDA-style signature string: "48 89 5C 24 ? 48 89 74 24"
        static uintptr_t FindPattern(const char* moduleName, const char* signature);

        // Find pattern in arbitrary memory range
        static uintptr_t ScanRange(uintptr_t start, size_t size, const char* signature);

        // Resolve relative rip-relative address (e.g., mov rax, [rip + offset])
        static uintptr_t ResolveRelative(uintptr_t instructionAddress, int offsetPosition = 3, int nextInstructionOffset = 7);

        // Retrieve base address and image size of a module
        static bool GetModuleBounds(const char* moduleName, uintptr_t& baseOut, size_t& sizeOut);
    };

} // namespace Solar::Security
