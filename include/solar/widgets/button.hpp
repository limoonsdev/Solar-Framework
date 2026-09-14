#pragma once
#include "solar/core/types.hpp"

namespace Solar {
    enum class ButtonStyle {
        Primary = 0,
        Secondary,
        Danger,
        Ghost
    };
}

namespace Solar::Widgets {

    bool Button(const char* label, const ImVec2& size = ImVec2(0, 36), ButtonStyle style = ButtonStyle::Primary);

} // namespace Solar::Widgets
