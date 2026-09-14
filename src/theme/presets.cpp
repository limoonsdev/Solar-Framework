#include "solar/theme/presets.hpp"

namespace Solar {

    Palette Presets::Get(ThemePreset preset) {
        Palette p;
        switch (preset) {
        case ThemePreset::SolarFlare:
            p.Accent       = Color::FromHex("#FF9E1B");
            p.AccentHover  = Color::FromHex("#FFAE3D");
            p.AccentActive = Color::FromHex("#E68A0E");
            p.AccentMuted  = Color::FromHex("#382714");
            break;

        case ThemePreset::CyberNeon:
            p.Accent       = Color::FromHex("#00F0FF");
            p.AccentHover  = Color::FromHex("#38F5FF");
            p.AccentActive = Color::FromHex("#00C8D6");
            p.AccentMuted  = Color::FromHex("#0C2E35");
            break;

        case ThemePreset::VoidAmethyst:
            p.Accent       = Color::FromHex("#A855F7");
            p.AccentHover  = Color::FromHex("#B870FF");
            p.AccentActive = Color::FromHex("#9333EA");
            p.AccentMuted  = Color::FromHex("#2C1542");
            break;

        case ThemePreset::EmeraldMatrix:
            p.Accent       = Color::FromHex("#10B981");
            p.AccentHover  = Color::FromHex("#34D399");
            p.AccentActive = Color::FromHex("#059669");
            p.AccentMuted  = Color::FromHex("#0C2E21");
            break;

        case ThemePreset::BloodRuby:
            p.Accent       = Color::FromHex("#F43F5E");
            p.AccentHover  = Color::FromHex("#FB7185");
            p.AccentActive = Color::FromHex("#E11D48");
            p.AccentMuted  = Color::FromHex("#36111A");
            break;

        case ThemePreset::ArcticWhite:
            p.Accent       = Color::FromHex("#38BDF8");
            p.AccentHover  = Color::FromHex("#7DD3FC");
            p.AccentActive = Color::FromHex("#0284C7");
            p.AccentMuted  = Color::FromHex("#122A3B");
            break;

        case ThemePreset::TokyoNight:
            p.Accent       = Color::FromHex("#7AA2F7");
            p.AccentHover  = Color::FromHex("#89DDFF");
            p.AccentActive = Color::FromHex("#565F89");
            p.AccentMuted  = Color::FromHex("#1F2335");
            break;

        case ThemePreset::MidnightCarbon:
            p.Accent       = Color::FromHex("#94A3B8");
            p.AccentHover  = Color::FromHex("#CBD5E1");
            p.AccentActive = Color::FromHex("#64748B");
            p.AccentMuted  = Color::FromHex("#222733");
            break;
        }
        return p;
    }

    std::vector<PresetInfo> Presets::GetAll() {
        return {
            { ThemePreset::SolarFlare, "Solar Flare", "Signature amber gold with obsidian background", Get(ThemePreset::SolarFlare) },
            { ThemePreset::CyberNeon, "Cyber Neon", "Electric cyan hyper-contrast aesthetic", Get(ThemePreset::CyberNeon) },
            { ThemePreset::VoidAmethyst, "Void Amethyst", "Deep royal purple with glowing magenta hues", Get(ThemePreset::VoidAmethyst) },
            { ThemePreset::EmeraldMatrix, "Emerald Matrix", "Toxic green cybernetic styling", Get(ThemePreset::EmeraldMatrix) },
            { ThemePreset::BloodRuby, "Blood Ruby", "Sleek crimson red and dark burgundy tones", Get(ThemePreset::BloodRuby) },
            { ThemePreset::ArcticWhite, "Arctic White", "High-clarity ice blue and platinum accents", Get(ThemePreset::ArcticWhite) },
            { ThemePreset::TokyoNight, "Tokyo Night", "Midnight indigo and lavender synthwave", Get(ThemePreset::TokyoNight) },
            { ThemePreset::MidnightCarbon, "Midnight Carbon", "Ultra-clean stealth monochromatic slate", Get(ThemePreset::MidnightCarbon) }
        };
    }

} // namespace Solar
