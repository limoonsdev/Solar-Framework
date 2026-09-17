#pragma once
#include "solar/core/types.hpp"
#include <string>

namespace Solar::Render {

    enum class GamingFontPreset {
        ValorantTactical = 0,   // Bahnschrift (Tactical SemiBold DIN)
        CyberpunkTerminal,      // Consolas Bold (Netrunner Matrix / Hacker HUD)
        CallOfDutyWarzone,      // Arial Black (Heavy Tactical Military)
        ArcadeTitan,            // Impact (Classic Titan / Arcade)
        ApexEsports,            // Segoe UI Black (Modern Competitive Esports)
        OverwatchSciFi,         // Corbel Bold (Clean Futuristic Sci-Fi)
        HaloSpartan,            // Trebuchet MS Bold (UNSC Military HUD)
        Count
    };

    class FontManager {
    public:
        static FontManager& Get();

        bool LoadFonts(float dpiScale = 1.0f);
        ImFont* GetRegularFont() const { return m_regular; }
        ImFont* GetBoldFont() const { return m_bold; }
        ImFont* GetIconFont() const { return m_icons; }
        ImFont* GetTitleFont() const { return m_title; }

        static const char* GetGamingFontName(GamingFontPreset preset);
        static const char* GetGamingFontDescription(GamingFontPreset preset);
        ImFont* GetGamingFont(GamingFontPreset preset, bool large = true) const;

        float GetDpiScale() const { return m_dpiScale; }

    private:
        FontManager() = default;
        ImFont* m_regular = nullptr;
        ImFont* m_bold = nullptr;
        ImFont* m_icons = nullptr;
        ImFont* m_title = nullptr;

        ImFont* m_gamingFontsMedium[static_cast<int>(GamingFontPreset::Count)] = {};
        ImFont* m_gamingFontsLarge[static_cast<int>(GamingFontPreset::Count)] = {};

        float m_dpiScale = 1.0f;
    };

} // namespace Solar::Render
