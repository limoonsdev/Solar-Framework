#pragma once
#include "solar/core/types.hpp"

#include <vector>
#include <string>

namespace Solar::Widgets {

    bool Combo(const char* label, int* current_item, const char* const items[], int items_count);
    bool Combo(const char* label, int* current_item, const std::vector<std::string>& items);

} // namespace Solar::Widgets
