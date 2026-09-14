#include "demo_app.hpp"
#include <cstdio>

namespace Solar {

    DemoApp& DemoApp::Get() {
        static DemoApp instance;
        return instance;
    }

    void DemoApp::Initialize() {
        m_espSettings.playerName = "Enemy_01";
        m_espSettings.weaponName = "Vandal [25/75]";
        m_espSettings.distance = 28.5f;
        m_espSettings.health = 85.0f;
        m_espSettings.armor = 60.0f;
    }

    void DemoApp::Render() {
        // Luxury Animated Splash / Loading Screen
        if (UI::SplashScreen::Get().Render()) {
            return;
        }

        // Floating Watermark HUD
        if (m_showWatermark) {
            WatermarkInfo wm;
            wm.frameworkName = "SOLAR";
            wm.version = "v1.0.1";
            wm.username = "SolarDev";
            wm.pingMs = 12;
            Watermark::Render(wm);
        }

        // Floating HUD Windows: Spectators & Keybinds
        if (m_showSpectators) {
            std::vector<std::string> specs = { "spectator_bot1", "Admin_04" };
            Widgets::SpectatorList(&m_showSpectators, specs);
        }

        if (m_showKeybinds) {
            std::vector<std::pair<std::string, std::string>> binds = {
                { "Aimbot", "M5 [HOLD]" },
                { "Triggerbot", "ALT [TOGGLE]" },
                { "Thirdperson", "M4 [TOGGLE]" }
            };
            Widgets::KeybindList(&m_showKeybinds, binds);
        }

        // Telemetry Profiler Window
        if (m_showProfiler) {
            Tools::Profiler::Render(&m_showProfiler);
        }

        if (!m_windowOpen) return;

        ImGui::SetNextWindowSize(ImVec2(940, 620), ImGuiCond_FirstUseEver);

        if (Widgets::BeginWindow("Solar Framework Demo", &m_windowOpen, ImVec2(940, 620))) {
            Widgets::RenderTitlebar("SOLAR", "FRAMEWORK  v1.0.1", &m_windowOpen, &m_minimized);

            if (!m_minimized) {
                // Sidebar Navigation
                Widgets::BeginSidebar(205.0f);
                {
                    Widgets::SidebarCategory("CHEAT ENGINE");
                    Widgets::SidebarTab("Combat", IconType::Crosshair, 0, &m_currentTab, 0, ICON_FA_CROSSHAIRS);
                    Widgets::SidebarTab("Visuals", IconType::Eye, 1, &m_currentTab, 2, ICON_FA_EYE);
                    Widgets::SidebarTab("Radar & HUD", IconType::Sliders, 2, &m_currentTab, 0, ICON_FA_EXPAND);

                    Widgets::SidebarCategory("SECURITY & TOOLS");
                    Widgets::SidebarTab("HWID Spoofer", IconType::Shield, 3, &m_currentTab, 0, ICON_FA_FINGERPRINT);
                    Widgets::SidebarTab("License Screen", IconType::User, 4, &m_currentTab, 0, ICON_FA_LOCK);

                    Widgets::SidebarCategory("PREFERENCES");
                    Widgets::SidebarTab("Themes & Audio", IconType::Palette, 5, &m_currentTab, 0, ICON_FA_PALETTE);
                    Widgets::SidebarTab("Profiles", IconType::Folder, 6, &m_currentTab, 0, ICON_FA_FLOPPY_DISK);

                    // User Profile at bottom of sidebar
                    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 58.0f);
                    ImGui::SetCursorPosX(14.0f);
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    ImVec2 userPos = ImGui::GetCursorScreenPos();
                    IconRenderer::DrawIcon(drawList, IconType::User, ImVec2(userPos.x + 12, userPos.y + 14), 14.0f,
                                          ThemeManager::ToU32(ThemeManager::Get().GetPalette().Accent), 1.6f);
                    drawList->AddText(ImVec2(userPos.x + 32, userPos.y + 4),
                                      ThemeManager::ToU32(ThemeManager::Get().GetPalette().TextPrimary), "SolarDev");
                    drawList->AddText(ImVec2(userPos.x + 32, userPos.y + 20),
                                      ThemeManager::ToU32(ThemeManager::Get().GetPalette().Accent), "LIFETIME VIP");
                }
                Widgets::EndSidebar();

                ImGui::SameLine(0, 10.0f);

                // Main Content View
                ImGui::BeginGroup();
                {
                    float contentWidth = ImGui::GetContentRegionAvail().x - 14.0f;
                    float cardWidth = (contentWidth - 10.0f) * 0.5f;

                    // ==========================================
                    // TAB 0: COMBAT & AIMBOT
                    // ==========================================
                    if (m_currentTab == 0) {
                        Widgets::SubTab("General", 0, &m_combatSubTab);
                        Widgets::SubTab("Targeting & Bones", 1, &m_combatSubTab);
                        Widgets::SubTab("Recoil Control", 2, &m_combatSubTab);
                        ImGui::NewLine();
                        Widgets::Spacing(6.0f);

                        if (Widgets::BeginCard("##AimCard", "Target Acquisition", IconType::Crosshair, ImVec2(cardWidth, 490.0f), ICON_FA_CROSSHAIRS)) {
                            Widgets::Toggle("Enable Aimbot", &m_aimbotEnabled, "Smooth angle correction towards targets");
                            Widgets::Toggle("Silent Aim", &m_silentAim, "Snaps bullet vector invisibly");
                            Widgets::Separator();

                            Widgets::SliderFloat("Field of View", &m_aimFov, 1.0f, 180.0f, "%.1f", "deg");
                            Widgets::SliderFloat("Smoothness", &m_aimSmooth, 1.0f, 30.0f, "%.1f", "x");

                            const char* bones[] = { "Head", "Neck", "Upper Chest", "Stomach", "Nearest Hitbox" };
                            Widgets::Combo("Preferred Hitbox", &m_targetBone, bones, 5);

                            Widgets::Separator();
                            Widgets::Keybind("Aim Hotkey", &m_aimKey, &m_aimKeyMode);

                            Widgets::EndCard();
                        }

                        ImGui::SameLine(0, 10.0f);

                        if (Widgets::BeginCard("##HitboxMatrixCard", "Hitbox Selection Matrix", IconType::User, ImVec2(cardWidth, 490.0f), ICON_FA_USER)) {
                            ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "Click body zones to toggle targetable bones:");
                            Widgets::Spacing(4.0f);

                            Widgets::HitboxPicker(m_hitboxes, ImVec2(cardWidth - 24.0f, 210.0f));
                            Widgets::Separator();

                            Widgets::Toggle("Recoil Compensation (RCS)", &m_rcsEnabled);
                            Widgets::SliderFloat("Pitch Comp", &m_rcsPitch, 0.0f, 100.0f, "%.0f", "%");
                            Widgets::SliderFloat("Yaw Comp", &m_rcsYaw, 0.0f, 100.0f, "%.0f", "%");

                            Widgets::EndCard();
                        }
                    }

                    // ==========================================
                    // TAB 1: VISUALS / ESP
                    // ==========================================
                    else if (m_currentTab == 1) {
                        Widgets::SubTab("Player ESP", 0, &m_visualsSubTab);
                        Widgets::SubTab("World & Chams", 1, &m_visualsSubTab);
                        ImGui::NewLine();
                        Widgets::Spacing(6.0f);

                        if (Widgets::BeginCard("##ESPSettings", "Visual Components", IconType::Eye, ImVec2(cardWidth, 490.0f), ICON_FA_EYE)) {
                            Widgets::Toggle("2D Bounding Box", &m_espSettings.enableBox);
                            const char* boxTypes[] = { "Full Box", "Corner Box", "Filled Box" };
                            Widgets::Combo("Box Style", &m_espSettings.boxType, boxTypes, 3);
                            float boxCol[4] = { m_espSettings.boxColor.x, m_espSettings.boxColor.y, m_espSettings.boxColor.z, m_espSettings.boxColor.w };
                            if (Widgets::ColorPicker("Box Accent", boxCol)) {
                                m_espSettings.boxColor = ImVec4(boxCol[0], boxCol[1], boxCol[2], boxCol[3]);
                            }

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
                            Widgets::Toggle("Snaplines", &m_espSettings.enableSnapline);
                            Widgets::Toggle("Glow Halo", &m_espSettings.enableGlow);

                            Widgets::EndCard();
                        }

                        ImGui::SameLine(0, 10.0f);

                        if (Widgets::BeginCard("##ESPPreviewCard", "Interactive 2D ESP Preview", IconType::Sparkle, ImVec2(cardWidth, 490.0f), ICON_FA_WAND_MAGIC)) {
                            ESPPreview::Render("##LiveMannequin", ImVec2(cardWidth - 24.0f, 320.0f), m_espSettings);

                            Widgets::Separator();
                            Widgets::SliderFloat("Simulated HP", &m_espSettings.health, 1.0f, 100.0f, "%.0f", "HP");
                            Widgets::SliderFloat("Simulated Armor", &m_espSettings.armor, 0.0f, 100.0f, "%.0f", "AP");
                            Widgets::SliderFloat("Target Distance", &m_espSettings.distance, 2.0f, 120.0f, "%.1f", "m");

                            Widgets::EndCard();
                        }
                    }

                    // ==========================================
                    // TAB 2: RADAR & HUD OVERLAYS
                    // ==========================================
                    else if (m_currentTab == 2) {
                        Widgets::SubTab("Tactical Radar", 0, &m_miscSubTab);
                        Widgets::SubTab("HUD Overlays", 1, &m_miscSubTab);
                        ImGui::NewLine();
                        Widgets::Spacing(6.0f);

                        if (Widgets::BeginCard("##RadarCard", "2D Tactical Mini-Radar", IconType::Crosshair, ImVec2(cardWidth, 490.0f), ICON_FA_CROSSHAIRS)) {
                            ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "Live 2D top-down positional radar with blips:");
                            Widgets::Spacing(6.0f);

                            Widgets::RadarPreview("##MiniRadar", ImVec2(cardWidth - 24.0f, 260.0f));
                            Widgets::Separator();

                            if (Widgets::Button("Center Radar on Crosshair", ImVec2(0, 36), ButtonStyle::Secondary)) {
                                Audio::PlayClick();
                            }

                            Widgets::EndCard();
                        }

                        ImGui::SameLine(0, 10.0f);

                        if (Widgets::BeginCard("##HUDCard", "HUD Elements & Windows", IconType::Bell, ImVec2(cardWidth, 490.0f), ICON_FA_EXPAND)) {
                            Widgets::Toggle("Show Watermark Overlay", &m_showWatermark);
                            Widgets::Toggle("Show Spectator List Window", &m_showSpectators);
                            Widgets::Toggle("Show Active Keybinds Window", &m_showKeybinds);
                            Widgets::Toggle("Show Engine Telemetry Profiler", &m_showProfiler);
                            Widgets::Separator();

                            ImGui::TextColored(ThemeManager::Get().GetPalette().TextSecondary, "Trigger Notification Toasts:");
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Post Success Notification", ImVec2(0, 34), ButtonStyle::Primary)) {
                                Notify::Success("Solar Framework", "Operation finished successfully!");
                            }
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Post Warning Notification", ImVec2(0, 34), ButtonStyle::Secondary)) {
                                Notify::Warning("Security Alert", "High memory signature detected.");
                            }
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Post Error Notification", ImVec2(0, 34), ButtonStyle::Danger)) {
                                Notify::Error("Hook Failure", "Failed to resolve swapchain pointer.");
                            }

                            Widgets::EndCard();
                        }
                    }

                    // ==========================================
                    // TAB 3: HWID SPOOFER & CLEANER
                    // ==========================================
                    else if (m_currentTab == 3) {
                        if (Widgets::BeginCard("##SpooferMainCard", "Hardware ID Spoofer & Trace Cleaner", IconType::Shield, ImVec2(contentWidth, 490.0f), ICON_FA_FINGERPRINT)) {
                            Widgets::SpooferPanel(m_spooferState);
                            Widgets::EndCard();
                        }
                    }

                    // ==========================================
                    // TAB 4: LOADER & LICENSE SCREEN
                    // ==========================================
                    else if (m_currentTab == 4) {
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
                    // TAB 5: THEMES & AUDIO
                    // ==========================================
                    else if (m_currentTab == 5) {
                        Widgets::SubTab("Color Presets", 0, &m_themeSubTab);
                        Widgets::SubTab("Audio & FX", 1, &m_themeSubTab);
                        ImGui::NewLine();
                        Widgets::Spacing(6.0f);

                        if (Widgets::BeginCard("##ThemesList", "Color Presets (PastOwl Signature)", IconType::Palette, ImVec2(cardWidth, 490.0f), ICON_FA_PALETTE)) {
                            if (Widgets::Button("Solar Flare (Amber Gold)", ImVec2(0, 36), ButtonStyle::Primary)) {
                                ThemeManager::Get().ApplyPreset(ThemePreset::SolarFlare);
                                Notify::Success("Theme Applied", "Switched to Solar Flare theme.");
                            }
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Cyber Neon (Electric Cyan)", ImVec2(0, 36), ButtonStyle::Secondary)) {
                                ThemeManager::Get().ApplyPreset(ThemePreset::CyberNeon);
                                Notify::Success("Theme Applied", "Switched to Cyber Neon theme.");
                            }
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Void Amethyst (Deep Purple)", ImVec2(0, 36), ButtonStyle::Secondary)) {
                                ThemeManager::Get().ApplyPreset(ThemePreset::VoidAmethyst);
                                Notify::Success("Theme Applied", "Switched to Void Amethyst theme.");
                            }
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Emerald Matrix (Vivid Green)", ImVec2(0, 36), ButtonStyle::Secondary)) {
                                ThemeManager::Get().ApplyPreset(ThemePreset::EmeraldMatrix);
                                Notify::Success("Theme Applied", "Switched to Emerald Matrix theme.");
                            }
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Blood Ruby (Crimson Rose)", ImVec2(0, 36), ButtonStyle::Secondary)) {
                                ThemeManager::Get().ApplyPreset(ThemePreset::BloodRuby);
                                Notify::Success("Theme Applied", "Switched to Blood Ruby theme.");
                            }
                            Widgets::Spacing(4.0f);

                            if (Widgets::Button("Arctic White (Ice Blue)", ImVec2(0, 36), ButtonStyle::Secondary)) {
                                ThemeManager::Get().ApplyPreset(ThemePreset::ArcticWhite);
                                Notify::Success("Theme Applied", "Switched to Arctic White theme.");
                            }
                            Widgets::Separator();

                            if (Widgets::ColorPicker("Custom Accent RGB", m_customColor)) {
                                ThemeManager::Get().SetCustomAccent(
                                    Color(m_customColor[0], m_customColor[1], m_customColor[2], 1.0f),
                                    Color(m_customColor[0] * 1.2f, m_customColor[1] * 1.2f, m_customColor[2] * 1.2f, 1.0f)
                                );
                            }

                            Widgets::EndCard();
                        }

                        ImGui::SameLine(0, 10.0f);

                        if (Widgets::BeginCard("##AudioAndFX", "Audio Feedback & Effects", IconType::Sparkle, ImVec2(cardWidth, 490.0f), ICON_FA_VOLUME_HIGH)) {
                            auto& style = ThemeManager::Get().GetStyle();

                            Widgets::Toggle("Enable Audio Feedback", &style.EnableAudio, "Procedural mechanical click and chime sounds");
                            if (Widgets::Button("Test Click Sound", ImVec2(0, 32), ButtonStyle::Secondary)) {
                                Audio::PlayClick();
                            }
                            if (Widgets::Button("Test Toggle On Sound", ImVec2(0, 32), ButtonStyle::Secondary)) {
                                Audio::PlayToggle(true);
                            }
                            if (Widgets::Button("Test Chime Notification", ImVec2(0, 32), ButtonStyle::Secondary)) {
                                Audio::PlayNotification();
                            }
                            if (Widgets::Button("Test Error Tone", ImVec2(0, 32), ButtonStyle::Danger)) {
                                Audio::PlayError();
                            }

                            Widgets::Separator();
                            Widgets::Toggle("Enable Glow Effects", &style.EnableGlow);
                            Widgets::SliderFloat("Glow Intensity", &style.GlowIntensity, 0.2f, 2.0f, "%.1f", "x");
                            
                            const char* particleModes[] = { "Falling Cyber Snow", "Rising Amber Embers", "Star Constellations", "Disabled" };
                            static int currentParticleMode = static_cast<int>(FX::ParticleSystem::Get().GetMode());
                            if (Widgets::Combo("Background FX Mode", &currentParticleMode, particleModes, 4)) {
                                FX::ParticleSystem::Get().SetMode(static_cast<FX::ParticleEffectMode>(currentParticleMode));
                            }
                            Widgets::SliderInt("Particle Count", &style.ParticleCount, 10, 100, "%d");

                            Widgets::Separator();
                            if (Widgets::Button("Replay Luxury Splash Screen", ImVec2(0, 36), ButtonStyle::Primary)) {
                                UI::SplashScreen::Get().Start(2.8f);
                            }

                            Widgets::EndCard();
                        }
                    }

                    // ==========================================
                    // TAB 6: CONFIG PROFILES
                    // ==========================================
                    else if (m_currentTab == 6) {
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
    }

} // namespace Solar
