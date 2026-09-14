#pragma once
#include "solar/core/types.hpp"
#include <vector>
#include <string>

namespace Solar::Widgets {

    bool MultiCombo(const char* label, std::vector<bool>& values, const std::vector<std::string>& items);

} // namespace Solar::Widgets
