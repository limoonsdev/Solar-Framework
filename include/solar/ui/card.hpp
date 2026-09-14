#pragma once
#include "solar/core/types.hpp"

namespace Solar::UI {

    class Card {
    public:
        static bool Begin(const char* str_id, const char* title, const ImVec2& size = ImVec2(0, 0), const char* icon = nullptr);
        static void End();
    };

} // namespace Solar::UI
