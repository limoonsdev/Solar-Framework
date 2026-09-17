#pragma once
#include "solar/core/types.hpp"
#include <string>
#include <vector>
#include <imgui.h>

namespace Solar::Widgets {

    // Interactive Chip / Pill Filter Selector
    bool ChipSelector(const char* label, std::vector<bool>& selections, const std::vector<std::string>& items);

} // namespace Solar::Widgets
