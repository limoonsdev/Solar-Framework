#pragma once
#include "solar/core/types.hpp"

namespace Solar::Widgets {

    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.1f", const char* suffix = nullptr);
    bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", const char* suffix = nullptr);

} // namespace Solar::Widgets
