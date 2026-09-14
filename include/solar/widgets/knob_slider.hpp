#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    /**
     * @brief Circular 270-Degree Rotary Knob Slider
     * Allows precision parameter tweaking via vertical or angular dragging, with radial arc indicators.
     */
    bool KnobSlider(const char* label, float* v, float v_min, float v_max,
                    float radius = 26.0f, const char* format = "%.1f", const char* unit = "");

} // namespace Solar::Widgets
