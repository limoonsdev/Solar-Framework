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

        case ThemePreset::ObsidianVeil:
            p.Background   = Color::FromHex("#101010");
            p.Header       = Color::FromHex("#161616");
            p.Sidebar      = Color::FromHex("#121212");
            p.Card         = Color::FromHex("#181818");
            p.CardHover    = Color::FromHex("#202020");
            p.Border       = Color::FromHex("#262626");
            p.BorderHover  = Color::FromHex("#5E69EE");
            p.Accent       = Color::FromHex("#5E69EE");
            p.AccentHover  = Color::FromHex("#707CEE");
            p.AccentActive = Color::FromHex("#4B57DB");
            p.AccentMuted  = Color::FromHex("#B388EB");
            p.TextPrimary  = Color::FromHex("#FFFFFF");
            p.TextSecondary= Color::FromHex("#8C8C8C");
            p.TextDisabled = Color::FromHex("#525252");
            break;

        case ThemePreset::PhantomSpectre:
            p.Background   = Color::FromHex("#0B0D11");
            p.Header       = Color::FromHex("#10131A");
            p.Sidebar      = Color::FromHex("#0E1015");
            p.Card         = Color::FromHex("#131720");
            p.CardHover    = Color::FromHex("#181E29");
            p.Border       = Color::FromHex("#1F2736");
            p.BorderHover  = Color::FromHex("#2DD4BF");
            p.Accent       = Color::FromHex("#2DD4BF"); // Arctic Mint
            p.AccentHover  = Color::FromHex("#5EEAD4");
            p.AccentActive = Color::FromHex("#14B8A6");
            p.AccentMuted  = Color::FromHex("#0F3B36");
            p.TextPrimary  = Color::FromHex("#F0FDFA");
            p.TextSecondary= Color::FromHex("#79939E");
            p.TextDisabled = Color::FromHex("#3D4B53");
            break;

        case ThemePreset::ChampagneRoyale:
            p.Background   = Color::FromHex("#0C0B0A");
            p.Header       = Color::FromHex("#131210");
            p.Sidebar      = Color::FromHex("#100F0D");
            p.Card         = Color::FromHex("#161512");
            p.CardHover    = Color::FromHex("#1F1C18");
            p.Border       = Color::FromHex("#292520");
            p.BorderHover  = Color::FromHex("#D4AF37");
            p.Accent       = Color::FromHex("#D4AF37"); // Champagne Royale Gold
            p.AccentHover  = Color::FromHex("#F3E5AB");
            p.AccentActive = Color::FromHex("#AA8C2C");
            p.AccentMuted  = Color::FromHex("#3D3315");
            p.TextPrimary  = Color::FromHex("#FFFDF5");
            p.TextSecondary= Color::FromHex("#9E9585");
            p.TextDisabled = Color::FromHex("#544E44");
            break;
        }
        return p;
    }

    std::vector<PresetInfo> Presets::GetAll() {
        return {
            { ThemePreset::SolarApex, "Solar Apex", "Hyperion signature amber gold with obsidian carbon background", Get(ThemePreset::SolarApex) },
            { ThemePreset::ObsidianVeil, "Obsidian Veil", "Peach-Framework luxury obsidian glass with electric periwinkle and lavender", Get(ThemePreset::ObsidianVeil) },
            { ThemePreset::CyberNexus, "Cyber Nexus", "High-contrast electric cyan & neon aqua cybernetic aesthetic", Get(ThemePreset::CyberNexus) },
            { ThemePreset::AstralAmethyst, "Astral Amethyst", "Royal amethyst purple with glowing celestial magenta hues", Get(ThemePreset::AstralAmethyst) },
            { ThemePreset::ViperMatrix, "Viper Matrix", "Military toxic emerald and bioluminescent mint styling", Get(ThemePreset::ViperMatrix) },
            { ThemePreset::AresCrimson, "Ares Crimson", "Tactical crimson scarlet and dark burgundy combat tones", Get(ThemePreset::AresCrimson) },
            { ThemePreset::SubzeroFrost, "Subzero Frost", "Cryogenic ice blue and platinum chrome accents", Get(ThemePreset::SubzeroFrost) },
            { ThemePreset::NeoTokyo2077, "Neo Tokyo 2077", "Midnight indigo, ultraviolet and synthwave harmonics", Get(ThemePreset::NeoTokyo2077) },
            { ThemePreset::StealthMonolith, "Stealth Monolith", "Ultra-clean aerospace matte carbon and stealth slate", Get(ThemePreset::StealthMonolith) },
            { ThemePreset::PhantomSpectre, "Phantom Spectre", "Ghost stealth monochrome with vibrant arctic mint accents", Get(ThemePreset::PhantomSpectre) },
            { ThemePreset::ChampagneRoyale, "Champagne Royale", "Prestige champagne gold with dark velvet obsidian framing", Get(ThemePreset::ChampagneRoyale) }
        };
    }

} // namespace Solar
