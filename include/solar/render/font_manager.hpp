#pragma once
#include "solar/core/types.hpp"
#include <string>

namespace Solar::Render {

    class FontManager {
    public:
        static FontManager& Get();

        bool LoadFonts(float dpiScale = 1.0f);
        ImFont* GetRegularFont() const { return m_regular; }
        ImFont* GetBoldFont() const { return m_bold; }
        ImFont* GetIconFont() const { return m_icons; }
        ImFont* GetTitleFont() const { return m_title; }

        float GetDpiScale() const { return m_dpiScale; }

    private:
        FontManager() = default;
        ImFont* m_regular = nullptr;
        ImFont* m_bold = nullptr;
        ImFont* m_icons = nullptr;
        ImFont* m_title = nullptr;
        float m_dpiScale = 1.0f;
    };

} // namespace Solar::Render
