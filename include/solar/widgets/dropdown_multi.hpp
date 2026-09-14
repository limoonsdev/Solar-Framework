#pragma once
#include "solar/core/types.hpp"
#include <vector>
#include <string>

namespace Solar::Widgets {

    /**
     * @brief Dropdown Multi-Select with Tag Chips
     * Renders a modern multi-selection dropdown that displays selected items as removable badges/chips
     * with an expandable selection popover.
     */
    bool DropdownMultiSelect(const char* label, std::vector<bool>& selections, const std::vector<std::string>& items);

} // namespace Solar::Widgets
