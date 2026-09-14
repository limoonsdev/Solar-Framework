#pragma once
#include "solar/core/types.hpp"

namespace Solar::UI {

    class Tooltip {
    public:
        static void Show(const char* text);
        static void ItemTooltip(const char* text);
    };

} // namespace Solar::UI
