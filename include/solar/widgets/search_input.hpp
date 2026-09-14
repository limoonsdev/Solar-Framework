#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>
#include <cstddef>

namespace Solar::Widgets {

    /**
     * @brief Search Input Field with Embedded Search Icon and Clear Action
     */
    bool SearchInput(const char* label, char* buffer, size_t bufferSize, const char* hint = "Search...");

} // namespace Solar::Widgets
