#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    // 240-degree Circular Cyber Arc Gauge
    bool RadialGauge(const char* label, float* v, float v_min, float v_max,
                     float radius = 38.0f, const char* format = "%.1f", const char* unit = nullptr);

} // namespace Solar::Widgets
