#pragma once
#include "solar/core/types.hpp"

namespace Solar::UI {

    class Groupbox {
    public:
        static void Begin(const char* title, const ImVec2& size = ImVec2(0, 0));
        static void End();
    };

} // namespace Solar::UI
