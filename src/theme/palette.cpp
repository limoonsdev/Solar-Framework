#include "solar/theme/palette.hpp"

namespace Solar {

    Palette::Palette() {
        // Deep Obsidian / Onyx PastOwl Signature Dark
        Background    = Color::FromHex("#0B0D13");
        Header        = Color::FromHex("#0F121C");
        Sidebar       = Color::FromHex("#0E1018");
        Card          = Color::FromHex("#141724");
        CardHover     = Color::FromHex("#191D2C");
        Border        = Color::FromHex("#22273A");
        BorderHover   = Color::FromHex("#323B58");

        // Solar Flare Luxury Amber / Gold
        Accent        = Color::FromHex("#FF9E1B");
        AccentHover   = Color::FromHex("#FFAE3D");
        AccentActive  = Color::FromHex("#E68A0E");
        AccentMuted   = Color::FromHex("#382714");

        TextPrimary   = Color::FromHex("#FFFFFF");
        TextSecondary = Color::FromHex("#949CB2");
        TextDisabled  = Color::FromHex("#52586B");

        Success       = Color::FromHex("#10B981");
        Warning       = Color::FromHex("#F59E0B");
        Danger        = Color::FromHex("#EF4444");
        Info          = Color::FromHex("#3B82F6");
    }

} // namespace Solar
