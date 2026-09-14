#pragma once

#include <imgui.h>
#include <string>
#include <vector>

namespace Solar {

    enum class ThemePreset {
        SolarFlare,     // Amber / Gold (Default)
        CyberNeon,      // Electric Cyan / Aqua
        VoidAmethyst,   // Deep Purple
        EmeraldMatrix,  // Vivid Mint / Emerald
        BloodRuby,      // Crimson / Scarlet
        ArcticWhite,    // Ice Blue / Platinum
        Custom          // User-defined RGB accent
    };

    struct Palette {
        // Deep Obsidian Backgrounds (PastOwl / Dribbble AAA palette)
        ImVec4 Background       = ImVec4(0.047f, 0.051f, 0.071f, 1.00f); // #0C0D12
        ImVec4 WindowBg         = ImVec4(0.063f, 0.071f, 0.102f, 0.98f); // #10121A
        ImVec4 CardBg           = ImVec4(0.078f, 0.090f, 0.133f, 0.94f); // #141722
        ImVec4 CardBgHover      = ImVec4(0.094f, 0.106f, 0.157f, 0.96f); // #181B28
        ImVec4 WidgetBg         = ImVec4(0.086f, 0.098f, 0.145f, 1.00f); // #161925
        ImVec4 WidgetBgHover    = ImVec4(0.118f, 0.133f, 0.196f, 1.00f); // #1E2232
        ImVec4 WidgetBgActive   = ImVec4(0.149f, 0.169f, 0.247f, 1.00f); // #262B3F

        // Borders
        ImVec4 Border           = ImVec4(0.133f, 0.149f, 0.220f, 0.70f); // #222638
        ImVec4 CardBorder       = ImVec4(0.149f, 0.169f, 0.247f, 0.75f); // #262B3F
        ImVec4 BorderHover      = ImVec4(0.227f, 0.255f, 0.361f, 0.90f); // #3A415C
        ImVec4 BorderActive     = ImVec4(1.000f, 0.480f, 0.000f, 0.80f);

        // Texts
        ImVec4 TextPrimary      = ImVec4(0.960f, 0.965f, 0.980f, 1.00f); // Pure white/silver
        ImVec4 TextSecondary    = ImVec4(0.550f, 0.580f, 0.680f, 1.00f); // Muted slate
        ImVec4 TextDisabled     = ImVec4(0.320f, 0.350f, 0.440f, 1.00f);

        // Accent & Glow
        ImVec4 Accent           = ImVec4(1.000f, 0.480f, 0.000f, 1.00f); // Solar Orange #FF7A00
        ImVec4 AccentGradient   = ImVec4(1.000f, 0.680f, 0.150f, 1.00f); // Gold #FFAE26
        ImVec4 AccentHover      = ImVec4(1.000f, 0.580f, 0.100f, 1.00f);
        ImVec4 AccentActive     = ImVec4(0.900f, 0.400f, 0.000f, 1.00f);
        ImVec4 AccentGlow       = ImVec4(1.000f, 0.480f, 0.000f, 0.35f);

        // Status Colors
        ImVec4 Success          = ImVec4(0.133f, 0.772f, 0.368f, 1.00f); // #22C55E
        ImVec4 Warning          = ImVec4(0.917f, 0.702f, 0.031f, 1.00f); // #EAB308
        ImVec4 Danger           = ImVec4(0.957f, 0.200f, 0.400f, 1.00f); // #F43F5E
        ImVec4 Info             = ImVec4(0.000f, 0.898f, 1.000f, 1.00f); // #00E5FF

        // Header and Navigation
        ImVec4 SidebarBg        = ImVec4(0.051f, 0.059f, 0.086f, 0.98f); // #0D0F16
        ImVec4 HeaderBg         = ImVec4(0.059f, 0.067f, 0.098f, 1.00f); // #0F1119
    };

    struct StyleSettings {
        // Geometric roundings (PastOwl signature smooth curve)
        float WindowRounding    = 12.0f;
        float ChildRounding     = 10.0f;
        float FrameRounding     = 7.0f;
        float PopupRounding     = 8.0f;
        float ScrollbarRounding = 6.0f;
        float GrabRounding      = 6.0f;
        float PillRounding      = 18.0f;

        // Visual effects
        bool  EnableGlow        = true;
        float GlowIntensity     = 1.0f;
        bool  EnableShadows     = true;
        float ShadowAlpha       = 0.45f;
        bool  EnableParticles   = true;
        int   ParticleCount     = 50;

        // Audio feedback
        bool  EnableAudio       = true;

        // Animation timings (speed factor)
        float AnimationSpeed    = 14.0f;
    };

    class ThemeManager {
    public:
        static ThemeManager& Get();

        Palette& GetPalette() { return m_palette; }
        const Palette& GetPalette() const { return m_palette; }

        StyleSettings& GetStyle() { return m_style; }
        const StyleSettings& GetStyle() const { return m_style; }

        ThemePreset GetCurrentPreset() const { return m_currentPreset; }
        void ApplyPreset(ThemePreset preset);
        void SetCustomAccent(const ImVec4& accent, const ImVec4& accentGradient);

        // Apply style to ImGuiStyle context
        void ApplyToImGui();

        // Color helper conversions
        static ImU32 ToU32(const ImVec4& col, float alphaMultiplier = 1.0f);
        static ImVec4 WithAlpha(const ImVec4& col, float alpha);

    private:
        ThemeManager();
        Palette m_palette;
        StyleSettings m_style;
        ThemePreset m_currentPreset = ThemePreset::SolarFlare;
    };

} // namespace Solar
