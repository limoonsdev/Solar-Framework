#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    /**
     * @brief KPI Metric Card with Delta Badge and Mini Sparkline
     * Displays a telemetry/metric card showing a key performance indicator, delta indicator,
     * and historical sparkline trend graph.
     */
    void StatCard(const char* title, const char* value,
                  const char* deltaText = nullptr, bool deltaPositive = true,
                  const float* sparklineData = nullptr, int sparklineCount = 0,
                  float width = 0.0f, float height = 85.0f);

} // namespace Solar::Widgets
