#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    /**
     * @brief Dual-Thumb Range Slider
     * Allows continuous bounded selection of both minimum and maximum values.
     */
    bool RangeSlider(const char* label, float* vMin, float* vMax,
                     float minLimit, float maxLimit,
                     const char* format = "%.1f", const char* unit = "");

} // namespace Solar::Widgets
