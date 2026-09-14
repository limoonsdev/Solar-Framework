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

        float baseSize = 16.0f * m_dpiScale;
        float titleSize = 18.0f * m_dpiScale;
        float iconSize = 15.0f * m_dpiScale;

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

        return true;
    }

} // namespace Solar::Render
