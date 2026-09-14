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

    private:
        DemoApp() = default;

        // Navigation state
        int m_currentTab = 0;
        int m_combatSubTab = 0;
        int m_visualsSubTab = 0;
        int m_miscSubTab = 0;
        int m_themeSubTab = 0;
        int m_securitySubTab = 0;

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
        bool m_showSpectators = true;
        bool m_showKeybinds = true;
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

        // Custom accent color
        float m_customColor[4] = { 1.0f, 0.48f, 0.0f, 1.0f };
        bool  m_enableRotatingBorders = true;
        bool  m_showWelcomeModal = false;
    };

} // namespace Solar
