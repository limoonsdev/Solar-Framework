#pragma once
#include "solar/core/types.hpp"

namespace Solar::UI {

    class Titlebar {
    public:
        static void Render(const char* title, const char* subtitle, bool* p_open = nullptr, bool* p_minimized = nullptr);
    };

} // namespace Solar::UI
