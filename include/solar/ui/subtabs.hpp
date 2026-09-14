#pragma once
#include "solar/core/types.hpp"

namespace Solar::UI {

    class Subtabs {
    public:
        static bool Tab(const char* label, int id, int* currentSubTab);
    };

} // namespace Solar::UI
