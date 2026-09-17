#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <imgui.h>

namespace Solar::UI {

    class Window {
    public:
        static bool Begin(const char* name, bool* p_open = nullptr, const ImVec2& defaultSize = ImVec2(940, 620));
        static void End();
        static void RenderResizeGrip(const ImVec2& winPos, const ImVec2& winSize);
    };

} // namespace Solar::UI
