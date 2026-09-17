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

        case ThemePreset::OnyxChroma:
            p.Background   = Color::FromHex("#09090C");
            p.Header       = Color::FromHex("#0F1017");
            p.Sidebar      = Color::FromHex("#0C0D12");
            p.Card         = Color::FromHex("#13141C");
            p.CardHover    = Color::FromHex("#1B1D28");
            p.Border       = Color::FromHex("#232533");
            p.BorderHover  = Color::FromHex("#8B5CF6");
            p.Accent       = Color::FromHex("#8B5CF6"); // Electric Chroma Violet
            p.AccentHover  = Color::FromHex("#A78BFA");
            p.AccentActive = Color::FromHex("#7C3AED");
            p.AccentMuted  = Color::FromHex("#2E1E5B");
            p.TextPrimary  = Color::FromHex("#F5F3FF");
            p.TextSecondary= Color::FromHex("#8B88A0");
            p.TextDisabled = Color::FromHex("#48455C");
            break;

        case ThemePreset::SynthwaveSunset:
            p.Background   = Color::FromHex("#0D0814");
            p.Header       = Color::FromHex("#140D20");
            p.Sidebar      = Color::FromHex("#100A1A");
            p.Card         = Color::FromHex("#1A112A");
            p.CardHover    = Color::FromHex("#24183A");
            p.Border       = Color::FromHex("#32204D");
            p.BorderHover  = Color::FromHex("#FF2A85");
            p.Accent       = Color::FromHex("#FF2A85"); // Hot Neon Magenta
            p.AccentHover  = Color::FromHex("#FF65A8");
            p.AccentActive = Color::FromHex("#D91A6D");
            p.AccentMuted  = Color::FromHex("#4D0E2B");
            p.TextPrimary  = Color::FromHex("#FFF1F6");
            p.TextSecondary= Color::FromHex("#9E7D8F");
            p.TextDisabled = Color::FromHex("#573F4E");
            break;

        case ThemePreset::GlacierTitanium:
            p.Background   = Color::FromHex("#0A0D12");
            p.Header       = Color::FromHex("#10151E");
            p.Sidebar      = Color::FromHex("#0D1118");
            p.Card         = Color::FromHex("#141A25");
            p.CardHover    = Color::FromHex("#1C2433");
            p.Border       = Color::FromHex("#253043");
            p.BorderHover  = Color::FromHex("#38BDF8");
            p.Accent       = Color::FromHex("#38BDF8"); // Glacier Ice Cyan
            p.AccentHover  = Color::FromHex("#7DD3FC");
            p.AccentActive = Color::FromHex("#0284C7");
            p.AccentMuted  = Color::FromHex("#0C2B3E");
            p.TextPrimary  = Color::FromHex("#F0F9FF");
            p.TextSecondary= Color::FromHex("#7E91A6");
            p.TextDisabled = Color::FromHex("#425061");
            break;

        case ThemePreset::KintsugiGold:
            p.Background   = Color::FromHex("#0C0C0B");
            p.Header       = Color::FromHex("#141412");
            p.Sidebar      = Color::FromHex("#10100E");
            p.Card         = Color::FromHex("#181815");
            p.CardHover    = Color::FromHex("#22221E");
            p.Border       = Color::FromHex("#303028");
            p.BorderHover  = Color::FromHex("#E5B842");
            p.Accent       = Color::FromHex("#E5B842"); // 24K Liquid Gold
            p.AccentHover  = Color::FromHex("#F7D070");
            p.AccentActive = Color::FromHex("#C89B2B");
            p.AccentMuted  = Color::FromHex("#443512");
            p.TextPrimary  = Color::FromHex("#FFFDF7");
            p.TextSecondary= Color::FromHex("#999687");
            p.TextDisabled = Color::FromHex("#525045");
            break;

        case ThemePreset::NebulaVoid:
            p.Background   = Color::FromHex("#060812");
            p.Header       = Color::FromHex("#0C1020");
            p.Sidebar      = Color::FromHex("#080B17");
            p.Card         = Color::FromHex("#11162B");
            p.CardHover    = Color::FromHex("#19203D");
            p.Border       = Color::FromHex("#222C52");
            p.BorderHover  = Color::FromHex("#06B6D4");
            p.Accent       = Color::FromHex("#06B6D4"); // Starlight Teal
            p.AccentHover  = Color::FromHex("#22D3EE");
            p.AccentActive = Color::FromHex("#0891B2");
            p.AccentMuted  = Color::FromHex("#083344");
            p.TextPrimary  = Color::FromHex("#ECFEFF");
            p.TextSecondary= Color::FromHex("#7A92A6");
            p.TextDisabled = Color::FromHex("#3F505E");
            break;

        case ThemePreset::BloodMoon:
            p.Background   = Color::FromHex("#0F0708");
            p.Header       = Color::FromHex("#180B0D");
            p.Sidebar      = Color::FromHex("#13090A");
            p.Card         = Color::FromHex("#1E0E10");
            p.CardHover    = Color::FromHex("#2B1417");
            p.Border       = Color::FromHex("#3B1A1E");
            p.BorderHover  = Color::FromHex("#EF4444");
            p.Accent       = Color::FromHex("#EF4444"); // Lunar Blood Red
            p.AccentHover  = Color::FromHex("#F87171");
            p.AccentActive = Color::FromHex("#DC2626");
            p.AccentMuted  = Color::FromHex("#450A0A");
            p.TextPrimary  = Color::FromHex("#FEF2F2");
            p.TextSecondary= Color::FromHex("#A17D81");
            p.TextDisabled = Color::FromHex("#543B3E");
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
            { ThemePreset::ChampagneRoyale, "Champagne Royale", "Prestige champagne gold with dark velvet obsidian framing", Get(ThemePreset::ChampagneRoyale) },
            { ThemePreset::OnyxChroma, "Onyx Chroma", "Deep stealth obsidian glass with chromatic violet and neon aura", Get(ThemePreset::OnyxChroma) },
            { ThemePreset::SynthwaveSunset, "Synthwave Sunset", "Retro 80s hot neon magenta and sunset peach glow", Get(ThemePreset::SynthwaveSunset) },
            { ThemePreset::GlacierTitanium, "Glacier Titanium", "Subzero polar glacier cyan with aerospace slate titanium", Get(ThemePreset::GlacierTitanium) },
            { ThemePreset::KintsugiGold, "Kintsugi Gold", "Dark Japanese ceramic charcoal infused with 24K liquid gold", Get(ThemePreset::KintsugiGold) },
            { ThemePreset::NebulaVoid, "Nebula Void", "Cosmic deep space abyss with starlight teal laser highlights", Get(ThemePreset::NebulaVoid) },
            { ThemePreset::BloodMoon, "Blood Moon", "Eclipsed lunar blood crimson with velvet smoke framing", Get(ThemePreset::BloodMoon) }
        };
    }

} // namespace Solar
