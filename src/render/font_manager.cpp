#include "solar/render/font_manager.hpp"
#include "solar/render/fa_solid_data.hpp"
#include <windows.h>

namespace Solar::Render {

    FontManager& FontManager::Get() {
        static FontManager instance;
        return instance;
    }

    bool FontManager::LoadFonts(float dpiScale) {
        m_dpiScale = (dpiScale < 1.0f) ? 1.0f : dpiScale;

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();

        ImFontConfig fontConfig;
        fontConfig.OversampleH = 3;
        fontConfig.OversampleV = 3;
        fontConfig.PixelSnapH = true;
        fontConfig.RasterizerMultiply = 1.15f; // Crisp contrast, zero fuzziness

        float baseSize = 15.5f * m_dpiScale;
        float titleSize = 18.0f * m_dpiScale;
        float iconSize = 14.5f * m_dpiScale;

        // Try Segoe UI SemiBold
        if (GetFileAttributesA("C:\\Windows\\Fonts\\seguisb.ttf") != INVALID_FILE_ATTRIBUTES) {
            m_bold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguisb.ttf", baseSize, &fontConfig);
        }

        if (GetFileAttributesA("C:\\Windows\\Fonts\\segoeui.ttf") != INVALID_FILE_ATTRIBUTES) {
            m_regular = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", baseSize, &fontConfig);
        } else {
            m_regular = io.Fonts->AddFontDefault(&fontConfig);
        }

        if (!m_bold) m_bold = m_regular;

        // Title font
        if (GetFileAttributesA("C:\\Windows\\Fonts\\seguisb.ttf") != INVALID_FILE_ATTRIBUTES) {
            m_title = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguisb.ttf", titleSize, &fontConfig);
        } else {
            m_title = m_bold;
        }

        // Merge FontAwesome 6 icons into primary font
        // Search multiple paths (cwd, exe directory, parent directory)
        char exePath[MAX_PATH] = {};
        GetModuleFileNameA(nullptr, exePath, MAX_PATH);
        char* lastSlash = strrchr(exePath, '\\');
        if (lastSlash) *lastSlash = '\0';

        std::string candidatePaths[] = {
            "vendor/fa-solid-900.ttf",
            "vendor\\fa-solid-900.ttf",
            std::string(exePath) + "\\vendor\\fa-solid-900.ttf",
            std::string(exePath) + "\\..\\vendor\\fa-solid-900.ttf",
            std::string(exePath) + "\\..\\..\\vendor\\fa-solid-900.ttf",
            "C:\\Solar\\vendor\\fa-solid-900.ttf"
        };

        std::string foundFontPath;
        for (const auto& path : candidatePaths) {
            if (GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
                foundFontPath = path;
                break;
            }
        }

        ImFontConfig iconConfig;
        iconConfig.MergeMode = true;
        iconConfig.PixelSnapH = true;
        iconConfig.OversampleH = 3;
        iconConfig.OversampleV = 3;
        iconConfig.RasterizerMultiply = 1.15f;
        static const ImWchar icon_ranges[] = { 0xe000, 0xf8ff, 0 };

        if (!foundFontPath.empty()) {
            m_icons = io.Fonts->AddFontFromFileTTF(foundFontPath.c_str(), iconSize, &iconConfig, icon_ranges);
        } else {
            m_icons = io.Fonts->AddFontFromMemoryCompressedBase85TTF(FontAwesomeSolid_compressed_data_base85, iconSize, &iconConfig, icon_ranges);
        }

        // Load Video Game Fonts at Medium and Large scales for Watermarks
        struct GamingFontDef {
            GamingFontPreset preset;
            const char* primaryPath;
            const char* fallbackPath;
        };

        const GamingFontDef gamingDefs[] = {
            { GamingFontPreset::ValorantTactical,  "C:\\Windows\\Fonts\\bahnschrift.ttf", "C:\\Windows\\Fonts\\seguisb.ttf" },
            { GamingFontPreset::CyberpunkTerminal, "C:\\Windows\\Fonts\\consolab.ttf",   "C:\\Windows\\Fonts\\consola.ttf" },
            { GamingFontPreset::CallOfDutyWarzone, "C:\\Windows\\Fonts\\ariblk.ttf",     "C:\\Windows\\Fonts\\seguibl.ttf" },
            { GamingFontPreset::ArcadeTitan,       "C:\\Windows\\Fonts\\impact.ttf",     "C:\\Windows\\Fonts\\ariblk.ttf" },
            { GamingFontPreset::ApexEsports,       "C:\\Windows\\Fonts\\seguibl.ttf",    "C:\\Windows\\Fonts\\seguisb.ttf" },
            { GamingFontPreset::OverwatchSciFi,    "C:\\Windows\\Fonts\\corbelb.ttf",    "C:\\Windows\\Fonts\\corbel.ttf" },
            { GamingFontPreset::HaloSpartan,       "C:\\Windows\\Fonts\\trebucbd.ttf",   "C:\\Windows\\Fonts\\trebuc.ttf" }
        };

        ImFontConfig gConfig;
        gConfig.OversampleH = 2;
        gConfig.OversampleV = 2;
        gConfig.PixelSnapH = true;
        gConfig.RasterizerMultiply = 1.15f;

        float medSize = 19.0f * m_dpiScale;
        float lrgSize = 64.0f * m_dpiScale;

        for (const auto& def : gamingDefs) {
            int idx = static_cast<int>(def.preset);
            const char* chosenPath = nullptr;
            if (GetFileAttributesA(def.primaryPath) != INVALID_FILE_ATTRIBUTES) {
                chosenPath = def.primaryPath;
            } else if (GetFileAttributesA(def.fallbackPath) != INVALID_FILE_ATTRIBUTES) {
                chosenPath = def.fallbackPath;
            }

            if (chosenPath) {
                m_gamingFontsMedium[idx] = io.Fonts->AddFontFromFileTTF(chosenPath, medSize, &gConfig);
                m_gamingFontsLarge[idx]  = io.Fonts->AddFontFromFileTTF(chosenPath, lrgSize, &gConfig);
            } else {
                m_gamingFontsMedium[idx] = m_bold;
                m_gamingFontsLarge[idx]  = m_title;
            }
        }

        return true;
    }

    const char* FontManager::GetGamingFontName(GamingFontPreset preset) {
        switch (preset) {
        case GamingFontPreset::ValorantTactical:  return "Valorant / CS2 Tactical (DIN)";
        case GamingFontPreset::CyberpunkTerminal: return "Cyberpunk 2077 Matrix (Consolas)";
        case GamingFontPreset::CallOfDutyWarzone: return "Call of Duty / Warzone (Arial Black)";
        case GamingFontPreset::ArcadeTitan:       return "Arcade Strike / Titan (Impact)";
        case GamingFontPreset::ApexEsports:       return "Apex Legends Esports (Segoe Black)";
        case GamingFontPreset::OverwatchSciFi:    return "Overwatch Sci-Fi (Corbel Bold)";
        case GamingFontPreset::HaloSpartan:       return "Halo Spartan HUD (Trebuchet Bold)";
        default:                                  return "Standard Tactical";
        }
    }

    const char* FontManager::GetGamingFontDescription(GamingFontPreset preset) {
        switch (preset) {
        case GamingFontPreset::ValorantTactical:  return "Official tactical DIN military geometry with ultra-clean kerning";
        case GamingFontPreset::CyberpunkTerminal: return "Netrunner hacker console and monospace HUD data stream";
        case GamingFontPreset::CallOfDutyWarzone: return "Heavy assault armored typography with high mass and authority";
        case GamingFontPreset::ArcadeTitan:       return "Massive condensed arcade combat lettering with maximum impact";
        case GamingFontPreset::ApexEsports:       return "Modern geometric esports championship typography";
        case GamingFontPreset::OverwatchSciFi:    return "Futuristic clean sci-fi aesthetic with rounded aerodynamic curves";
        case GamingFontPreset::HaloSpartan:       return "Iconic UNSC military HUD visor typography";
        default:                                  return "Standard tactical HUD font";
        }
    }

    ImFont* FontManager::GetGamingFont(GamingFontPreset preset, bool large) const {
        int idx = static_cast<int>(preset);
        if (idx < 0 || idx >= static_cast<int>(GamingFontPreset::Count)) return m_bold ? m_bold : m_regular;
        ImFont* font = large ? m_gamingFontsLarge[idx] : m_gamingFontsMedium[idx];
        if (font) return font;
        return m_bold ? m_bold : m_regular;
    }

} // namespace Solar::Render
