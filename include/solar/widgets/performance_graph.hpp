#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    struct GraphMetrics {
        float minVal = 0.0f;
        float maxVal = 0.0f;
        float avgVal = 0.0f;
        float curVal = 0.0f;
    };

    /**
     * @brief Luxury real-time telemetry graph with spline curve, gradient fill, and interactive scrubbing
     * @param label Unique ID and title displayed in header
     * @param values Pointer to contiguous float buffer
     * @param count Number of samples in buffer
     * @param minScale Explicit min scale bound (<= 0 for auto)
     * @param maxScale Explicit max scale bound (<= 0 for auto)
     * @param size Widget size (width <= 0 fills region)
     * @param accent Curve and gradient accent color
     * @param unit Unit suffix (e.g. "ms", "fps", "%")
     * @param outMetrics Optional output for computed stats
     */
    void PerformanceGraph(const char* label, const float* values, int count,
                          float minScale = 0.0f, float maxScale = 0.0f,
                          const ImVec2& size = ImVec2(0.0f, 95.0f),
                          Color accent = Color(0.37f, 0.41f, 0.93f, 1.0f),
                          const char* unit = "ms",
                          GraphMetrics* outMetrics = nullptr);

} // namespace Solar::Widgets
