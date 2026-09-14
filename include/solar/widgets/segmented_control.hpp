#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>
#include <vector>
#include <string>

namespace Solar::Widgets {

    /**
     * @brief Sliding Segmented Control (iOS / PastOwl Style)
     * Displays a horizontal pill container with an animated sliding highlight.
     */
    bool SegmentedControl(const char* label, int* selectedIndex,
                          const std::vector<std::string>& items,
                          float height = 32.0f);

} // namespace Solar::Widgets
