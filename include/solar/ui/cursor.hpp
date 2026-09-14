#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>

namespace Solar::UI {

    class CustomCursor {
    public:
        static CustomCursor& Get();

        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        void SetClickRipple(bool enabled) { m_enableRipple = enabled; }
        void SetTrailing(bool enabled) { m_enableTrailing = enabled; }

        void Render();

    private:
        CustomCursor() = default;

        bool   m_enabled = true;
        bool   m_enableRipple = true;
        bool   m_enableTrailing = true;
        ImVec2 m_smoothPos = ImVec2(-100.0f, -100.0f);
        float  m_clickRipple = 0.0f;
        float  m_hoverAnim = 0.0f;
        float  m_clickAnim = 0.0f;
    };

} // namespace Solar::UI
