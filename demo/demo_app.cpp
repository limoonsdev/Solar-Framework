#include "demo_app.hpp"
#include <cstdio>

namespace Solar {

    static void RenderCardPagination(int* currentPage, int totalPages, const char* idSuffix) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        char prevLabel[48];
        snprintf(prevLabel, sizeof(prevLabel), "< Prev##%s", idSuffix);
        char nextLabel[48];
        snprintf(nextLabel, sizeof(nextLabel), "Next >##%s", idSuffix);

        bool canPrev = (*currentPage > 0);
        bool canNext = (*currentPage < totalPages - 1);

        float availW = ImGui::GetContentRegionAvail().x;
        float btnW = 75.0f;
        float badgeW = 92.0f;
        float totalW = btnW * 2.0f + badgeW + 16.0f;
        float startX = ImGui::GetCursorPosX() + (std::max)(0.0f, (availW - totalW) * 0.5f);
        ImGui::SetCursorPosX(startX);

        if (!canPrev) ImGui::BeginDisabled();
        if (Widgets::Button(prevLabel, ImVec2(btnW, 26.0f), ButtonStyle::Secondary)) {
            (*currentPage)--;
            Audio::PlayClick();
        }
        if (!canPrev) ImGui::EndDisabled();

        ImGui::SameLine(0, 8.0f);
        char pageBuf[32];
        snprintf(pageBuf, sizeof(pageBuf), "Page %d / %d", *currentPage + 1, totalPages);
        ImVec2 ts = ImGui::CalcTextSize(pageBuf);
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();
        draw->AddRectFilled(p, ImVec2(p.x + badgeW, p.y + 26.0f), pal.CardHover.WithAlpha(0.60f).ToU32(), 4.0f);
        Render::ImGuiExt::AddSmoothBorder(draw, p, ImVec2(p.x + badgeW, p.y + 26.0f), pal.Border.WithAlpha(0.50f).ToU32(), 4.0f, 1.0f);
        draw->AddText(ImVec2(p.x + (badgeW - ts.x) * 0.5f, p.y + (26.0f - ts.y) * 0.5f), pal.TextSecondary.ToU32(), pageBuf);
        ImGui::Dummy(ImVec2(badgeW, 26.0f));

        ImGui::SameLine(0, 8.0f);
        if (!canNext) ImGui::BeginDisabled();
        if (Widgets::Button(nextLabel, ImVec2(btnW, 26.0f), ButtonStyle::Secondary)) {
            (*currentPage)++;
            Audio::PlayClick();
        }
        if (!canNext) ImGui::EndDisabled();
    }

    DemoApp& DemoApp::Get() {
        static DemoApp instance;
        return instance;
    }

    void DemoApp::PushNavHistory(int tab) {
        if (m_navHistoryIndex >= 0 && m_navHistoryIndex < static_cast<int>(m_navHistory.size())) {
            if (m_navHistory[m_navHistoryIndex] == tab) return;
            m_navHistory.resize(m_navHistoryIndex + 1);
        }
        m_navHistory.push_back(tab);
        m_navHistoryIndex = static_cast<int>(m_navHistory.size()) - 1;
    }

    void DemoApp::NavBack() {
        if (CanNavBack()) {
            m_navHistoryIndex--;
            m_currentTab = m_navHistory[m_navHistoryIndex];
            Audio::PlayTabSwitch();
        }
    }

    void DemoApp::NavForward() {
        if (CanNavForward()) {
            m_navHistoryIndex++;
            m_currentTab = m_navHistory[m_navHistoryIndex];
            Audio::PlayTabSwitch();
        }
    }

    void DemoApp::Initialize() {
        m_espSettings.playerName = "Enemy_01";
        m_espSettings.weaponName = "Vandal [25/75]";
        m_espSettings.distance = 28.5f;
        m_espSettings.health = 85.0f;
        m_espSettings.armor = 60.0f;
        m_showRadarWindow = true;
        m_showSpectators = true;
        m_showKeybinds = true;
        m_showWatermark = true;
        m_watermarkInfo.frameworkName = "SOLAR";
        m_watermarkInfo.version = "v1.0.2-dev";
        m_watermarkInfo.username = "SolarDev";
        m_watermarkInfo.customLink = "discord.gg/solarud";
        m_watermarkInfo.pingMs = 12;
        m_watermarkInfo.showLink = true;

        m_screenWatermark.enabled = true;
        m_screenWatermark.text = ".gg/solarud";
        m_screenWatermark.fontPreset = GamingFontPreset::ValorantTactical;
        m_screenWatermark.opacity = 0.12f;
        m_screenWatermark.scale = 1.0f;
        m_screenWatermark.layout = ScreenWatermarkLayout::CenterDiagonal;
        m_screenWatermark.effect = ScreenWatermarkEffect::SolidAlpha;
        m_screenWatermark.useThemeColor = true;
        m_screenWatermark.animatedPulse = true;

        PushNavHistory(0);
        ThemeManager::Get().ApplyPreset(ThemePreset::ObsidianVeil);

        m_radarEntities = {
            { 14.0f, 22.0f, 0.0f, 45.0f, true, false, 1.0f },
            { -18.0f, 12.0f, 2.5f, 120.0f, true, false, 0.65f },
            { -8.0f, -25.0f, -1.0f, 280.0f, false, false, 1.0f },
            { 30.0f, -14.0f, 0.0f, 195.0f, true, true, 0.30f },
            { 6.0f, 32.0f, 1.2f, 15.0f, true, false, 0.90f }
        };

        // Register Command Palette Commands (Ctrl + P)
        auto& cp = UI::CommandPalette::Get();
        cp.RegisterCommand("Go to: Combat & Aimbot", "Navigation", "Tab 1", ICON_FA_CROSSHAIRS, [this]() {
            SetCurrentTab(0);
        });
        cp.RegisterCommand("Go to: Visuals 2.0 & ESP", "Navigation", "Tab 2", ICON_FA_EYE, [this]() {
            SetCurrentTab(1);
        });
        cp.RegisterCommand("Go to: Radar & HUD Overlays", "Navigation", "Tab 3", ICON_FA_EXPAND, [this]() {
            SetCurrentTab(2);
        });
        cp.RegisterCommand("Go to: Widget Suite", "Navigation", "Tab 4", ICON_FA_SLIDERS, [this]() {
            SetCurrentTab(3);
        });
        cp.RegisterCommand("Go to: Security & Pattern Scanner", "Navigation", "Tab 5", ICON_FA_SHIELD, [this]() {
            SetCurrentTab(4);
        });
        cp.RegisterCommand("Go to: License & Auth", "Navigation", "Tab 6", ICON_FA_LOCK, [this]() {
            SetCurrentTab(5);
        });
        cp.RegisterCommand("Go to: Themes & Engine Preferences", "Navigation", "Tab 7", ICON_FA_PALETTE, [this]() {
            SetCurrentTab(6);
        });
        cp.RegisterCommand("Go to: Profiles & Presets", "Navigation", "Tab 8", ICON_FA_FLOPPY_DISK, [this]() {
            SetCurrentTab(7);
        });
        cp.RegisterCommand("Trigger Kill Frag Banner (Esports Popup)", "Combat", "Test", ICON_FA_SKULL, []() {
            UI::TriggerKillBanner("Jett_Main_99", "VANDAL PRIME", 160, true, 4);
        });
        cp.RegisterCommand("Toggle Tactical Radar Window", "Overlays", "F1", ICON_FA_CROSSHAIRS, [this]() {
            m_showRadarWindow = !m_showRadarWindow;
        });
        cp.RegisterCommand("Toggle Watermark Overlay", "Overlays", "", ICON_FA_TAG, [this]() {
            m_showWatermark = !m_showWatermark;
        });
        cp.RegisterCommand("Toggle Advanced Big Screen Watermark", "Overlays", "", ICON_FA_DESKTOP, [this]() {
            m_screenWatermark.enabled = !m_screenWatermark.enabled;
        });
        cp.RegisterCommand("Cycle Cursor Visual Style", "Preferences", "", ICON_FA_LOCATION_ARROW, []() {
            int nextStyle = (static_cast<int>(UI::CustomCursor::Get().GetStyle()) + 1) % 5;
            UI::CustomCursor::Get().SetStyle(static_cast<UI::CursorStyle>(nextStyle));
        });
    }

    void DemoApp::Render() {
        // Luxury Animated Splash / Loading Screen
        if (UI::SplashScreen::Get().Render()) {
            UI::CustomCursor::Get().Render();
            return;
        }

        // Luxury Animated Welcome Screen Overlay
        if (UI::WelcomeScreen::Get().Render()) {
            UI::CustomCursor::Get().Render();
            return;
        }

        const ImGuiIO& io = ImGui::GetIO();

        // Fullscreen Ambient Cyber Snow / Particle FX
        if (ThemeManager::Get().GetStyle().EnableParticles) {
            FX::ParticleSystem::Get().UpdateAndRender(ImGui::GetBackgroundDrawList(),
                                                      ImVec2(0, 0), io.DisplaySize,
                                                      ThemeManager::Get().GetPalette().Accent);
        }

        // Floating Watermark HUD (with Custom Links & Copy-to-Clipboard)
        if (m_showWatermark) {
            m_watermarkInfo.customLink = m_customLinkInput;
            m_watermarkInfo.position = static_cast<WatermarkPosition>(m_watermarkPosIndex);
            Watermark::Render(m_watermarkInfo);
        }

        // Advanced Screen Watermark Overlay (Gaming Typography Suite)
        if (m_screenWatermark.enabled) {
            m_screenWatermark.text = m_screenWatermarkTextInput;
            m_screenWatermark.fontPreset = static_cast<GamingFontPreset>(m_screenFontIndex);
            m_screenWatermark.layout = static_cast<ScreenWatermarkLayout>(m_screenLayoutIndex);
            m_screenWatermark.effect = static_cast<ScreenWatermarkEffect>(m_screenEffectIndex);
            ScreenWatermark::Render(m_screenWatermark);
        }

        // Floating HUD Windows: Tactical Radar, Spectators & Keybinds
        if (m_showRadarWindow) {
            static Widgets::RadarSettings winRadarSettings;
            static std::vector<Widgets::RadarEntity> winRadarEntities = {
                { 14.0f, 22.0f, 0.0f, 45.0f, true, false, 1.0f },
                { -18.0f, 12.0f, 2.5f, 120.0f, true, false, 0.65f },
                { -8.0f, -25.0f, -1.0f, 280.0f, false, false, 1.0f },
                { 30.0f, -14.0f, 0.0f, 195.0f, true, true, 0.30f }
            };
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 250.0f, 380.0f), ImGuiCond_FirstUseEver);
            Widgets::RadarWindow(&m_showRadarWindow, winRadarSettings, winRadarEntities);
        }

        if (m_showSpectators) {
            std::vector<std::string> specs = { "spectator_bot1", "Admin_04" };
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 230.0f, 48.0f), ImGuiCond_FirstUseEver);
            Widgets::SpectatorList(&m_showSpectators, specs);
        }

        if (m_showKeybinds) {
            std::vector<std::pair<std::string, std::string>> binds = {
                { "Aimbot", "M5 [HOLD]" },
                { "Triggerbot", "ALT [TOGGLE]" },
                { "Thirdperson", "M4 [TOGGLE]" }
            };
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 230.0f, 215.0f), ImGuiCond_FirstUseEver);
            Widgets::KeybindList(&m_showKeybinds, binds);
        }

        // Telemetry Profiler Window
        if (m_showProfiler) {
            Tools::Profiler::Render(&m_showProfiler);
        }

        if (!m_windowOpen) {
            UI::CustomCursor::Get().Render();
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(940, 620), ImGuiCond_FirstUseEver);

        if (Widgets::BeginWindow("Solar Framework Demo", &m_windowOpen, ImVec2(940, 620))) {
            if (m_enableRotatingBorders) {
                ImVec2 wPos = ImGui::GetWindowPos();
                ImVec2 wSize = ImGui::GetWindowSize();
                const auto& pal = ThemeManager::Get().GetPalette();
                m_rotatingBorderConfig.colorA = pal.Accent;
                FX::DrawRotatingBorder(ImGui::GetWindowDrawList(), wPos, ImVec2(wPos.x + wSize.x, wPos.y + wSize.y), 8.0f,
                                       m_rotatingBorderConfig);
            }
            Widgets::RenderTitlebar("SOLAR", "FRAMEWORK  v1.0.1", &m_windowOpen, &m_minimized);

            if (!m_minimized) {
                // Sidebar Navigation
                Widgets::BeginSidebar(205.0f);
                {
                    // Universal Feature Search Filter (Peach-Framework inspired)
                    ImGui::SetCursorPosX(10.0f);
                    ImGui::PushItemWidth(185.0f);
                    if (Widgets::SearchInput("##SidebarSearch", m_searchQuery, sizeof(m_searchQuery), "Search features...")) {
                        if (m_searchQuery[0] != '\0') {
                            std::string q = m_searchQuery;
                            for (auto& c : q) c = (char)tolower((unsigned char)c);
                            if (q.find("aim") != std::string::npos || q.find("fov") != std::string::npos || q.find("smooth") != std::string::npos || q.find("recoil") != std::string::npos || q.find("combat") != std::string::npos) {
                                m_currentTab = 0;
                            } else if (q.find("esp") != std::string::npos || q.find("box") != std::string::npos || q.find("skeleton") != std::string::npos || q.find("cham") != std::string::npos || q.find("visual") != std::string::npos) {
                                m_currentTab = 1;
                            } else if (q.find("radar") != std::string::npos || q.find("hud") != std::string::npos) {
                                m_currentTab = 2;
                            } else if (q.find("widget") != std::string::npos || q.find("slider") != std::string::npos || q.find("toggle") != std::string::npos || q.find("button") != std::string::npos) {
                                m_currentTab = 3;
                            } else if (q.find("spoofer") != std::string::npos || q.find("scan") != std::string::npos || q.find("security") != std::string::npos) {
                                m_currentTab = 4;
                            } else if (q.find("license") != std::string::npos || q.find("key") != std::string::npos) {
                                m_currentTab = 5;
                            } else if (q.find("theme") != std::string::npos || q.find("color") != std::string::npos || q.find("audio") != std::string::npos) {
                                m_currentTab = 6;
                            } else if (q.find("config") != std::string::npos || q.find("profile") != std::string::npos) {
                                m_currentTab = 7;
                            }
                        }
                    }
                    ImGui::PopItemWidth();
                    ImGui::Spacing();

                    Widgets::SidebarCategory("CHEAT ENGINE");
                    if (Widgets::SidebarTab("Combat", IconType::Crosshair, 0, &m_currentTab, 0, ICON_FA_CROSSHAIRS)) PushNavHistory(0);
                    if (Widgets::SidebarTab("Visuals 2.0", IconType::Eye, 1, &m_currentTab, 2, ICON_FA_EYE)) PushNavHistory(1);
                    if (Widgets::SidebarTab("Radar & HUD", IconType::Sliders, 2, &m_currentTab, 0, ICON_FA_EXPAND)) PushNavHistory(2);

                    Widgets::SidebarCategory("UI & WIDGETS");
                    if (Widgets::SidebarTab("Widget Suite", IconType::Sliders, 3, &m_currentTab, 0, ICON_FA_SLIDERS)) PushNavHistory(3);

                    Widgets::SidebarCategory("SECURITY & TOOLS");
                    if (Widgets::SidebarTab("Security Suite", IconType::Shield, 4, &m_currentTab, 0, ICON_FA_SHIELD)) PushNavHistory(4);
                    if (Widgets::SidebarTab("License Screen", IconType::User, 5, &m_currentTab, 0, ICON_FA_LOCK)) PushNavHistory(5);

                    Widgets::SidebarCategory("PREFERENCES");
                    if (Widgets::SidebarTab("Themes & Engine", IconType::Palette, 6, &m_currentTab, 0, ICON_FA_PALETTE)) PushNavHistory(6);
                    if (Widgets::SidebarTab("Profiles", IconType::Folder, 7, &m_currentTab, 0, ICON_FA_FLOPPY_DISK)) PushNavHistory(7);

                    // User Profile at bottom of sidebar (luxury glass chip with interactive profile menu)
                    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 62.0f);
                    ImGui::SetCursorPosX(10.0f);
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    const auto& pal = ThemeManager::Get().GetPalette();
                    ImVec2 userPos = ImGui::GetCursorScreenPos();
                    float userCardW = 185.0f;
                    float userCardH = 46.0f;

                    ImGui::InvisibleButton("##UserProfileBtn", ImVec2(userCardW, userCardH));
                    bool userHovered = ImGui::IsItemHovered();
                    bool userActive = ImGui::IsItemActive();
                    if (ImGui::IsItemClicked()) {
                        Audio::PlayClick();
                        ImGui::OpenPopup("##UserProfilePopup");
                    }

                    u32 cardBg = userActive ? pal.CardHover.ToU32() : (userHovered ? pal.CardHover.WithAlpha(0.85f).ToU32() : pal.Card.WithAlpha(0.70f).ToU32());
                    drawList->AddRectFilled(userPos, ImVec2(userPos.x + userCardW, userPos.y + userCardH), cardBg, 6.0f);
                    Render::ImGuiExt::DrawSpecularEdge(drawList, userPos, ImVec2(userPos.x + userCardW, userPos.y + userCardH),
                                                       IM_COL32(255, 255, 255, userHovered ? 38 : 20), 4.0f, 1.0f);
                    Render::ImGuiExt::AddSmoothBorder(drawList, userPos, ImVec2(userPos.x + userCardW, userPos.y + userCardH),
                                                      (userHovered ? pal.Accent.WithAlpha(0.65f).ToU32() : pal.Border.WithAlpha(0.60f).ToU32()), 6.0f, 1.0f);

                    // User avatar circle
                    ImVec2 avatarC(userPos.x + 22.0f, userPos.y + userCardH * 0.5f);
                    drawList->AddCircleFilled(avatarC, 12.0f, pal.Accent.WithAlpha(0.18f).ToU32(), 20);
                    drawList->AddCircle(avatarC, 12.0f, pal.Accent.WithAlpha(userHovered ? 0.85f : 0.50f).ToU32(), 20, 1.0f);
                    IconRenderer::DrawIcon(drawList, IconType::User, avatarC, 11.0f,
                                          ThemeManager::ToU32(pal.Accent), 1.4f);

                    // Status online dot
                    drawList->AddCircleFilled(ImVec2(avatarC.x + 8.0f, avatarC.y + 8.0f), 3.0f, IM_COL32(35, 215, 95, 255), 10);

                    // Text labels
                    drawList->AddText(ImVec2(userPos.x + 42.0f, userPos.y + 7.0f),
                                      pal.TextPrimary.ToU32(), "SolarDev");
                    drawList->AddText(ImVec2(userPos.x + 42.0f, userPos.y + 23.0f),
                                      pal.Accent.ToU32(), "LIFETIME VIP");

                    // Small indicator chevron on right
                    drawList->AddText(ImVec2(userPos.x + userCardW - 18.0f, userPos.y + 16.0f),
                                      userHovered ? pal.Accent.ToU32() : pal.TextDisabled.ToU32(), ICON_FA_CHEVRON_RIGHT);

                    // Floating User Profile Context Menu
                    ImGui::SetNextWindowPos(ImVec2(userPos.x, userPos.y - 265.0f), ImGuiCond_Always);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
                    ImGui::PushStyleColor(ImGuiCol_PopupBg, pal.Card.WithAlpha(0.98f).ToVec4());
                    ImGui::PushStyleColor(ImGuiCol_Border, pal.Accent.WithAlpha(0.60f).ToVec4());

                    if (ImGui::BeginPopup("##UserProfilePopup")) {
                        ImGui::TextColored(pal.Accent, "SolarDev Account");
                        ImGui::TextColored(pal.TextDisabled, "ID: #SLR-9842 | Tier: VIP");
                        Widgets::Separator();

                        if (Widgets::Button("Command Palette (Ctrl+P)", ImVec2(175.0f, 26.0f), ButtonStyle::Primary)) {
                            UI::ToggleCommandPalette();
                            ImGui::CloseCurrentPopup();
                        }
                        Widgets::Spacing(2.0f);

                        if (Widgets::Button("Test Kill Frag Banner", ImVec2(175.0f, 26.0f), ButtonStyle::Secondary)) {
                            UI::TriggerKillBanner("Jett_Main_99", "VANDAL PRIME", 160, true, 3);
                            ImGui::CloseCurrentPopup();
                        }
                        Widgets::Spacing(2.0f);

                        if (Widgets::Button("Copy HWID", ImVec2(175.0f, 26.0f), ButtonStyle::Secondary)) {
                            ImGui::SetClipboardText("HWID-SOLAR-7F9A-4B21-99CE-DEV");
                            Notify::Success("HWID Copied", "Client hardware identifier copied to clipboard.");
                            ImGui::CloseCurrentPopup();
                        }
                        Widgets::Spacing(2.0f);

                        if (Widgets::Button("Profiles & Presets", ImVec2(175.0f, 26.0f), ButtonStyle::Secondary)) {
                            m_currentTab = 7; // Profiles tab
                            PushNavHistory(7);
                            ImGui::CloseCurrentPopup();
                        }
                        Widgets::Spacing(2.0f);

                        if (Widgets::Button("Themes & Engine", ImVec2(175.0f, 26.0f), ButtonStyle::Secondary)) {
                            m_currentTab = 6; // Themes tab
                            PushNavHistory(6);
                            ImGui::CloseCurrentPopup();
                        }
                        Widgets::Spacing(2.0f);

                        if (Widgets::Button("Sign Out / Lock", ImVec2(175.0f, 26.0f), ButtonStyle::Danger)) {
                            m_currentTab = 5; // License tab
                            PushNavHistory(5);
                            Notify::Warning("Session Locked", "Authorization locked. Please re-enter license key.");
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::EndPopup();
                    }
                    ImGui::PopStyleColor(2);
                    ImGui::PopStyleVar(2);
                }
                Widgets::EndSidebar();

                ImGui::SameLine(0, 10.0f);

                // Main Content View
                ImGui::BeginGroup();
                {
                    float contentWidth = ImGui::GetContentRegionAvail().x - 14.0f;
                    float cardWidth = (contentWidth - 10.0f) * 0.5f;

                    // Navigation Breadcrumbs & History (Peach-Framework inspired)
                    ImGui::BeginGroup();
                    {
                        bool canBack = CanNavBack();
                        bool canFwd = CanNavForward();

                        if (!canBack) ImGui::BeginDisabled();
                        if (ImGui::Button(" < ##NavBack", ImVec2(28, 22))) {
                            NavBack();
                        }
                        if (!canBack) ImGui::EndDisabled();

                        ImGui::SameLine(0, 4.0f);

                        if (!canFwd) ImGui::BeginDisabled();
                        if (ImGui::Button(" > ##NavFwd", ImVec2(28, 22))) {
                            NavForward();
                        }
                        if (!canFwd) ImGui::EndDisabled();

                        ImGui::SameLine(0, 10.0f);
                        const char* tabNames[] = { "Combat & Aimbot", "Studio Visuals 2.0", "Radar & HUD", "Widget Suite", "Security Suite", "License Screen", "Themes & Engine", "Profiles & Config" };
                        const char* activeTabName = (m_currentTab >= 0 && m_currentTab < 8) ? tabNames[m_currentTab] : "Overview";
                        ImGui::AlignTextToFramePadding();
                        ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "Workspace >");
                        ImGui::SameLine(0, 4.0f);
                        ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "%s", activeTabName);
                    }
                    ImGui::EndGroup();
                    Widgets::Spacing(6.0f);

                    // ==========================================
                    // TAB 0: COMBAT & AIMBOT
                    // ==========================================
                    if (m_currentTab == 0) {
                        Widgets::SubTab("General", 0, &m_combatSubTab);
                        Widgets::SubTab("Targeting & Bones", 1, &m_combatSubTab);
                        Widgets::SubTab("Recoil Control", 2, &m_combatSubTab);
                        ImGui::NewLine();
                        Widgets::Spacing(6.0f);

                        // SUBTAB 0: GENERAL AIMBOT & TARGETING
                        if (m_combatSubTab == 0) {
                            if (Widgets::BeginCard("##AimCard", "Target Acquisition", IconType::Crosshair, ImVec2(cardWidth, 490.0f), ICON_FA_CROSSHAIRS)) {
                                if (m_combatPage == 0) {
                                    Widgets::Toggle("Enable Aimbot", &m_aimbotEnabled, "Smooth angle correction towards targets");
                                    Widgets::Toggle("Silent Aim", &m_silentAim, "Snaps bullet vector invisibly");
                                    Widgets::Separator();

                                    Widgets::SliderFloat("Field of View", &m_aimFov, 1.0f, 180.0f, "%.1f", "deg");
                                    Widgets::SliderFloat("Smoothness", &m_aimSmooth, 1.0f, 30.0f, "%.1f", "x");

                                    const char* bones[] = { "Head", "Neck", "Upper Chest", "Stomach", "Nearest Hitbox" };
                                    Widgets::Combo("Preferred Hitbox", &m_targetBone, bones, 5);

                                    Widgets::Separator();
                                    Widgets::Keybind("Aim Hotkey", &m_aimKey, &m_aimKeyMode);
                                } else {
                                    Widgets::Toggle("Auto-Fire / Triggerbot", &m_triggerbot, "Fires when crosshair overlaps hitbox");
                                    Widgets::SliderInt("Trigger Delay", &m_triggerDelay, 0, 250, "%d ms");
                                    static int triggerKeyMode = 0;
                                    Widgets::Keybind("Trigger Key", &m_triggerKey, &triggerKeyMode);
                                    Widgets::Separator();

                                    static bool hitchanceCheck = true;
                                    static float minHitchance = 72.0f;
                                    Widgets::Toggle("Hitchance Validation", &hitchanceCheck, "Validates weapon spread cone");
                                    Widgets::SliderFloat("Min Hitchance", &minHitchance, 10.0f, 100.0f, "%.0f", "%");
                                    
                                    static bool smokeCheck = true;
                                    Widgets::Toggle("Smoke Occlusion Check", &smokeCheck);
                                }

                                RenderCardPagination(&m_combatPage, 2, "CombatGen");
                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##AimDynamicsCard", "Aimbot Dynamics & RCS", IconType::Sliders, ImVec2(cardWidth, 490.0f), ICON_FA_GEAR)) {
                                Widgets::Toggle("Recoil Compensation (RCS)", &m_rcsEnabled);
                                Widgets::SliderFloat("Pitch Comp", &m_rcsPitch, 0.0f, 100.0f, "%.0f", "%");
                                Widgets::SliderFloat("Yaw Comp", &m_rcsYaw, 0.0f, 100.0f, "%.0f", "%");
                                Widgets::Separator();

                                const char* curveTypes[] = { "Linear Interpolation", "Cubic EaseInOut", "Spring Damper", "Humanized Micro-Jitter" };
                                static int curveType = 1;
                                Widgets::Combo("Smoothing Algorithm", &curveType, curveTypes, 4);

                                static float humanizeJitter = 0.35f;
                                Widgets::SliderFloat("Micro-Jitter Factor", &humanizeJitter, 0.0f, 1.0f, "%.2f");

                                static bool dynamicFov = true;
                                Widgets::Toggle("Dynamic FOV Scaling", &dynamicFov, "Scales FOV inversely with target distance");

                                Widgets::Separator();
                                Widgets::Badge("Solar Humanizer v2 Active", ThemeManager::Get().GetPalette().Success);

                                Widgets::EndCard();
                            }
                        }
                        // SUBTAB 1: TARGETING & HITBOXES
                        else if (m_combatSubTab == 1) {
                            if (Widgets::BeginCard("##HitboxMatrixCard", "Hitbox Selection Matrix", IconType::User, ImVec2(cardWidth, 490.0f), ICON_FA_USER)) {
                                ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "Click body zones to toggle targetable bones:");
                                Widgets::Spacing(4.0f);

                                Widgets::HitboxPicker(m_hitboxes, ImVec2(cardWidth - 24.0f, 240.0f));
                                Widgets::Separator();

                                static bool multipoint = true;
                                static float multipointScale = 0.65f;
                                Widgets::Toggle("Multipoint Bone Scanning", &multipoint);
                                Widgets::SliderFloat("Multipoint Scale", &multipointScale, 0.1f, 1.0f, "%.2f");

                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##TargetFilterCard", "Target Filtering & Priority", IconType::Crosshair, ImVec2(cardWidth, 490.0f), ICON_FA_CROSSHAIRS)) {
                                const char* sortModes[] = { "Distance (Nearest First)", "Crosshair Proximity (Lowest FOV)", "Lowest Health", "Maximum Threat" };
                                static int sortMode = 1;
                                Widgets::Combo("Target Sort Priority", &sortMode, sortModes, 4);

                                static bool ignoreTeam = true;
                                static bool ignoreDormant = true;
                                static bool ignoreFlashed = false;
                                static bool headshotOnly = false;
                                Widgets::Toggle("Ignore Teammates", &ignoreTeam);
                                Widgets::Toggle("Ignore Dormant Entities", &ignoreDormant);
                                Widgets::Toggle("Ignore Flashed Targets", &ignoreFlashed);
                                Widgets::Toggle("Headshot Only Priority", &headshotOnly);

                                Widgets::Separator();
                                static bool autowall = true;
                                static float minDamage = 18.0f;
                                Widgets::Toggle("Bullet Penetration (Autowall)", &autowall);
                                Widgets::SliderFloat("Minimum Wallbang DMG", &minDamage, 1.0f, 100.0f, "%.0f", "HP");

                                Widgets::EndCard();
                            }
                        }
                        // SUBTAB 2: RECOIL CONTROL & SPRAY VISUALIZER
                        else if (m_combatSubTab == 2) {
                            if (Widgets::BeginCard("##RecoilEngineCard", "Recoil Pattern Engine", IconType::Crosshair, ImVec2(cardWidth, 490.0f), ICON_FA_CROSSHAIRS)) {
                                const char* weaponList[] = { "AK-47 (7.62x39mm)", "M4A4 (5.56 NATO)", "Vandal (Heavy Rifle)", "Phantom (Suppressed)" };
                                Widgets::Combo("Weapon Profile", &m_recoilWeapon, weaponList, 4);

                                Widgets::SliderInt("Bullet Step", &m_recoilBullet, -1, 30, "%d");
                                Widgets::Toggle("Show Mouse Compensation Curve", &m_showRecoilComp);

                                Widgets::Separator();
                                Widgets::SliderFloat("Stand Pitch Compensation", &m_rcsPitch, 0.0f, 100.0f, "%.0f", "%");
                                Widgets::SliderFloat("Stand Yaw Compensation", &m_rcsYaw, 0.0f, 100.0f, "%.0f", "%");

                                static float crouchFactor = 0.85f;
                                Widgets::SliderFloat("Crouch Compensation Factor", &crouchFactor, 0.1f, 1.0f, "%.2f", "x");

                                Widgets::Separator();
                                if (Widgets::Button("Step Bullet (+1)", ImVec2((cardWidth - 32.0f) * 0.5f, 32.0f), ButtonStyle::Primary)) {
                                    m_recoilBullet = (m_recoilBullet >= 30) ? 0 : m_recoilBullet + 1;
                                    Audio::PlayClick();
                                }
                                ImGui::SameLine(0, 8.0f);
                                if (Widgets::Button("Full Spray View", ImVec2((cardWidth - 32.0f) * 0.5f, 32.0f), ButtonStyle::Secondary)) {
                                    m_recoilBullet = -1;
                                    Audio::PlayClick();
                                }

                                Widgets::Spacing(8.0f);
                                ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "Solar Recoil Engine automatically computes inverted spray deflection vectors.");

                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##RecoilSprayCard", "Ballistic Spray Pattern Visualizer", IconType::Sparkle, ImVec2(cardWidth, 490.0f), ICON_FA_WAND_MAGIC)) {
                                Game::RecoilVisualizer::Render("##RecoilSprayCanvas", ImVec2(cardWidth - 24.0f, 430.0f),
                                                               static_cast<Game::RecoilWeapon>(m_recoilWeapon),
                                                               m_recoilBullet, m_showRecoilComp);
                                Widgets::EndCard();
                            }
                        }
                    }

                    // ==========================================
                    // TAB 1: VISUALS / ESP
                    // ==========================================
                    else if (m_currentTab == 1) {
                        Widgets::SubTab("Player ESP", 0, &m_visualsSubTab);
                        Widgets::SubTab("In-Game Engine & FOV", 1, &m_visualsSubTab);
                        Widgets::SubTab("World & Ballistics", 2, &m_visualsSubTab);
                        Widgets::SubTab("Cosmetics & Skin Changer", 3, &m_visualsSubTab);
                        ImGui::NewLine();
                        Widgets::Spacing(6.0f);

                        // SUBTAB 0: PLAYER ESP
                        if (m_visualsSubTab == 0) {
                            if (Widgets::BeginCard("##ESPSettings", "Visual Components 2.0", IconType::Eye, ImVec2(cardWidth, 490.0f), ICON_FA_EYE)) {
                                if (m_espPage == 0) {
                                    Widgets::Toggle("2D Bounding Box", &m_espSettings.enableBox);
                                    const char* boxTypes[] = { "Full Box", "Corner Box", "Filled Box", "Gradient Box" };
                                    Widgets::Combo("Box Style", &m_espSettings.boxType, boxTypes, 4);
                                    float boxCol[4] = { m_espSettings.boxColor.x, m_espSettings.boxColor.y, m_espSettings.boxColor.z, m_espSettings.boxColor.w };
                                    if (Widgets::ColorPicker("Box Accent", boxCol)) {
                                        m_espSettings.boxColor = ImVec4(boxCol[0], boxCol[1], boxCol[2], boxCol[3]);
                                    }

                                    Widgets::Toggle("3D Oriented Box (OBB)", &m_espSettings.enable3DBox, "8-vertex rotating 3D wireframe box");
                                    Widgets::Toggle("Glow Contour Outline", &m_espSettings.enableGlowOutline, "Multi-pass outer ambient bloom");
                                    Widgets::Toggle("Barrel Line of Sight Ray", &m_espSettings.enableBarrelRay, "Target facing direction vector");
                                    Widgets::Toggle("Acoustic Sound Waves", &m_espSettings.enableAcousticWaves, "Ground radar footsteps expansion");

                                    Widgets::Separator();
                                    Widgets::Toggle("Skeleton Bones", &m_espSettings.enableSkeleton);
                                    float skelCol[4] = { m_espSettings.skeletonColor.x, m_espSettings.skeletonColor.y, m_espSettings.skeletonColor.z, m_espSettings.skeletonColor.w };
                                    if (Widgets::ColorPicker("Skeleton Color", skelCol)) {
                                        m_espSettings.skeletonColor = ImVec4(skelCol[0], skelCol[1], skelCol[2], skelCol[3]);
                                    }

                                    Widgets::Separator();
                                    Widgets::Toggle("Health Bar", &m_espSettings.enableHealthBar);
                                    Widgets::Toggle("Armor Bar", &m_espSettings.enableArmorBar);
                                    Widgets::Toggle("Player Name", &m_espSettings.enableName);
                                    Widgets::Toggle("Active Weapon", &m_espSettings.enableWeapon);
                                    Widgets::Toggle("Distance Meter", &m_espSettings.enableDistance);
                                } else {
                                    static bool ammoBar = true;
                                    static bool headCircle = true;
                                    static bool badgeScoped = true;
                                    static bool badgeFlashed = true;
                                    static bool badgeReloading = true;
                                    static float occludedAlpha = 0.45f;

                                    Widgets::Toggle("Ammo Bar Indicator", &ammoBar);
                                    Widgets::Toggle("Head Position Circle", &headCircle);
                                    Widgets::Separator();
                                    Widgets::Toggle("Scoped Status Badge", &badgeScoped);
                                    Widgets::Toggle("Flashed Status Badge", &badgeFlashed);
                                    Widgets::Toggle("Reloading Status Badge", &badgeReloading);
                                    Widgets::Separator();
                                    Widgets::SliderFloat("Occluded Wall Alpha", &occludedAlpha, 0.1f, 1.0f, "%.2f", "x");
                                    Widgets::Toggle("Offscreen Direction Arrows", &m_offscreenArrows);
                                }

                                RenderCardPagination(&m_espPage, 2, "ESPComps");
                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##ESPPreviewCard", "Studio 3D Mannequin ESP Preview", IconType::Sparkle, ImVec2(cardWidth, 490.0f), ICON_FA_WAND_MAGIC)) {
                                ESPPreview::Render("##LiveMannequin", ImVec2(cardWidth - 24.0f, 305.0f), m_espSettings);

                                Widgets::Separator();
                                std::vector<std::string> stanceItems = { "T-Pose", "Stand", "Crouch", "Scope", "Jump" };
                                if (Widgets::SegmentedControl("Stance Pose", &m_espSettings.stance, stanceItems)) {
                                    Audio::PlayClick();
                                }
                                Widgets::Spacing(2.0f);
                                Widgets::SliderFloat("Simulated HP", &m_espSettings.health, 1.0f, 100.0f, "%.0f", "HP");
                                Widgets::SliderFloat("Simulated Armor", &m_espSettings.armor, 0.0f, 100.0f, "%.0f", "AP");
                                Widgets::SliderFloat("Target Distance", &m_espSettings.distance, 2.0f, 120.0f, "%.1f", "m");

                                Widgets::EndCard();
                            }
                        }
                        // SUBTAB 1: IN-GAME COMBAT OVERLAYS & FOV
                        else if (m_visualsSubTab == 1) {
                            if (Widgets::BeginCard("##CombatEngineCard", "Dynamic FOV Engine & Reticle", IconType::Crosshair, ImVec2(cardWidth, 490.0f), ICON_FA_CROSSHAIRS)) {
                                Game::FOVRenderer::RenderControls(m_fovSettings);
                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##LiveGameSimCard", "Dynamic FOV Vector Preview", IconType::Eye, ImVec2(cardWidth, 490.0f), ICON_FA_CROSSHAIRS)) {
                                Game::FOVRenderer::RenderPreview("##FovPreviewCanvas", ImVec2(cardWidth - 36.0f, 430.0f), m_fovSettings);
                                Widgets::EndCard();
                            }
                        }
                        // SUBTAB 2: WORLD, CHAMS & BALLISTICS TRAJECTORY
                        else if (m_visualsSubTab == 2) {
                            if (Widgets::BeginCard("##ChamsSettingsCard", "World & Ballistics Settings", IconType::Sparkle, ImVec2(cardWidth, 490.0f), ICON_FA_WAND_MAGIC)) {
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "GRENADE BALLISTICS PREDICTOR");
                                const char* projNames[] = { "High Explosive (HE)", "Smoke Grenade", "Flashbang", "Molotov / Incendiary" };
                                Widgets::Combo("Projectile Type", &m_trajectoryType, projNames, 4);
                                Widgets::SliderFloat("Throw Pitch Angle", &m_throwAngle, 10.0f, 85.0f, "%.1f", "deg");
                                Widgets::SliderFloat("Throw Velocity Scale", &m_throwPower, 0.2f, 2.0f, "%.2f", "x");

                                Widgets::Separator();
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "DIRECTX CHAMS MATERIAL SUITE");
                                const char* matNames[] = { "Flat Shaded", "Metallic Gloss", "Glow Outline", "Glass Translucent", "Wireframe Cyber" };
                                Widgets::Combo("Chams Material", &m_chamsMaterial, matNames, 5);
                                Widgets::ColorPicker("Chams Tint Color", m_chamsColor);

                                Widgets::Separator();
                                static bool droppedWeapons = true;
                                static bool c4Timer = true;
                                static bool grenadeTraj = true;
                                Widgets::Toggle("Dropped Weapons ESP", &droppedWeapons);
                                Widgets::Toggle("C4 Planted Timer & Radius", &c4Timer);
                                Widgets::Toggle("Grenade Prediction Trajectory", &grenadeTraj);

                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##PreviewWorldCard", "Interactive Simulation & Preview", IconType::Shield, ImVec2(cardWidth, 490.0f), ICON_FA_SHIELD)) {
                                if (m_worldPage == 0) {
                                    Game::TrajectoryRenderer::Render("##TrajSimCanvas", ImVec2(cardWidth - 24.0f, 380.0f),
                                                                    static_cast<Game::ProjectileType>(m_trajectoryType),
                                                                    m_throwAngle, m_throwPower);
                                } else {
                                    Game::ChamsPreview::Render("##ChamsLiveMannequin", ImVec2(cardWidth - 24.0f, 380.0f),
                                                               static_cast<Game::ChamsMaterial>(m_chamsMaterial),
                                                               Color(m_chamsColor[0], m_chamsColor[1], m_chamsColor[2], m_chamsColor[3]));
                                }

                                RenderCardPagination(&m_worldPage, 2, "WorldSimCard");
                                Widgets::EndCard();
                            }
                        }
                        // SUBTAB 3: COSMETICS & WEAPON SKIN CHANGER
                        else if (m_visualsSubTab == 3) {
                            if (Widgets::BeginCard("##SkinChangerCard", "Cosmetics & Weapon Skin Changer Suite", IconType::Sparkle, ImVec2(contentWidth, 490.0f), ICON_FA_WAND_MAGIC)) {
                                Game::SkinChangerPreview::Render("##SkinChangerMain", ImVec2(contentWidth - 36.0f, 440.0f), m_activeSkin, m_skinInventory);
                                Widgets::EndCard();
                            }
                        }
                    }

                    // ==========================================
                    // TAB 2: RADAR & HUD OVERLAYS
                    // ==========================================
                    else if (m_currentTab == 2) {
                        Widgets::SubTab("Tactical Radar", 0, &m_miscSubTab);
                        Widgets::SubTab("Watermarks & HUD", 1, &m_miscSubTab);
                        ImGui::NewLine();
                        Widgets::Spacing(6.0f);

                        if (m_miscSubTab == 0) {
                            if (Widgets::BeginCard("##RadarCard", "2D Tactical Mini-Radar", IconType::Crosshair, ImVec2(cardWidth, 490.0f), ICON_FA_CROSSHAIRS)) {
                                if (m_radarEntities.empty()) {
                                    m_radarEntities.push_back({ 14.0f, 22.0f, 0.0f, 45.0f, true, false, 1.0f });
                                    m_radarEntities.push_back({ -18.0f, 12.0f, 2.5f, 120.0f, true, false, 0.65f });
                                    m_radarEntities.push_back({ -8.0f, -25.0f, -1.0f, 280.0f, false, false, 1.0f });
                                    m_radarEntities.push_back({ 30.0f, -14.0f, 0.0f, 195.0f, true, true, 0.30f });
                                }
                                Widgets::Radar("##TacticalRadarDisplay", ImVec2(cardWidth - 24.0f, 220.0f), m_radarSettings, m_radarEntities);
                                Widgets::Separator();

                                const char* radarShapes[] = { "Circular Dial", "Rounded Square", "Square Cartesian" };
                                int curShape = static_cast<int>(m_radarSettings.shape);
                                if (Widgets::Combo("Radar Geometry", &curShape, radarShapes, 3)) {
                                    m_radarSettings.shape = static_cast<Widgets::RadarShape>(curShape);
                                }
                                Widgets::SliderFloat("Radar Zoom", &m_radarSettings.zoom, 0.5f, 2.5f, "%.2f", "x");
                                Widgets::SliderFloat("Blip Marker Size", &m_radarSettings.blipSize, 2.0f, 6.0f, "%.1f", "px");
                                Widgets::SliderFloat("Radar Radius", &m_radarSettings.rangeMeters, 15.0f, 120.0f, "%.0f", "m");
                                Widgets::Toggle("Sweep Beam Animation", &m_radarSettings.showSweep, "Continuous rotating phosphorescent sweep");
                                Widgets::Toggle("Directional Heading Cones", &m_radarSettings.showHeadingCones, "Entity orientation vectors");
                                Widgets::Toggle("Tactical Cartesian Grid", &m_radarSettings.showGrid);
                                Widgets::Toggle("Concentric Distance Rings", &m_radarSettings.showRings);

                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##HUDCard", "HUD Elements & Windows", IconType::Bell, ImVec2(cardWidth, 490.0f), ICON_FA_EXPAND)) {
                                Widgets::Toggle("Show External Radar Window", &m_showRadarWindow, "Floating square HUD box with tactical grid & sweep");
                                Widgets::Toggle("Show Watermark Overlay", &m_showWatermark);
                                Widgets::Toggle("Show Spectator List Window", &m_showSpectators);
                                Widgets::Toggle("Show Active Keybinds Window", &m_showKeybinds);
                                Widgets::Toggle("Show Engine Telemetry Profiler", &m_showProfiler);
                                Widgets::Separator();

                                ImGui::TextColored(ThemeManager::Get().GetPalette().TextSecondary, "Combat Frag Telemetry:");
                                Widgets::Spacing(4.0f);

                                if (Widgets::Button("Test Esports Kill Banner (Frag Popup)", ImVec2(0, 34), ButtonStyle::Primary)) {
                                    static int killStreak = 1;
                                    killStreak = (killStreak % 5) + 1;
                                    const char* victims[] = { "Sova_Main_99", "Reyna_Duels", "Jett_Pro_42", "Phoenix_Ace", "Omen_Shadow" };
                                    const char* weapons[] = { "VANDAL PRIME", "OPERATOR DRAGON", "PHANTOM ONI", "SHERIFF REAVER", "VANDAL REAVER" };
                                    UI::TriggerKillBanner(victims[killStreak - 1], weapons[killStreak - 1], 160, true, killStreak);
                                }
                                Widgets::Spacing(6.0f);

                                ImGui::TextColored(ThemeManager::Get().GetPalette().TextSecondary, "Trigger Notification Toasts:");
                                Widgets::Spacing(4.0f);

                                if (Widgets::Button("Post Success Notification", ImVec2(0, 30), ButtonStyle::Secondary)) {
                                    Notify::Success("Solar Framework", "Operation finished successfully!");
                                }
                                Widgets::Spacing(4.0f);

                                if (Widgets::Button("Post Warning Notification", ImVec2(0, 30), ButtonStyle::Secondary)) {
                                    Notify::Warning("Security Alert", "High memory signature detected.");
                                }
                                Widgets::Spacing(4.0f);

                                if (Widgets::Button("Post Error Notification", ImVec2(0, 30), ButtonStyle::Danger)) {
                                    Notify::Error("Hook Failure", "Failed to resolve swapchain pointer.");
                                }

                                Widgets::EndCard();
                            }
                        } else if (m_miscSubTab == 1) {
                            if (Widgets::BeginCard("##HUDWatermarkCard", "HUD Status Watermark (Links & Interactivity)", IconType::Shield, ImVec2(cardWidth, 490.0f), ICON_FA_LINK)) {
                                Widgets::Toggle("Enable HUD Watermark", &m_showWatermark, "Show sleek status pill in viewport");
                                Widgets::Spacing(4.0f);
                                Widgets::InputText("Custom Link URL", m_customLinkInput, sizeof(m_customLinkInput));
                                Widgets::Toggle("Display Link Badge", &m_watermarkInfo.showLink, "Include interactive link in HUD bar");
                                Widgets::Toggle("Display Latency (Ping)", &m_watermarkInfo.showPing);
                                Widgets::Toggle("Display Framerate (FPS)", &m_watermarkInfo.showFps);
                                Widgets::Toggle("Display System Clock", &m_watermarkInfo.showTime);
                                Widgets::Toggle("Display User Profile", &m_watermarkInfo.showUser);
                                Widgets::Toggle("Pulsing Status LED", &m_watermarkInfo.showStatusDot);
                                Widgets::Separator();

                                std::vector<std::string> posOptions = { "Top Right", "Top Left", "Bottom Right", "Bottom Left", "Free Draggable" };
                                Widgets::Combo("Position Anchor", &m_watermarkPosIndex, posOptions);
                                Widgets::Spacing(6.0f);

                                if (Widgets::Button("Test Copy Link to Clipboard", ImVec2(0, 34), ButtonStyle::Secondary)) {
                                    ImGui::SetClipboardText(m_customLinkInput);
                                    Audio::PlayClick();
                                    Notify::Success("Link Copied", (std::string(m_customLinkInput) + " copied to clipboard!").c_str());
                                }

                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##AdvancedScreenWatermarkCard", "Advanced Screen Watermark (Gaming Fonts)", IconType::Crosshair, ImVec2(cardWidth, 490.0f), ICON_FA_GAMEPAD)) {
                                Widgets::Toggle("Enable Screen Watermark", &m_screenWatermark.enabled, "Large subtle overlay across entire screen");
                                Widgets::Spacing(4.0f);
                                Widgets::InputText("Screen Overlay Text", m_screenWatermarkTextInput, sizeof(m_screenWatermarkTextInput));

                                std::vector<std::string> fontOptions = {
                                    "Valorant / CS2 Tactical (DIN)",
                                    "Cyberpunk 2077 Matrix (Consolas)",
                                    "Call of Duty / Warzone (Arial Black)",
                                    "Arcade Strike / Titan (Impact)",
                                    "Apex Legends Esports (Segoe Black)",
                                    "Overwatch Sci-Fi (Corbel Bold)",
                                    "Halo Spartan HUD (Trebuchet Bold)"
                                };
                                Widgets::Combo("Gaming Font Family", &m_screenFontIndex, fontOptions);

                                std::vector<std::string> layoutOptions = {
                                    "Center Diagonal (-25 deg)",
                                    "Center Horizontal",
                                    "Bottom Streamer Banner",
                                    "Top Header Banner",
                                    "Tiled Security Matrix (Anti-Leak)"
                                };
                                Widgets::Combo("Overlay Layout", &m_screenLayoutIndex, layoutOptions);

                                std::vector<std::string> effectOptions = {
                                    "Solid Alpha (Crisp Vector)",
                                    "Outlined Aura Glow"
                                };
                                Widgets::Combo("Render Effect", &m_screenEffectIndex, effectOptions);

                                Widgets::SliderFloat("Watermark Opacity", &m_screenWatermark.opacity, 0.03f, 0.45f, "%.2f");
                                Widgets::SliderFloat("Font Scale Multiplier", &m_screenWatermark.scale, 0.5f, 2.2f, "%.1f", "x");
                                Widgets::Toggle("Match Theme Color", &m_screenWatermark.useThemeColor, "Tint watermark with active theme accent");
                                Widgets::Toggle("Breathing Opacity Pulse", &m_screenWatermark.animatedPulse, "Subtle organic luminance wave");

                                Widgets::EndCard();
                            }
                        }
                    }

                    // ==========================================
                    // TAB 3: UI CONTROLS & WIDGET SUITE
                    // ==========================================
                    else if (m_currentTab == 3) {
                        if (Widgets::BeginCard("##WidgetsCard1", "Advanced Controls Suite", IconType::Sliders, ImVec2(cardWidth, 490.0f), ICON_FA_SLIDERS)) {
                            if (m_widgetsPage == 0) {
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "SEGMENTED PILL CONTROL");
                                std::vector<std::string> segModes = { "Stealth", "Adaptive", "Rage", "Legit" };
                                Widgets::SegmentedControl("Aimbot Mode", &m_segmentedIdx, segModes);

                                Widgets::Separator();
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "DUAL-THUMB RANGE SLIDER");
                                Widgets::RangeSlider("Field of View Range", &m_rangeMin, &m_rangeMax, 0.0f, 120.0f, "%.0f", "deg");

                                Widgets::Separator();
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "NUMERIC STEPPER & SEARCH FILTER");
                                Widgets::NumberStepper("Simulation Tickrate", &m_stepperVal, 16, 256, 16);
                                Widgets::SearchInput("##WidgetSearch", m_searchQuery, sizeof(m_searchQuery), "Search component signatures...");

                                Widgets::Separator();
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "DROPDOWN MULTI-TAG SELECTOR");
                                Widgets::DropdownMultiSelect("Active Visual Shaders", m_multiDropdownSelections, m_multiDropdownItems);
                            } else {
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "INTERACTIVE FILTER CHIPS");
                                Widgets::ChipSelector("Target Filter", m_chipSelections, m_chipItems);

                                Widgets::Separator();
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "240-DEGREE RADIAL GAUGE");
                                Widgets::RadialGauge("Core Compute Load", &m_radialGaugeVal, 0.0f, 100.0f, 36.0f, "%.1f", "%");

                                Widgets::Separator();
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "270-DEGREE ROTARY KNOB");
                                Widgets::KnobSlider("Gain Master", &m_knobVal, 0.0f, 100.0f, 26.0f, "%.0f", "%");
                            }

                            RenderCardPagination(&m_widgetsPage, 2, "WidgetsCard1Page");
                            Widgets::EndCard();
                        }

                        ImGui::SameLine(0, 10.0f);

                        if (Widgets::BeginCard("##WidgetsCard2", "Telemetry Cards & Accordions", IconType::Sparkle, ImVec2(cardWidth, 490.0f), ICON_FA_WAND_MAGIC)) {
                            // Live Telemetry Spline Graph with Gradient Fill
                            Widgets::PerformanceGraph("Pipeline Telemetry Spline", m_fpsSparkline, 16, 130.0f, 195.0f,
                                                     ImVec2(cardWidth - 24.0f, 85.0f), ThemeManager::Get().GetPalette().Accent, "fps");
                            Widgets::Spacing(6.0f);

                            // KPI Stat Cards with sparklines
                            Widgets::StatCard("RENDER PIPELINE FRAMERATE", "185.4 FPS", "+18.2%", true, m_fpsSparkline, 16, cardWidth - 24.0f, 68.0f);
                            Widgets::Spacing(4.0f);
                            Widgets::StatCard("COMPUTE SHADER LATENCY", "0.38 ms", "-24.5%", true, m_latencySparkline, 16, cardWidth - 24.0f, 68.0f);

                            Widgets::Separator();
                            ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "COLLAPSIBLE ACCORDION SECTIONS");

                            if (Widgets::BeginAccordion("DirectX 11 Low-Level Tuning", &m_accordion1Open, "Deferred contexts & dynamic VBO buffering")) {
                                static bool vsync = false;
                                static bool msaa = true;
                                Widgets::Toggle("Force Triple Buffering", &vsync);
                                Widgets::Toggle("4x MSAA Antialiasing", &msaa);
                                Widgets::EndAccordion();
                            }

                            if (Widgets::BeginAccordion("RK4 Motion Dynamics Solver", &m_accordion2Open, "Runge-Kutta 4th order spring integration")) {
                                static float mass = 1.0f;
                                static float stiffness = 180.0f;
                                static float damping = 22.0f;
                                Widgets::SliderFloat("Mass (kg)", &mass, 0.1f, 5.0f, "%.1f");
                                Widgets::SliderFloat("Stiffness (k)", &stiffness, 10.0f, 500.0f, "%.0f");
                                Widgets::SliderFloat("Damping (c)", &damping, 1.0f, 100.0f, "%.0f");
                                Widgets::EndAccordion();
                            }

                            Widgets::Separator();
                            Widgets::Badge("Solar Component Library v1.0.1", ThemeManager::Get().GetPalette().Success);

                            Widgets::EndCard();
                        }
                    }

                    // ==========================================
                    // TAB 4: SECURITY & MEMORY SCANNER
                    // ==========================================
                    else if (m_currentTab == 4) {
                        Widgets::SubTab("HWID Spoofer", 0, &m_securitySubTab);
                        Widgets::SubTab("AOB Memory Scanner", 1, &m_securitySubTab);
                        ImGui::NewLine();
                        Widgets::Spacing(6.0f);

                        if (m_securitySubTab == 0) {
                            if (Widgets::BeginCard("##SpooferMainCard", "Hardware ID Spoofer & Trace Cleaner", IconType::Shield, ImVec2(contentWidth, 490.0f), ICON_FA_FINGERPRINT)) {
                                Widgets::SpooferPanel(m_spooferState);
                                Widgets::EndCard();
                            }
                        } else if (m_securitySubTab == 1) {
                            if (Widgets::BeginCard("##ScannerConfigCard", "AOB Signature Pattern Scanner", IconType::Shield, ImVec2(cardWidth, 490.0f), ICON_FA_SHIELD)) {
                                Widgets::InputText("Target Module", m_patternModule, sizeof(m_patternModule), "e.g. ntdll.dll");
                                Widgets::InputText("Byte Signature", m_patternInput, sizeof(m_patternInput), "e.g. 48 8B 05 ? ? ? ? 48 85 C0");
                                Widgets::Spacing(8.0f);

                                if (Widgets::Button("Inspect Module Bounds", ImVec2(0, 34), ButtonStyle::Secondary)) {
                                    if (Security::PatternScanner::GetModuleBounds(m_patternModule, m_scannedBase, m_scannedSize)) {
                                        Notify::Success("Module Bounds Retrieved", std::string(m_patternModule) + " mapped.");
                                    } else {
                                        Notify::Error("Module Scan Error", "Target module not mapped in current process.");
                                    }
                                    Audio::PlayClick();
                                }
                                Widgets::Spacing(4.0f);

                                if (Widgets::Button("Execute Pattern Scan", ImVec2(0, 36), ButtonStyle::Primary)) {
                                    m_patternResult = Security::PatternScanner::FindPattern(m_patternModule, m_patternInput);
                                    m_hasScanned = true;
                                    if (m_patternResult) {
                                        m_resolvedRelative = Security::PatternScanner::ResolveRelative(m_patternResult, 3, 7);
                                        Notify::Success("Signature Located", "Pattern match located in memory.");
                                    } else {
                                        Notify::Warning("Signature Not Found", "Pattern did not match module byte sequences.");
                                    }
                                    Audio::PlayClick();
                                }

                                Widgets::Separator();
                                ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "IDA-style multi-byte pattern matching with wildcards (? or ??).");
                                ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "Operates with zero runtime heap allocation directly over PE section bounds.");

                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##ScannerResultsCard", "Memory Telemetry & Disassembly", IconType::Sliders, ImVec2(cardWidth, 490.0f), ICON_FA_GEAR)) {
                                ImDrawList* draw = ImGui::GetWindowDrawList();
                                const auto& pal = ThemeManager::Get().GetPalette();

                                ImGui::TextColored(pal.Accent, "TARGET MODULE METRICS");
                                ImGui::Text("Module Name: %s", m_patternModule);
                                if (m_scannedBase != 0) {
                                    ImGui::Text("Base Address: 0x%016llX", (unsigned long long)m_scannedBase);
                                    ImGui::Text("Virtual Size: %zu KB (%.2f MB)", m_scannedSize / 1024, (float)m_scannedSize / (1024.0f * 1024.0f));
                                } else {
                                    ImGui::TextColored(pal.TextDisabled, "Base Address: [Click 'Inspect Module Bounds']");
                                }

                                Widgets::Separator();
                                ImGui::TextColored(pal.Accent, "PATTERN SCAN RESULT");
                                if (!m_hasScanned) {
                                    ImGui::TextColored(pal.TextDisabled, "Status: Scan idle. Click 'Execute Pattern Scan'.");
                                } else if (m_patternResult != 0) {
                                    ImGui::TextColored(pal.Success, "MATCH FOUND: 0x%016llX", (unsigned long long)m_patternResult);
                                    if (m_resolvedRelative != 0) {
                                        ImGui::TextColored(pal.Accent, "Resolved Relative (disp32): 0x%016llX", (unsigned long long)m_resolvedRelative);
                                    }

                                    Widgets::Spacing(6.0f);
                                    ImGui::TextColored(pal.TextSecondary, "Hex Memory Inspector [Match + 0x00]:");

                                    ImVec2 dumpPos = ImGui::GetCursorScreenPos();
                                    float dumpW = cardWidth - 24.0f;
                                    float dumpH = 75.0f;
                                    draw->AddRectFilled(dumpPos, ImVec2(dumpPos.x + dumpW, dumpPos.y + dumpH), IM_COL32(10, 11, 15, 255), 4.0f);
                                    draw->AddRect(dumpPos, ImVec2(dumpPos.x + dumpW, dumpPos.y + dumpH), IM_COL32(255, 255, 255, 20), 4.0f);

                                    ImGui::SetCursorScreenPos(ImVec2(dumpPos.x + 8.0f, dumpPos.y + 8.0f));
                                    ImGui::BeginGroup();
                                    const u8* bytes = reinterpret_cast<const u8*>(m_patternResult);
                                    char line1[64], line2[64];
                                    snprintf(line1, sizeof(line1), "+00: %02X %02X %02X %02X %02X %02X %02X %02X",
                                             bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7]);
                                    snprintf(line2, sizeof(line2), "+08: %02X %02X %02X %02X %02X %02X %02X %02X",
                                             bytes[8], bytes[9], bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);
                                    ImGui::TextColored(pal.Accent, "%s", line1);
                                    ImGui::TextColored(pal.TextPrimary, "%s", line2);
                                    ImGui::EndGroup();
                                    ImGui::SetCursorScreenPos(ImVec2(dumpPos.x, dumpPos.y + dumpH + 8.0f));
                                } else {
                                    ImGui::TextColored(pal.Danger, "Status: Pattern NOT found in module address space.");
                                }

                                Widgets::Separator();
                                Widgets::Badge("AVX2 Hardware-Accelerated Scanner", pal.Accent);

                                Widgets::EndCard();
                            }
                        }
                    }

                    // ==========================================
                    // TAB 5: LOADER & LICENSE SCREEN
                    // ==========================================
                    else if (m_currentTab == 5) {
                        if (Widgets::BeginCard("##LicenseCard", "Loader Authentication & Entitlement", IconType::User, ImVec2(contentWidth, 490.0f), ICON_FA_LOCK)) {
                            const char* hwid = "8F91-AA02-CC49-0012-98EF";
                            Widgets::LicenseScreen(m_licenseKey, sizeof(m_licenseKey), hwid, &m_rememberMe, &m_loggedIn);

                            if (m_loggedIn) {
                                Widgets::Spacing(8.0f);
                                Widgets::Badge("AUTHENTICATED: FULL ACCESS GRANTED", ThemeManager::Get().GetPalette().Success);
                            }

                            Widgets::EndCard();
                        }
                    }

                    // ==========================================
                    // TAB 6: THEMES & AUDIO
                    // ==========================================
                    else if (m_currentTab == 6) {
                        Widgets::SubTab("Color Presets", 0, &m_themeSubTab);
                        Widgets::SubTab("Audio & FX", 1, &m_themeSubTab);
                        Widgets::SubTab("Rotating Borders", 2, &m_themeSubTab);
                        Widgets::SubTab("Satellite Windows", 3, &m_themeSubTab);
                        Widgets::SubTab("Modded Engine", 4, &m_themeSubTab);
                        ImGui::NewLine();
                        Widgets::Spacing(6.0f);

                        // SUBTAB 0: COLOR PRESETS
                        if (m_themeSubTab == 0) {
                            if (Widgets::BeginCard("##ThemesList", "Color Presets (Pro Engineering & Esports)", IconType::Palette, ImVec2(cardWidth, 490.0f), ICON_FA_PALETTE)) {
                                ImGui::BeginChild("##ThemesScrollArea", ImVec2(0, 420.0f), false, ImGuiWindowFlags_NoBackground);
                                auto allPresets = Presets::GetAll();
                                ThemePreset activePreset = ThemeManager::Get().GetCurrentPreset();
                                for (const auto& pi : allPresets) {
                                    bool isActive = (pi.preset == activePreset);
                                    char label[128];
                                    snprintf(label, sizeof(label), "%s%s", pi.name.c_str(), isActive ? "  [ACTIVE]" : "");
                                    if (Widgets::Button(label, ImVec2(0, 35), isActive ? ButtonStyle::Primary : ButtonStyle::Secondary)) {
                                        ThemeManager::Get().ApplyPreset(pi.preset);
                                        Notify::Success("Theme Applied", (std::string("Switched to ") + pi.name + " theme.").c_str());
                                    }
                                    if (ImGui::IsItemHovered()) {
                                        ImGui::SetTooltip("%s", pi.description.c_str());
                                    }
                                    Widgets::Spacing(3.0f);
                                }
                                ImGui::EndChild();
                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##CustomColorTuning", "Custom Palette & Accent Calibration", IconType::Sliders, ImVec2(cardWidth, 490.0f), ICON_FA_GEAR)) {
                                ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "Live real-time dual-gradient RGB accent tuner:");
                                Widgets::Spacing(6.0f);

                                if (Widgets::ColorPicker("Custom Accent RGB", m_customColor)) {
                                    ThemeManager::Get().SetCustomAccent(
                                        Color(m_customColor[0], m_customColor[1], m_customColor[2], 1.0f),
                                        Color(m_customColor[0] * 1.2f, m_customColor[1] * 1.2f, m_customColor[2] * 1.2f, 1.0f)
                                    );
                                }
                                Widgets::Spacing(4.0f);

                                bool rainbow = ThemeManager::Get().IsRainbowMode();
                                if (Widgets::Toggle("Rainbow Chroma Mode", &rainbow, "Smooth spectrum RGB hue cycling")) {
                                    ThemeManager::Get().SetRainbowMode(rainbow);
                                }
                                if (rainbow) {
                                    float speed = ThemeManager::Get().GetRainbowSpeed();
                                    if (Widgets::SliderFloat("Rainbow Speed", &speed, 0.2f, 4.0f, "%.1f", "x")) {
                                        ThemeManager::Get().SetRainbowSpeed(speed);
                                    }
                                }

                                Widgets::Separator();
                                ImDrawList* draw = ImGui::GetWindowDrawList();
                                ImVec2 swatchPos = ImGui::GetCursorScreenPos();
                                float swatchW = cardWidth - 24.0f;
                                float swatchH = 45.0f;
                                const auto& pal = ThemeManager::Get().GetPalette();
                                draw->AddRectFilledMultiColor(swatchPos, ImVec2(swatchPos.x + swatchW, swatchPos.y + swatchH),
                                                             ThemeManager::ToU32(pal.Accent), ThemeManager::ToU32(pal.AccentHover),
                                                             ThemeManager::ToU32(pal.AccentHover), ThemeManager::ToU32(pal.Accent));
                                draw->AddRect(swatchPos, ImVec2(swatchPos.x + swatchW, swatchPos.y + swatchH), IM_COL32(255, 255, 255, 40), 4.0f);
                                ImGui::Dummy(ImVec2(0, swatchH + 12.0f));

                                Widgets::Badge("Live Procedural Gradient Shader Active", pal.Accent);
                                Widgets::EndCard();
                            }
                        }
                        // SUBTAB 1: AUDIO & FX
                        else if (m_themeSubTab == 1) {
                            if (Widgets::BeginCard("##AudioSuiteCard", "Procedural Audio Engine", IconType::Sparkle, ImVec2(cardWidth, 490.0f), ICON_FA_VOLUME_HIGH)) {
                                auto& style = ThemeManager::Get().GetStyle();

                                Widgets::Toggle("Enable Audio Feedback", &style.EnableAudio, "Procedural wave synthesis (zero audio files on disk)");
                                Widgets::Spacing(4.0f);

                                if (Widgets::Button("Mechanical Click Sound", ImVec2(0, 32), ButtonStyle::Secondary)) {
                                    Audio::PlayClick();
                                }
                                if (Widgets::Button("Toggle Switch Tone", ImVec2(0, 32), ButtonStyle::Secondary)) {
                                    Audio::PlayToggle(true);
                                }
                                if (Widgets::Button("Harmonic Chime Toast", ImVec2(0, 32), ButtonStyle::Secondary)) {
                                    Audio::PlayNotification();
                                }
                                if (Widgets::Button("Critical Error Tone", ImVec2(0, 32), ButtonStyle::Danger)) {
                                    Audio::PlayError();
                                }

                                Widgets::Separator();
                                ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "Audio samples are synthesized in-memory via mathematical sinusoids and envelope shaping.");

                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##FXSuiteCard", "Visual Effects & Particle Physics", IconType::Sparkle, ImVec2(cardWidth, 490.0f), ICON_FA_WAND_MAGIC)) {
                                auto& style = ThemeManager::Get().GetStyle();

                                Widgets::Toggle("Enable Glow Effects", &style.EnableGlow);
                                Widgets::SliderFloat("Glow Intensity", &style.GlowIntensity, 0.2f, 2.0f, "%.1f", "x");
                                
                                const char* particleModes[] = { "Falling Cyber Snow", "Rising Amber Embers", "Star Constellations", "Disabled" };
                                static int currentParticleMode = static_cast<int>(FX::ParticleSystem::Get().GetMode());
                                if (Widgets::Combo("Background FX Mode", &currentParticleMode, particleModes, 4)) {
                                    FX::ParticleSystem::Get().SetMode(static_cast<FX::ParticleEffectMode>(currentParticleMode));
                                }
                                Widgets::SliderInt("Particle Count", &style.ParticleCount, 10, 100, "%d");

                                Widgets::Separator();
                                bool cursorEnabled = UI::CustomCursor::Get().IsEnabled();
                                if (Widgets::Toggle("Cyber Glowing Cursor", &cursorEnabled, "Theme-reactive neon core with trailing ghost and click ripple")) {
                                    UI::CustomCursor::Get().SetEnabled(cursorEnabled);
                                }
                                const char* cursorStyles[] = { "Cyber Arrow", "Crosshair Dot", "Cyber Dot", "Precision Triangle", "Minimal Ring" };
                                int curStyle = static_cast<int>(UI::CustomCursor::Get().GetStyle());
                                if (Widgets::Combo("Cursor Visual Style", &curStyle, cursorStyles, 5)) {
                                    UI::CustomCursor::Get().SetStyle(static_cast<UI::CursorStyle>(curStyle));
                                }
                                const char* fpsOptions[] = { "VSync (Monitor Synchronized)", "30 FPS (Power Saver)", "60 FPS (Standard 60Hz)", "120 FPS (High Performance)", "144 FPS (Esports 144Hz)", "240 FPS (Ultra Smooth)", "Uncapped (Maximum Throttle)" };
                                static int curFpsIdx = 0;
                                if (Widgets::Combo("Menu FPS Limiter", &curFpsIdx, fpsOptions, 7)) {
                                    if (curFpsIdx == 0) m_fpsCap = 0;
                                    else if (curFpsIdx == 1) m_fpsCap = 30;
                                    else if (curFpsIdx == 2) m_fpsCap = 60;
                                    else if (curFpsIdx == 3) m_fpsCap = 120;
                                    else if (curFpsIdx == 4) m_fpsCap = 144;
                                    else if (curFpsIdx == 5) m_fpsCap = 240;
                                    else if (curFpsIdx == 6) m_fpsCap = -1;
                                }
                                Widgets::Toggle("Rotating Glowing Borders", &m_enableRotatingBorders);
                                if (Widgets::Button("Launch Luxury Welcome Screen", ImVec2(0, 34), ButtonStyle::Primary)) {
                                    UI::WelcomeScreen::Get().Show();
                                }
                                Widgets::Spacing(4.0f);
                                if (Widgets::Button("Replay Splash Loading Screen", ImVec2(0, 34), ButtonStyle::Secondary)) {
                                    UI::SplashScreen::Get().Start(2.8f);
                                }

                                Widgets::EndCard();
                            }
                        }
                        // SUBTAB 2: CONFIGURABLE ROTATING BORDERS
                        else if (m_themeSubTab == 2) {
                            if (Widgets::BeginCard("##RotatingBorderStudio", "Rotating Glowing Border Engine", IconType::Sparkle, ImVec2(cardWidth, 490.0f), ICON_FA_WAND_MAGIC)) {
                                Widgets::Toggle("Enable Rotating Border", &m_enableRotatingBorders, "Parametric GPU multi-pass neon aura");
                                Widgets::Spacing(6.0f);

                                const char* modes[] = { "Two-Color Lerp Wave", "Rainbow 360° Spectrum", "Neon Energy Comet", "Cyber Tri-Gradient", "Dual Opposing Orbit" };
                                int curMode = static_cast<int>(m_rotatingBorderConfig.mode);
                                if (Widgets::Combo("Rotation Algorithm", &curMode, modes, 5)) {
                                    m_rotatingBorderConfig.mode = static_cast<FX::BorderRotationMode>(curMode);
                                }

                                Widgets::SliderFloat("Rotation Speed", &m_rotatingBorderConfig.speed, 0.2f, 4.0f, "%.1f", "x");
                                Widgets::SliderFloat("Border Thickness", &m_rotatingBorderConfig.thickness, 1.0f, 5.0f, "%.1f", "px");
                                Widgets::SliderFloat("Glow Multiplier", &m_rotatingBorderConfig.glowIntensity, 0.0f, 3.0f, "%.1f", "x");
                                Widgets::SliderFloat("Comet Tail Length", &m_rotatingBorderConfig.trailLength, 0.1f, 1.0f, "%.2f");
                                Widgets::Toggle("Clockwise Direction", &m_rotatingBorderConfig.clockwise);
                                Widgets::SliderInt("Gaussian Passes", &m_rotatingBorderConfig.glowPasses, 1, 5, "%d");

                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##BorderColorPalette", "Secondary Neon Color Calibration", IconType::Palette, ImVec2(cardWidth, 490.0f), ICON_FA_PALETTE)) {
                                static float secColor[4] = { 0.70f, 0.20f, 1.00f, 0.95f };
                                if (Widgets::ColorPicker("Secondary Neon Glow", secColor)) {
                                    m_rotatingBorderConfig.colorB = Color(secColor[0], secColor[1], secColor[2], secColor[3]);
                                }

                                Widgets::Separator();
                                ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "The rotating border wraps seamlessly around rounded corners with 0.1ms compute cost and zero heap allocation.");
                                Widgets::EndCard();
                            }
                        }
                        // SUBTAB 3: MODULAR SATELLITE WINDOWS & DOCK
                        else if (m_themeSubTab == 3) {
                            if (Widgets::BeginCard("##SatelliteDockCard", "Modular Detached Satellite Windows", IconType::Sliders, ImVec2(cardWidth, 490.0f), ICON_FA_EXPAND)) {
                                Widgets::Toggle("Floating Keybinds Satellite", &m_showSatelliteKeybinds, "Detached floating card attached to GUI");
                                Widgets::Toggle("Floating Session Telemetry", &m_showSatelliteSpectators, "Secondary auxiliary floating card");
                                Widgets::Spacing(6.0f);

                                const char* anchors[] = { "Free Floating", "Dock Right", "Dock Left", "Dock Top", "Dock Bottom", "Dock Top Right", "Dock Bottom Right" };
                                int anchorIdx = static_cast<int>(m_satelliteKeybindsConfig.anchor);
                                if (Widgets::Combo("Keybinds Dock Side", &anchorIdx, anchors, 7)) {
                                    m_satelliteKeybindsConfig.anchor = static_cast<UI::SatelliteAnchor>(anchorIdx);
                                    if (m_satelliteKeybindsConfig.anchor != UI::SatelliteAnchor::FreeFloating) {
                                        m_satelliteKeybindsConfig.isPinned = true;
                                    }
                                }

                                Widgets::SliderFloat("Separation Gap", &m_satelliteKeybindsConfig.offsetGap, 4.0f, 40.0f, "%.0f", "px");
                                Widgets::Toggle("Magnetic Snap to Parent", &m_satelliteKeybindsConfig.magneticSnap, "Snaps back automatically when dragged near GUI");
                                Widgets::SliderFloat("Snap Radius", &m_satelliteKeybindsConfig.snapThreshold, 15.0f, 75.0f, "%.0f", "px");
                                Widgets::Toggle("Smooth Spring Lag Physics", &m_satelliteKeybindsConfig.smoothSpring, "Fluid trailing motion when dragging GUI");
                                Widgets::Toggle("Neon Connector Beam", &m_satelliteKeybindsConfig.drawConnectorBeam, "Glowing energy bracket connecting parent to satellite");
                                Widgets::Toggle("Rotating Border on Satellite", &m_satelliteKeybindsConfig.enableRotatingBorder);

                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##SatelliteArchCard", "Satellite Architecture Overview", IconType::Shield, ImVec2(cardWidth, 490.0f), ICON_FA_SHIELD)) {
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "ATTACHED-YET-DETACHED PARADIGM");
                                ImGui::Spacing();
                                ImGui::TextWrapped("Satellite windows are rendered as isolated DirectX 11 draw calls with independent drop shadows and glass cards. They lock magnetically to the host window edges and can be pinned or unpinned on the fly with the padlock icon.");

                                Widgets::Separator();
                                if (Widgets::Button("Reset Satellites to Dock", ImVec2(0, 36), ButtonStyle::Secondary)) {
                                    m_satelliteKeybindsConfig.isPinned = true;
                                    m_satelliteKeybindsConfig.anchor = UI::SatelliteAnchor::DockRight;
                                    m_satelliteKeybindsConfig.currentPos = ImVec2(-1, -1);
                                    m_satelliteSpectatorsConfig.isPinned = true;
                                    m_satelliteSpectatorsConfig.anchor = UI::SatelliteAnchor::DockLeft;
                                    m_satelliteSpectatorsConfig.currentPos = ImVec2(-1, -1);
                                    Notify::Success("Satellites Reset", "Magnetic dock restored to default positions.");
                                }

                                Widgets::EndCard();
                            }
                        }
                        // SUBTAB 4: MODDED IMGUI ENGINE (IMGUIEXT)
                        else if (m_themeSubTab == 4) {
                            if (Widgets::BeginCard("##ImGuiExtPrimitives", "Modded ImGui Custom Primitives", IconType::Sparkle, ImVec2(cardWidth, 490.0f), ICON_FA_WAND_MAGIC)) {
                                ImDrawList* draw = ImGui::GetWindowDrawList();
                                ImVec2 canvasPos = ImGui::GetCursorScreenPos();
                                ImVec2 canvasSize(cardWidth - 24.0f, 340.0f);
                                ImGui::InvisibleButton("##ExtCanvas", canvasSize);

                                draw->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(11, 12, 16, 255), 6.0f);
                                draw->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(255, 255, 255, 18), 6.0f);

                                // Showcase AddPill
                                Render::ImGuiExt::AddPill(draw, ImVec2(canvasPos.x + 20, canvasPos.y + 25), ImVec2(canvasPos.x + 190, canvasPos.y + 60),
                                                         ThemeManager::ToU32(Color(0.2f, 0.6f, 1.0f, 0.4f)),
                                                         ThemeManager::ToU32(Color(0.2f, 0.7f, 1.0f, 1.0f)), 1.5f);
                                draw->AddText(ImVec2(canvasPos.x + 40, canvasPos.y + 35), IM_COL32(255, 255, 255, 240), "AddPill Primitive");

                                // Showcase AddGlowCapsule
                                Render::ImGuiExt::AddGlowCapsule(draw, ImVec2(canvasPos.x + 35, canvasPos.y + 105), ImVec2(canvasPos.x + 185, canvasPos.y + 105),
                                                                 16.0f, Color(1.0f, 0.5f, 0.1f, 0.85f), 12.0f);
                                draw->AddText(ImVec2(canvasPos.x + 48, canvasPos.y + 97), IM_COL32(255, 255, 255, 240), "AddGlowCapsule");

                                // Showcase AddConicGradientRect
                                static float conicAngle = 0.0f;
                                conicAngle += ImGui::GetIO().DeltaTime * 1.5f;
                                Render::ImGuiExt::AddConicGradientRect(draw, ImVec2(canvasPos.x + 20, canvasPos.y + 155), ImVec2(canvasPos.x + 190, canvasPos.y + 285),
                                                                      8.0f, Color(1.0f, 0.2f, 0.5f, 0.8f), Color(0.2f, 0.8f, 1.0f, 0.8f), conicAngle, 1.5f);
                                draw->AddText(ImVec2(canvasPos.x + 30, canvasPos.y + 215), IM_COL32(255, 255, 255, 240), "AddConicGradientRect");

                                Widgets::Separator();
                                Widgets::Badge("ImGui Extended Render Pipeline", ThemeManager::Get().GetPalette().Accent);

                                Widgets::EndCard();
                            }

                            ImGui::SameLine(0, 10.0f);

                            if (Widgets::BeginCard("##ImGuiExtSafety", "Engine Stability & Safety", IconType::Shield, ImVec2(cardWidth, 490.0f), ICON_FA_SHIELD)) {
                                ImDrawList* draw = ImGui::GetWindowDrawList();
                                Render::ImGuiExt::ReserveDrawlistBuffers(draw, 512, 1024);

                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "ZERO-CRASH BUFFER ARCHITECTURE");
                                ImGui::Spacing();
                                ImGui::TextWrapped("ReserveDrawlistBuffers pre-allocates contiguous memory inside ImDrawList VtxBuffer and IdxBuffer, eliminating runtime memory reallocation crashes and heap fragmentation during heavy ESP frames.");

                                Widgets::Separator();
                                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "SAFE STRING FORMATTING");
                                ImGui::Spacing();
                                ImGui::TextWrapped("SafeText wrappers prevent format-string exploits and buffer overflow conditions with strict bounds checking and automatic truncation.");

                                Widgets::Spacing(8.0f);
                                Render::ImGuiExt::SafeText(draw, ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y),
                                                          ThemeManager::ToU32(ThemeManager::Get().GetPalette().Success),
                                                          "SafeText Validated: Zero Heap Vulnerabilities");
                                ImGui::Dummy(ImVec2(0, 24.0f));

                                Widgets::Separator();
                                ImGui::Text("Active Vertex Count: %d", draw->VtxBuffer.Size);
                                ImGui::Text("Active Index Count: %d", draw->IdxBuffer.Size);

                                Widgets::EndCard();
                            }
                        }
                    }

                    // ==========================================
                    // TAB 7: CONFIG PROFILES
                    // ==========================================
                    else if (m_currentTab == 7) {
                        if (Widgets::BeginCard("##ConfigListCard", "Configurations & Presets", IconType::Folder, ImVec2(cardWidth, 490.0f), ICON_FA_FLOPPY_DISK)) {
                            const auto& presets = ConfigManager::Get().GetSavedPresetNames();
                            for (size_t i = 0; i < presets.size(); i++) {
                                bool selected = (m_selectedConfigIndex == static_cast<int>(i));
                                ImGui::PushID(static_cast<int>(i));
                                if (ImGui::Selectable(presets[i].c_str(), selected, 0, ImVec2(0, 32))) {
                                    m_selectedConfigIndex = static_cast<int>(i);
                                    Audio::PlayClick();
                                }
                                ImGui::PopID();
                            }

                            Widgets::Separator();
                            Widgets::InputText("##NewCfgName", m_configNameInput, sizeof(m_configNameInput), "New preset name...");
                            if (Widgets::Button("Create Profile", ImVec2(0, 34), ButtonStyle::Secondary)) {
                                Notify::Success("Profile Created", std::string(m_configNameInput) + ".cfg created.");
                            }

                            Widgets::EndCard();
                        }

                        ImGui::SameLine(0, 10.0f);

                        if (Widgets::BeginCard("##ConfigOpsCard", "Profile Operations", IconType::Sliders, ImVec2(cardWidth, 490.0f), ICON_FA_GEAR)) {
                            if (Widgets::Button("Load Selected Profile", ImVec2(0, 38), ButtonStyle::Primary)) {
                                Notify::Success("Profile Loaded", "Profile settings restored successfully!");
                            }
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Save Current Settings", ImVec2(0, 38), ButtonStyle::Secondary)) {
                                Notify::Success("Profile Saved", "Saved to local configuration storage.");
                            }
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Export to Clipboard", ImVec2(0, 38), ButtonStyle::Secondary)) {
                                Notify::Info("Export", "Profile data copied to clipboard.");
                            }
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Import from Clipboard", ImVec2(0, 38), ButtonStyle::Secondary)) {
                                Notify::Info("Import", "Profile data parsed from clipboard.");
                            }
                            Widgets::Spacing(12.0f);

                            if (Widgets::Button("Delete Profile", ImVec2(0, 38), ButtonStyle::Danger)) {
                                Notify::Warning("Profile Deleted", "Profile removed from disk.");
                            }

                            Widgets::EndCard();
                        }
                    }

                }
                ImGui::EndGroup();
            }
        }
        Widgets::EndWindow();

        // ==============================================================================
        // Modular Satellite Windows (Detached Floating Panels Attached to Main Window)
        // ==============================================================================
        if (m_windowOpen && !m_minimized) {
            // Satellite 1: Active Keybinds HUD
            if (m_showSatelliteKeybinds) {
                if (UI::BeginSatellite("##SatelliteKeybinds", "Active Keybinds", ImVec2(215, 175),
                                       m_satelliteKeybindsConfig, &m_showSatelliteKeybinds, "Solar Framework Demo")) {
                    const auto& pal = ThemeManager::Get().GetPalette();
                    auto renderKeyRow = [&](const char* name, const char* key, bool active) {
                        ImGui::TextColored(active ? pal.TextPrimary : pal.TextDisabled, "%s", name);
                        ImGui::SameLine(ImGui::GetWindowWidth() - 65.0f);
                        ImGui::TextColored(active ? pal.Accent : pal.TextDisabled, "[%s]", key);
                    };
                    renderKeyRow("Aimbot Assist", "M5", m_aimbotEnabled);
                    renderKeyRow("Silent Aim", "CAPS", m_silentAim);
                    renderKeyRow("Triggerbot", "ALT", m_triggerbot);
                    renderKeyRow("Tactical Radar", "F1", m_showRadarWindow);
                    renderKeyRow("ESP Visuals", "INS", m_espSettings.enableBox);
                    UI::EndSatellite();
                }
            }

            // Satellite 2: Session & Telemetry HUD
            if (m_showSatelliteSpectators) {
                if (UI::BeginSatellite("##SatelliteSession", "Session Telemetry", ImVec2(215, 145),
                                       m_satelliteSpectatorsConfig, &m_showSatelliteSpectators, "Solar Framework Demo")) {
                    const auto& pal = ThemeManager::Get().GetPalette();
                    ImGui::TextColored(pal.TextSecondary, "Framerate:");
                    ImGui::SameLine(120.0f);
                    ImGui::TextColored(pal.Accent, "%.0f FPS", ImGui::GetIO().Framerate);

                    ImGui::TextColored(pal.TextSecondary, "GPU Latency:");
                    ImGui::SameLine(120.0f);
                    ImGui::TextColored(Color(0.2f, 0.9f, 0.4f, 1.0f), "1.8 ms");

                    ImGui::TextColored(pal.TextSecondary, "Spectators:");
                    ImGui::SameLine(120.0f);
                    ImGui::TextColored(Color(1.0f, 0.4f, 0.2f, 1.0f), "2 Watching");

                    ImGui::TextColored(pal.TextSecondary, "State:");
                    ImGui::SameLine(120.0f);
                    ImGui::TextColored(pal.Accent, "Synchronized");
                    UI::EndSatellite();
                }
            }
        }

        // Tactical Radar External HUD Window
        if (m_showRadarWindow) {
            Game::RadarWindow::Render(&m_showRadarWindow, m_radarSettings, m_radarEntities);
        }

        // Esports Kill Frag Banner Notification Popup
        UI::KillBanner::Get().Render();

        // High-Tech Quick Command Palette (Ctrl + P)
        UI::CommandPalette::Get().Render();

        UI::CustomCursor::Get().Render();
    }

} // namespace Solar
