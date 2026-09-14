#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"

namespace Solar {

    ThemeManager& ThemeManager::Get() {
        static ThemeManager instance;
        return instance;
    }

    void ThemeManager::Initialize() {
        ApplyPreset(ThemePreset::SolarFlare);
    }

    void ThemeManager::ApplyPreset(ThemePreset preset) {
        m_currentPreset = preset;
        m_palette = Presets::Get(preset);
        SyncWithImGui();
    }

    void ThemeManager::SetCustomAccent(const Color& accent, const Color& accentHover) {
        m_palette.Accent = accent;
        m_palette.AccentHover = accentHover;
        m_palette.AccentActive = accent.Multiplied(0.85f);
        m_palette.AccentMuted = accent.WithAlpha(0.18f);
        SyncWithImGui();
    }

    void ThemeManager::SyncWithImGui() {
        ImGuiStyle& s = ImGui::GetStyle();

        s.WindowRounding    = m_style.WindowRounding;
        s.ChildRounding     = m_style.CardRounding;
        s.FrameRounding     = m_style.WidgetRounding;
        s.PopupRounding     = m_style.PopupRounding;
        s.ScrollbarRounding = m_style.ScrollbarRounding;
        s.GrabRounding      = m_style.WidgetRounding;

        s.WindowBorderSize  = m_style.BorderSize;
        s.ChildBorderSize   = m_style.CardBorderSize;
        s.FrameBorderSize   = 0.0f;
        s.PopupBorderSize   = 1.0f;

        s.WindowPadding     = m_style.WindowPadding;
        s.FramePadding      = m_style.FramePadding;
        s.ItemSpacing       = m_style.ItemSpacing;
        s.ItemInnerSpacing  = ImVec2(6.0f, 6.0f);

        s.AntiAliasedLines        = true;
        s.AntiAliasedLinesUseTex  = true;
        s.AntiAliasedFill         = true;

        ImVec4* colors = s.Colors;
        colors[ImGuiCol_WindowBg]             = m_palette.Background;
        colors[ImGuiCol_ChildBg]              = m_palette.Card;
        colors[ImGuiCol_PopupBg]              = m_palette.Header;
        colors[ImGuiCol_Border]               = m_palette.Border;
        colors[ImGuiCol_BorderShadow]         = ImVec4(0, 0, 0, 0);

        colors[ImGuiCol_FrameBg]              = m_palette.Background;
        colors[ImGuiCol_FrameBgHovered]       = m_palette.CardHover;
        colors[ImGuiCol_FrameBgActive]        = m_palette.Card;

        colors[ImGuiCol_TitleBg]              = m_palette.Header;
        colors[ImGuiCol_TitleBgActive]        = m_palette.Header;
        colors[ImGuiCol_TitleBgCollapsed]     = m_palette.Header;

        colors[ImGuiCol_ScrollbarBg]          = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_ScrollbarGrab]        = m_palette.Border;
        colors[ImGuiCol_ScrollbarGrabHovered] = m_palette.BorderHover;
        colors[ImGuiCol_ScrollbarGrabActive]  = m_palette.Accent;

        colors[ImGuiCol_CheckMark]            = m_palette.Accent;
        colors[ImGuiCol_SliderGrab]           = m_palette.Accent;
        colors[ImGuiCol_SliderGrabActive]     = m_palette.AccentActive;

        colors[ImGuiCol_Button]               = m_palette.Card;
        colors[ImGuiCol_ButtonHovered]        = m_palette.CardHover;
        colors[ImGuiCol_ButtonActive]         = m_palette.Border;

        colors[ImGuiCol_Header]               = m_palette.Card;
        colors[ImGuiCol_HeaderHovered]        = m_palette.CardHover;
        colors[ImGuiCol_HeaderActive]         = m_palette.Border;

        colors[ImGuiCol_Separator]            = m_palette.Border;
        colors[ImGuiCol_SeparatorHovered]     = m_palette.BorderHover;
        colors[ImGuiCol_SeparatorActive]      = m_palette.Accent;

        colors[ImGuiCol_Text]                 = m_palette.TextPrimary;
        colors[ImGuiCol_TextDisabled]         = m_palette.TextDisabled;
    }

} // namespace Solar
