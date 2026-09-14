#pragma once
#include "solar/core/types.hpp"

namespace Solar::UI {

    class Sidebar {
    public:
        static void Begin(float width = 210.0f);
        static void End();
        static void Category(const char* label);
        static bool Tab(const char* label, int id, int* currentTab, const char* icon = nullptr);
    };

} // namespace Solar::UI
