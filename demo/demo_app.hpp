#pragma once

#include "solar/solar.hpp"
#include <string>
#include <vector>

namespace Solar {

    class DemoApp {
    public:
        static DemoApp& Get();

        void Initialize();
        void Render();
        void SetCurrentTab(int tab) {
            m_currentTab = tab;
            PushNavHistory(tab);
        }
        void SetWidgetsPage(int page) { m_widgetsPage = page; }
        void SetMiscSubTab(int sub) { m_miscSubTab = sub; }
        void SetThemeSubTab(int sub) { m_themeSubTab = sub; }

    private:
        DemoApp() = default;

        // Navigation state
        int m_currentTab = 0;
        int m_combatSubTab = 0;
        int m_visualsSubTab = 0;
        int m_miscSubTab = 0;
        int m_themeSubTab = 0;
        int m_securitySubTab = 0;

        // Peach-Framework inspired Breadcrumb Navigation History
        std::vector<int> m_navHistory;
        int m_navHistoryIndex = -1;
        void PushNavHistory(int tab);
        void NavBack();
        void NavForward();
        bool CanNavBack() const { return m_navHistoryIndex > 0; }
        bool CanNavForward() const { return m_navHistoryIndex >= 0 && m_navHistoryIndex < static_cast<int>(m_navHistory.size()) - 1; }

        bool m_windowOpen = true;
        bool m_minimized = false;

        // Aimbot state
        bool  m_aimbotEnabled = true;
        bool  m_silentAim = false;
        float m_aimFov = 14.5f;
        float m_aimSmooth = 4.5f;
        int   m_targetBone = 0;
        int   m_aimKey = 0x05; // M5
        int   m_aimKeyMode = static_cast<int>(KeyMode::Hold);

        bool  m_rcsEnabled = true;
        float m_rcsPitch = 85.0f;
        float m_rcsYaw = 80.0f;
        bool  m_triggerbot = false;
        int   m_triggerDelay = 35;
        int   m_triggerKey = 0x12; // ALT

        std::vector<bool> m_hitboxes = { true, true, true, true, false, false, false, false };

        // Visuals / ESP state
        ESPPreviewSettings m_espSettings;
        int    m_espPage = 0;
        int    m_worldPage = 0;
        int    m_espStance = 0;
        bool   m_drawFOVCircle = true;
        float  m_fovRadius = 125.0f;
        ImVec4 m_fovColor = ImVec4(1.0f, 0.75f, 0.15f, 0.85f);
        bool   m_fovGlow = true;
        int    m_snaplineOrigin = 0;
        bool   m_snaplineDashed = false;
        bool   m_offscreenArrows = true;
        float  m_hitmarkerProgress = 0.0f;
        float  m_hitmarkerDamage = 84.0f;
        float  m_targetOscillate = 0.0f;
        int    m_chamsMaterial = 0;
        float  m_chamsColor[4] = { 0.22f, 0.68f, 1.0f, 0.85f };
        bool   m_drawSpreadCrosshair = true;
        float  m_currentSpread = 24.0f;
        struct FloatingDmg {
            ImVec2 screenPos;
            float damage;
            bool isCrit;
            float lifetime;
            float initialLifetime;
            ImVec2 velocity;
        };
        std::vector<FloatingDmg> m_floatingDamages;
        float  m_acousticWaveTimer = 0.0f;

        // Widget Suite State
        int   m_widgetsPage = 0;
        int   m_widgetSubTab = 0;
        int   m_segmentedIdx = 0;
        float m_rangeMin = 20.0f;
        float m_rangeMax = 75.0f;
        int   m_stepperVal = 64;
        char  m_searchQuery[64] = "";
        std::vector<bool> m_multiDropdownSelections = { true, false, true, false };
        std::vector<std::string> m_multiDropdownItems = { "Chams Shading", "Glow Contour", "Bullet Tracers", "Sound Wave Rings" };
        float m_knobVal = 55.0f;
        std::vector<bool> m_chipSelections = { true, true, false, true, false };
        std::vector<std::string> m_chipItems = { "Players", "Bots", "NPCs", "Weapons", "Vehicles" };
        float m_radialGaugeVal = 74.5f;
        bool  m_accordion1Open = true;
        bool  m_accordion2Open = false;
        float m_fpsSparkline[16] = { 142.0f, 144.0f, 141.0f, 150.0f, 158.0f, 162.0f, 165.0f, 164.0f, 170.0f, 168.0f, 172.0f, 175.0f, 174.0f, 178.0f, 180.0f, 185.0f };
        float m_latencySparkline[16] = { 1.2f, 1.1f, 1.3f, 0.9f, 0.8f, 0.75f, 0.7f, 0.65f, 0.6f, 0.55f, 0.5f, 0.48f, 0.45f, 0.42f, 0.40f, 0.38f };

        // Combat & Visuals Extended State
        int    m_combatPage = 0;
        int    m_recoilWeapon = 0;
        int    m_recoilBullet = -1;
        bool   m_showRecoilComp = true;
        int    m_trajectoryType = 0;
        float  m_throwAngle = 48.0f;
        float  m_throwPower = 1.0f;

        // Security / Pattern Scanner State
        int       m_securityPage = 0;
        char      m_patternInput[128] = "48 8B 05 ? ? ? ? 48 85 C0";
        char      m_patternModule[64] = "ntdll.dll";
        uintptr_t m_patternResult = 0;
        uintptr_t m_scannedBase = 0;
        size_t    m_scannedSize = 0;
        uintptr_t m_resolvedRelative = 0;
        bool      m_hasScanned = false;

        // Misc state
        bool m_bhop = true;
        bool m_autoStrafe = true;
        bool m_fastCrouch = false;
        int  m_fakeLag = 4;
        bool m_showWatermark = true;
        WatermarkInfo m_watermarkInfo;
        ScreenWatermarkSettings m_screenWatermark;
        char m_customLinkInput[128] = "discord.gg/solarud";

        // FOV & Skin Changer State
        Game::FOVSettings m_fovSettings;
        Game::SkinItem m_activeSkin = { "Vandal", "Prime 2.0", Game::SkinRarity::Ultra, 0.0210f, 661, true, 1337, ImVec4(1.0f, 0.28f, 0.55f, 1.0f), true };
        std::vector<Game::SkinItem> m_skinInventory;
        char m_screenWatermarkTextInput[128] = ".gg/solarud";
        int  m_watermarkPosIndex = 0;
        int  m_screenFontIndex = 0;
        int  m_screenLayoutIndex = 0;
        int  m_screenEffectIndex = 0;
        bool m_showSpectators = true;
        bool m_showKeybinds = true;
        bool m_showRadarWindow = true;
        bool m_showProfiler = false;

        // Spoofer state
        SpooferState m_spooferState;

        // License state
        char m_licenseKey[64] = "SOLAR-PRO-9842-8819-B21A";
        bool m_rememberMe = true;
        bool m_loggedIn = false;

        // Config state
        char m_configNameInput[64] = "MyNewConfig";
        int  m_selectedConfigIndex = 0;

        // Custom accent color & Rotating border config
        float m_customColor[4] = { 1.0f, 0.48f, 0.0f, 1.0f };
        bool  m_enableRotatingBorders = true;
        FX::RotatingBorderConfig m_rotatingBorderConfig;

        // Satellite & Floating Dock System
        bool m_showSatelliteKeybinds = true;
        bool m_showSatelliteSpectators = false;
        UI::SatelliteConfig m_satelliteKeybindsConfig;
        UI::SatelliteConfig m_satelliteSpectatorsConfig;

        bool  m_showWelcomeModal = false;
    };

} // namespace Solar
