#pragma once
#include "solar/core/types.hpp"

namespace Solar::Widgets {

    bool Keybind(const char* label, int* key, KeyMode* mode);
    bool Keybind(const char* label, int* key, int* mode);

} // namespace Solar::Widgets
