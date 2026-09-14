#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    /**
     * @brief Number Stepper with Micro [-] / [+] Action Buttons
     */
    bool NumberStepper(const char* label, int* value, int minVal, int maxVal, int step = 1);

} // namespace Solar::Widgets
