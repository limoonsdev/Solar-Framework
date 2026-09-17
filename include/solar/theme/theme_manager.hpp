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

        void SetRainbowMode(bool enabled) { m_rainbowMode = enabled; }
        bool IsRainbowMode() const { return m_rainbowMode; }
        void SetRainbowSpeed(float speed) { m_rainbowSpeed = speed; }
        float GetRainbowSpeed() const { return m_rainbowSpeed; }

        void Update(float dt);
        void SyncWithImGui();

        static u32 ToU32(const Color& c) { return c.ToU32(); }

    private:
        ThemeManager() = default;
        Palette m_palette;
        Style m_style;
        ThemePreset m_currentPreset = ThemePreset::SolarFlare;
        bool m_rainbowMode = false;
        float m_rainbowSpeed = 1.0f;
        float m_rainbowTime = 0.0f;
    };

} // namespace Solar
