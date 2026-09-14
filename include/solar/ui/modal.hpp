#pragma once
#include "solar/core/types.hpp"

namespace Solar::UI {

    class Modal {
    public:
        static bool Begin(const char* name, bool* p_open = nullptr, const ImVec2& size = ImVec2(400, 250));
        static void End();
    };

} // namespace Solar::UI
