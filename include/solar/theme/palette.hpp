#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"

namespace Solar {

    struct Palette {
        Color Background;
        Color Header;
        Color Sidebar;
        Color Card;
        Color CardHover;
        Color Border;
        Color BorderHover;

        Color Accent;
        Color AccentHover;
        Color AccentActive;
        Color AccentMuted;

        Color TextPrimary;
        Color TextSecondary;
        Color TextDisabled;

        Color Success;
        Color Warning;
        Color Danger;
        Color Info;

        Palette();
    };

} // namespace Solar
