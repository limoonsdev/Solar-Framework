#pragma once
#include "palette.hpp"
#include <vector>
#include <string>

namespace Solar {

    enum class ThemePreset {
        SolarFlare = 0, // Amber / Gold
        CyberNeon,      // Electric Cyan
        VoidAmethyst,   // Royal Purple
        EmeraldMatrix,  // Vivid Toxic Green
        BloodRuby,      // Crimson Red
        ArcticWhite,    // Frost Ice Blue
        TokyoNight,     // Indigo / Lilac
        MidnightCarbon  // Monochromatic Slate
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
