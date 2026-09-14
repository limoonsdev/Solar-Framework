#pragma once
#include "palette.hpp"
#include "style.hpp"
#include "presets.hpp"

namespace Solar {

    class ThemeManager {
    public:
        static ThemeManager& Get();

        void Initialize();
        void ApplyPreset(ThemePreset preset);
        void SetCustomAccent(const Color& accent, const Color& accentHover);

        Palette& GetPalette() { return m_palette; }
        const Palette& GetPalette() const { return m_palette; }

        Style& GetStyle() { return m_style; }
        const Style& GetStyle() const { return m_style; }

        ThemePreset GetCurrentPreset() const { return m_currentPreset; }

        void SyncWithImGui();

        static u32 ToU32(const Color& c) { return c.ToU32(); }

    private:
        ThemeManager() = default;
        Palette m_palette;
        Style m_style;
        ThemePreset m_currentPreset = ThemePreset::SolarFlare;
    };

} // namespace Solar
