#pragma once
#include "palette.hpp"
#include <vector>
#include <string>

namespace Solar {

    enum class ThemePreset {
        SolarApex = 0,       // Hyperion Amber Gold & Carbon
        SolarFlare = 0,      // Legacy alias
        CyberNexus = 1,      // Electric Cyan & Neon Aqua
        CyberNeon = 1,       // Legacy alias
        AstralAmethyst = 2,  // Royal Amethyst & Cosmic Magenta
        VoidAmethyst = 2,    // Legacy alias
        ViperMatrix = 3,     // Military Toxic Emerald & Bioluminescent Mint
        EmeraldMatrix = 3,   // Legacy alias
        AresCrimson = 4,     // Tactical Crimson Scarlet & Dark Burgundy
        BloodRuby = 4,       // Legacy alias
        SubzeroFrost = 5,    // Cryogenic Ice Blue & Platinum Chrome
        ArcticWhite = 5,     // Legacy alias
        NeoTokyo2077 = 6,    // Midnight Indigo, Ultraviolet & Synthwave
        TokyoNight = 6,      // Legacy alias
        StealthMonolith = 7, // Aerospace Matte Carbon & Tactical Slate
        MidnightCarbon = 7,  // Legacy alias
        ObsidianVeil = 8,    // Peach-Framework Luxury Obsidian Glass & Electric Periwinkle
        ObsidianViolet = 8,  // Legacy alias
        PhantomSpectre = 9,  // Ghost Stealth Monochrome & Arctic Mint
        ChampagneRoyale = 10 // Prestige Champagne Gold & Dark Velvet
    };

    struct PresetInfo {
        ThemePreset preset;
        std::string name;
        std::string description;
        Palette palette;
    };

    class Presets {
    public:
        static Palette Get(ThemePreset preset);
        static std::vector<PresetInfo> GetAll();
    };

} // namespace Solar
