#include "studio_app.hpp"
#include "solar_ai.hpp"
#include <solar/solar.hpp>
#include <imgui_internal.h>
#include <cmath>
#include <thread>

namespace Solar::Studio {

    StudioApp& StudioApp::Get() {
        static StudioApp instance;
        return instance;
    }

    void StudioApp::Initialize() {
        // Auto-detect MSBuild
        std::string detectedMSBuild = ProjectGenerator::AutoDetectMSBuild();
        strncpy_s(m_msbuildPathBuf, sizeof(m_msbuildPathBuf), detectedMSBuild.c_str(), _TRUNCATE);
        m_config.msbuildPath = detectedMSBuild;

        // Initialize Local Solar AI
        SolarAI::Get().Initialize();

        // Default to Modern Software Loader Template
        ApplyTemplate(0);

        m_buildLogs.push_back("[Solar Studio BETA v0.1.2] Engine environment initialized.");
        m_buildLogs.push_back("[Solar Studio] MSBuild detected: " + detectedMSBuild);
        m_buildLogs.push_back("[Solar Studio] Local Solar AI Copilot ready for natural language instructions.");
    }

    void StudioApp::ClearComponents() {
        m_components.clear();
        Audio::PlayClick();
    }

    void StudioApp::AddComponent(const StudioComponent& comp) {
        m_components.push_back(comp);
        Audio::PlayClick();
    }

    void StudioApp::SetAccentColor(float r, float g, float b, float a) {
        m_config.accentColor[0] = r;
        m_config.accentColor[1] = g;
        m_config.accentColor[2] = b;
        m_config.accentColor[3] = a;

        auto& theme = ThemeManager::Get();
        theme.GetPalette().Accent = Color(r, g, b, a);
        theme.GetPalette().AccentHover = Color(r * 1.1f, g * 1.1f, b * 1.1f, a);
        theme.SyncWithImGui();
    }

    void StudioApp::SetBorderRounding(float r) {
        m_config.borderRounding = r;
        ThemeManager::Get().GetStyle().WindowRounding = r;
        ThemeManager::Get().GetStyle().CardRounding = r;
    }

    void StudioApp::ApplyAnimationToAll(ComponentAnimation anim) {
        for (auto& comp : m_components) {
            comp.anim = anim;
        }
        Audio::PlayNotification();
    }

    void StudioApp::SetupLoaderTemplate() {
        m_config.accentColor[0] = 0.18f; m_config.accentColor[1] = 0.85f; m_config.accentColor[2] = 0.55f; m_config.accentColor[3] = 1.0f;
        SetBorderRounding(14.0f);

        strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Solar Secure Software Launcher", _TRUNCATE);
        strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "v2.5.0 • Fast Cloud Activation", _TRUNCATE);
        m_config.windowTitle = m_windowTitleBuf;
        m_config.windowSubtitle = m_windowSubtitleBuf;

        m_components.clear();

        StudioComponent keyInput;
        keyInput.type = "input_text";
        keyInput.label = "License Product Key";
        keyInput.subtitle = "Enter your 16-character alphanumeric license key";
        strncpy_s(keyInput.textBuf, sizeof(keyInput.textBuf), "SOLAR-98FA-4C12-880B", _TRUNCATE);
        keyInput.anim = ComponentAnimation::None;
        m_components.push_back(keyInput);

        StudioComponent rememberToggle;
        rememberToggle.type = "toggle";
        rememberToggle.label = "Remember HWID Session";
        rememberToggle.subtitle = "Encrypt credentials securely in Windows Credential Store";
        rememberToggle.boolVal = true;
        rememberToggle.anim = ComponentAnimation::None;
        m_components.push_back(rememberToggle);

        StudioComponent progress;
        progress.type = "progress_bar";
        progress.label = "Verification Status";
        progress.subtitle = "Validating cryptographic integrity & CDN assets...";
        progress.floatVal = 82.0f;
        progress.anim = ComponentAnimation::ShimmerWave;
        m_components.push_back(progress);

        StudioComponent launchBtn;
        launchBtn.type = "button";
        launchBtn.label = "Authenticate & Launch Application";
        launchBtn.subtitle = "";
        launchBtn.anim = ComponentAnimation::GlowPulse;
        launchBtn.animSpeed = 2.8f;
        m_components.push_back(launchBtn);

        StudioComponent telemetry;
        telemetry.type = "stat_card";
        telemetry.label = "Cloud CDN Latency";
        telemetry.floatVal = 14.2f;
        telemetry.anim = ComponentAnimation::BreathingSine;
        m_components.push_back(telemetry);

        Audio::PlayNotification();
    }

    void StudioApp::TriggerMSBuild() {
        m_inspectorTab = 1; // Switch to MSBuild tab
        m_isBuilding = true;
        m_buildLogs.push_back("[Solar Studio] Triggered automated MSBuild build pass...");

        std::thread([this]() {
            ProjectGenerator::RunMSBuild(m_config, [this](const std::string& line) {
                m_buildLogs.push_back(line);
            });
            m_isBuilding = false;
        }).detach();
    }

    void StudioApp::ApplyTemplate(int templateIndex) {
        m_selectedTemplate = templateIndex;

        switch (templateIndex) {
        case 0: // Modern Software Loader (NEW!)
            SetupLoaderTemplate();
            break;
        case 1: // Tactical Overlay
            m_config.accentColor[0] = 0.20f; m_config.accentColor[1] = 0.65f; m_config.accentColor[2] = 1.00f; m_config.accentColor[3] = 1.0f;
            SetBorderRounding(8.0f);
            strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Tactical Overlay Engine", _TRUNCATE);
            strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "DirectX 11 • Low Latency Pipeline", _TRUNCATE);
            m_components.clear();
            m_components.push_back({ "toggle", "High Performance Multithreading", "Enables asynchronous graphics pass", 0, 0, true, "", ComponentAnimation::GlowPulse });
            m_components.push_back({ "slider", "Target Processing Frequency", "", 75.0f, 0, false, "", ComponentAnimation::None });
            m_components.push_back({ "fov_reticle", "Dynamic FOV Reticle", "", 120.0f, 0, true, "", ComponentAnimation::BreathingSine });
            m_components.push_back({ "button", "Execute Simulation Step", "", 0, 0, false, "", ComponentAnimation::ShimmerWave });
            break;
        case 2: // Obsidian Luxury
            m_config.accentColor[0] = 0.95f; m_config.accentColor[1] = 0.75f; m_config.accentColor[2] = 0.25f; m_config.accentColor[3] = 1.0f;
            SetBorderRounding(12.0f);
            strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Obsidian Gold Edition", _TRUNCATE);
            strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "Luxury Dashboard Suite", _TRUNCATE);
            m_components.clear();
            m_components.push_back({ "toggle", "Gold Sheen Specular Engine", "Active specular surface bloom", 0, 0, true, "", ComponentAnimation::ShimmerWave });
            m_components.push_back({ "radial_gauge", "Memory Bandwidth", "", 91.5f, 0, false, "", ComponentAnimation::BreathingSine });
            m_components.push_back({ "button", "Confirm VIP Allocation", "", 0, 0, false, "", ComponentAnimation::GlowPulse });
            break;
        case 3: // Cyberpunk 2077 Neon
            m_config.accentColor[0] = 1.00f; m_config.accentColor[1] = 0.20f; m_config.accentColor[2] = 0.55f; m_config.accentColor[3] = 1.0f;
            SetBorderRounding(4.0f);
            strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Cyberpunk Night City HUD", _TRUNCATE);
            strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "Synthetic Neon Subsystem", _TRUNCATE);
            m_components.clear();
            m_components.push_back({ "toggle", "Neural Sync Link", "Direct neural graphics bridge", 0, 0, true, "", ComponentAnimation::GlowPulse });
            m_components.push_back({ "slider", "Overdrive Overclock", "", 88.0f, 0, false, "", ComponentAnimation::None });
            m_components.push_back({ "fov_reticle", "Cyber Smart Reticle", "", 135.0f, 0, true, "", ComponentAnimation::GlowPulse });
            m_components.push_back({ "button", "Engage Cyber Overdrive", "", 0, 0, false, "", ComponentAnimation::ShimmerWave });
            break;
        case 4: // Minimalist Streamer HUD
            m_config.accentColor[0] = 0.18f; m_config.accentColor[1] = 0.85f; m_config.accentColor[2] = 0.55f; m_config.accentColor[3] = 1.0f;
            SetBorderRounding(16.0f);
            strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Minimal Streamer HUD", _TRUNCATE);
            strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "Clean Broadcast Telemetry", _TRUNCATE);
            m_components.clear();
            m_components.push_back({ "toggle", "Streamer Privacy Overlay", "Hides private IDs during stream", 0, 0, true, "", ComponentAnimation::None });
            m_components.push_back({ "slider", "Overlay Transparency", "", 45.0f, 0, false, "", ComponentAnimation::None });
            m_components.push_back({ "button", "Toggle Clean Cam View", "", 0, 0, false, "", ComponentAnimation::None });
            break;
        case 5: // Esports Pro Telemetry
            m_config.accentColor[0] = 0.98f; m_config.accentColor[1] = 0.40f; m_config.accentColor[2] = 0.12f; m_config.accentColor[3] = 1.0f;
            SetBorderRounding(6.0f);
            strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Apex Esports Telemetry", _TRUNCATE);
            strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "Competitive Ballistics & Stats", _TRUNCATE);
            m_components.clear();
            m_components.push_back({ "toggle", "Tickrate Telemetry Monitor", "Live server tick capture", 0, 0, true, "", ComponentAnimation::None });
            m_components.push_back({ "skin_changer", "Tournament Weapon Inspect", "", 0, 0, false, "", ComponentAnimation::ShimmerWave });
            m_components.push_back({ "button", "Lock Tournament Loadout", "", 0, 0, false, "", ComponentAnimation::GlowPulse });
            break;
        }

        m_config.windowTitle = m_windowTitleBuf;
        m_config.windowSubtitle = m_windowSubtitleBuf;
        Audio::PlayClick();
    }

    void StudioApp::RenderHeaderBar() {
        const auto& pal = ThemeManager::Get().GetPalette();
        ImVec2 p = ImGui::GetCursorScreenPos();
        float w = ImGui::GetContentRegionAvail().x;
        float headerH = 50.0f;

        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 hMin = p;
        ImVec2 hMax = ImVec2(p.x + w, p.y + headerH);

        // Dark banner with specular line
        draw->AddRectFilled(hMin, hMax, pal.Header.ToU32(), 8.0f);
        draw->AddLine(ImVec2(hMin.x, hMax.y), hMax, pal.Border.ToU32(), 1.0f);

        // Logo text & glowing version badge
        draw->AddText(ImVec2(hMin.x + 16.0f, hMin.y + 14.0f), IM_COL32(255, 255, 255, 255), "SOLAR STUDIO");

        ImVec2 badgePos(hMin.x + 130.0f, hMin.y + 13.0f);
        draw->AddRectFilled(badgePos, ImVec2(badgePos.x + 105.0f, badgePos.y + 22.0f), pal.Accent.WithAlpha(0.18f).ToU32(), 4.0f);
        draw->AddRect(badgePos, ImVec2(badgePos.x + 105.0f, badgePos.y + 22.0f), pal.Accent.ToU32(), 4.0f, 0, 1.0f);
        draw->AddText(ImVec2(badgePos.x + 8.0f, badgePos.y + 3.0f), pal.Accent.ToU32(), "BETA v0.1.2");

        // Solar AI status badge
        ImVec2 aiBadgePos(badgePos.x + 115.0f, hMin.y + 13.0f);
        float pulse = (std::sin(static_cast<float>(ImGui::GetTime()) * 3.0f) * 0.5f + 0.5f);
        draw->AddRectFilled(aiBadgePos, ImVec2(aiBadgePos.x + 120.0f, aiBadgePos.y + 22.0f), IM_COL32(80, 140, 255, static_cast<int>(30 + 40 * pulse)), 4.0f);
        draw->AddRect(aiBadgePos, ImVec2(aiBadgePos.x + 120.0f, aiBadgePos.y + 22.0f), IM_COL32(100, 170, 255, 220), 4.0f, 0, 1.0f);
        draw->AddText(ImVec2(aiBadgePos.x + 7.0f, aiBadgePos.y + 3.0f), IM_COL32(140, 200, 255, 255), "SOLAR AI COPILOT");

        // Action controls on the right
        ImGui::SetCursorPos(ImVec2(w - 530.0f, 8.0f));
        ImGui::SetNextItemWidth(170.0f);
        const char* templates[] = { "Modern Software Loader", "Tactical Overlay", "Obsidian Luxury", "Cyberpunk Neon", "Minimal Streamer", "Esports Pro" };
        if (Widgets::Combo("##TemplateSelect", &m_selectedTemplate, templates, 6)) {
            ApplyTemplate(m_selectedTemplate);
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(140.0f);
        const char* modes[] = { "Standalone (.exe)", "In-Process (.dll)" };
        int modeIdx = static_cast<int>(m_config.outputType);
        if (Widgets::Combo("##ModeSelect", &modeIdx, modes, 2)) {
            m_config.outputType = static_cast<TargetOutputType>(modeIdx);
            Audio::PlayClick();
        }

        ImGui::SameLine();
        if (Widgets::Button("Build Project", ImVec2(130.0f, 32.0f), ButtonStyle::Primary)) {
            TriggerMSBuild();
        }

        ImGui::SetCursorPosY(headerH + 12.0f);
    }

    void StudioApp::RenderToolbox(float width) {
        if (Widgets::BeginCard("##ToolboxCard", "Component Palette", IconType::Sliders, ImVec2(width, 0.0f))) {
            ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "CLICK TO ADD COMPONENT");
            Widgets::Spacing(6.0f);

            if (Widgets::Button("+ License Key Input", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                StudioComponent c;
                c.type = "input_text";
                c.label = "License Key";
                c.subtitle = "Software product key";
                strncpy_s(c.textBuf, sizeof(c.textBuf), "XXXX-XXXX-XXXX-XXXX", _TRUNCATE);
                m_components.push_back(c);
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Progress Bar", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                StudioComponent c;
                c.type = "progress_bar";
                c.label = "Loading Progress";
                c.subtitle = "Downloading resources...";
                c.floatVal = 64.0f;
                c.anim = ComponentAnimation::ShimmerWave;
                m_components.push_back(c);
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Toggle Switch", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "toggle", "New Feature Switch", "Configured in Solar Studio", 0, 0, true, "", ComponentAnimation::None });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Linear Slider", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "slider", "Calibrated Dynamic Value", "", 50.0f, 0, false, "", ComponentAnimation::None });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Dropdown Combo", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "combo", "Execution Mode Option", "", 0, 0, false, "", ComponentAnimation::None });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Action Button", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "button", "Trigger Process", "", 0, 0, false, "", ComponentAnimation::GlowPulse });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Radial Gauge", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "radial_gauge", "Metric Telemetry", "", 78.0f, 0, false, "", ComponentAnimation::BreathingSine });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Dynamic FOV Reticle", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "fov_reticle", "Dynamic FOV Reticle", "", 120.0f, 0, true, "", ComponentAnimation::BreathingSine });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Skin Changer Preview", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "skin_changer", "Cosmetic Customizer", "", 0, 0, false, "", ComponentAnimation::ShimmerWave });
                Audio::PlayClick();
            }

            Widgets::Separator();
            if (Widgets::Button("Clear All Components", ImVec2(width - 36.0f, 28.0f), ButtonStyle::Danger)) {
                ClearComponents();
            }

            Widgets::EndCard();
        }
    }

    void StudioApp::RenderCanvas(float width) {
        if (Widgets::BeginCard("##WYSIWYGCard", "Live WYSIWYG Canvas (Pixel Accurate)", IconType::Sparkle, ImVec2(width, 0.0f))) {
            ImDrawList* draw = ImGui::GetWindowDrawList();
            ImVec2 cMin = ImGui::GetCursorScreenPos();
            float availW = ImGui::GetContentRegionAvail().x;
            float availH = 680.0f;
            ImVec2 cMax = ImVec2(cMin.x + availW, cMin.y + availH);

            // Dark canvas background with blueprint dot matrix
            draw->AddRectFilled(cMin, cMax, IM_COL32(8, 10, 14, 255), 8.0f);

            float gridStep = 20.0f;
            u32 dotCol = IM_COL32(255, 255, 255, 14);
            for (float gx = cMin.x + 10.0f; gx < cMax.x; gx += gridStep) {
                for (float gy = cMin.y + 10.0f; gy < cMax.y; gy += gridStep) {
                    draw->AddCircleFilled(ImVec2(gx, gy), 1.0f, dotCol);
                }
            }

            // Simulated Target Window Frame
            float winW = std::min(availW - 40.0f, 620.0f);
            float winH = 640.0f;
            ImVec2 winMin = ImVec2(cMin.x + (availW - winW) * 0.5f, cMin.y + 20.0f);
            ImVec2 winMax = ImVec2(winMin.x + winW, winMin.y + winH);

            // Window Drop Shadow
            draw->AddRectFilled(ImVec2(winMin.x - 4, winMin.y + 6), ImVec2(winMax.x + 4, winMax.y + 8), IM_COL32(0, 0, 0, 180), m_config.borderRounding + 2.0f);

            // Window Body
            u32 winBodyCol = IM_COL32(14, 17, 24, 250);
            draw->AddRectFilled(winMin, winMax, winBodyCol, m_config.borderRounding);

            // Titlebar Strip
            float tbH = 44.0f;
            ImVec2 tbMax = ImVec2(winMax.x, winMin.y + tbH);
            u32 accentCol = ImGui::ColorConvertFloat4ToU32(ImVec4(m_config.accentColor[0], m_config.accentColor[1], m_config.accentColor[2], m_config.accentColor[3]));
            draw->AddRectFilled(winMin, tbMax, IM_COL32(20, 24, 34, 255), m_config.borderRounding, ImDrawFlags_RoundCornersTop);
            draw->AddLine(ImVec2(winMin.x, winMin.y + tbH), tbMax, IM_COL32(255, 255, 255, 18), 1.0f);

            // Titlebar Accent Indicator & Text
            draw->AddRectFilled(ImVec2(winMin.x + 14.0f, winMin.y + 14.0f), ImVec2(winMin.x + 18.0f, winMin.y + 30.0f), accentCol, 2.0f);
            draw->AddText(ImVec2(winMin.x + 26.0f, winMin.y + 13.0f), IM_COL32(255, 255, 255, 255), m_config.windowTitle.c_str());
            draw->AddText(ImVec2(winMin.x + 26.0f, winMin.y + 27.0f), IM_COL32(140, 155, 175, 220), m_config.windowSubtitle.c_str());

            // Window Border (Exact pixel rounding specified by user)
            Render::ImGuiExt::AddSmoothBorder(draw, winMin, winMax, IM_COL32(255, 255, 255, 30), m_config.borderRounding, 1.0f);

            // Render live components inside the simulated canvas window
            ImGui::SetCursorScreenPos(ImVec2(winMin.x + 18.0f, winMin.y + tbH + 16.0f));
            ImGui::BeginChild("##CanvasInnerChild", ImVec2(winW - 36.0f, winH - tbH - 32.0f), false, ImGuiWindowFlags_NoBackground);

            float timeNow = static_cast<float>(ImGui::GetTime());

            for (size_t i = 0; i < m_components.size(); ++i) {
                auto& comp = m_components[i];
                ImGui::PushID(static_cast<int>(i));

                ImVec2 itemStartPos = ImGui::GetCursorScreenPos();
                float itemWidth = ImGui::GetContentRegionAvail().x;

                // 1. Dynamic Animation Visual Processing
                if (comp.anim == ComponentAnimation::GlowPulse) {
                    float pulse = (std::sin(timeNow * comp.animSpeed) * 0.5f + 0.5f);
                    u32 pulseGlowCol = ImGui::ColorConvertFloat4ToU32(ImVec4(m_config.accentColor[0], m_config.accentColor[1], m_config.accentColor[2], 0.28f * pulse));
                    draw->AddRectFilled(itemStartPos, ImVec2(itemStartPos.x + itemWidth, itemStartPos.y + 42.0f), pulseGlowCol, 6.0f);
                } else if (comp.anim == ComponentAnimation::ShimmerWave) {
                    float shimmerX = std::fmod(timeNow * 160.0f, itemWidth + 80.0f) - 40.0f;
                    ImVec2 s1(itemStartPos.x + shimmerX, itemStartPos.y);
                    ImVec2 s2(itemStartPos.x + shimmerX + 30.0f, itemStartPos.y + 42.0f);
                    draw->AddLine(s1, s2, IM_COL32(255, 255, 255, 60), 16.0f);
                } else if (comp.anim == ComponentAnimation::BreathingSine) {
                    float breath = std::sin(timeNow * comp.animSpeed) * 1.5f;
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + breath);
                }

                // 2. Component Rendering
                if (comp.type == "input_text") {
                    Widgets::InputText(comp.label.c_str(), comp.textBuf, sizeof(comp.textBuf));
                    if (!comp.subtitle.empty()) {
                        ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "%s", comp.subtitle.c_str());
                    }
                } else if (comp.type == "progress_bar") {
                    Widgets::ProgressBar(comp.floatVal / 100.0f, ImVec2(itemWidth - 10.0f, 22.0f));
                    if (!comp.subtitle.empty()) {
                        ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "%s", comp.subtitle.c_str());
                    }
                } else if (comp.type == "toggle") {
                    Widgets::Toggle(comp.label.c_str(), &comp.boolVal, comp.subtitle.empty() ? nullptr : comp.subtitle.c_str());
                } else if (comp.type == "slider") {
                    Widgets::SliderFloat(comp.label.c_str(), &comp.floatVal, 0.0f, 100.0f, "%.1f", "units");
                } else if (comp.type == "combo") {
                    const char* opts[] = { "Option Alpha", "Option Beta", "Option Gamma" };
                    Widgets::Combo(comp.label.c_str(), &comp.intVal, opts, 3);
                } else if (comp.type == "button") {
                    if (Widgets::Button(comp.label.c_str(), ImVec2(0, 36.0f), ButtonStyle::Primary)) {
                        Audio::PlayClick();
                    }
                } else if (comp.type == "radial_gauge") {
                    Widgets::RadialGauge(comp.label.c_str(), &comp.floatVal, 0.0f, 100.0f, 38.0f);
                } else if (comp.type == "stat_card") {
                    Widgets::StatCard(comp.label.c_str(), "14.2 ms", "+4.8%", true);
                } else if (comp.type == "fov_reticle") {
                    m_previewFov.radius = comp.floatVal;
                    m_previewFov.color = ImVec4(m_config.accentColor[0], m_config.accentColor[1], m_config.accentColor[2], m_config.accentColor[3]);
                    Game::FOVRenderer::RenderPreview("##StudioFovPreview", ImVec2(0, 190.0f), m_previewFov);
                } else if (comp.type == "skin_changer") {
                    Game::SkinChangerPreview::Render("##StudioSkinPreview", ImVec2(0, 320.0f), m_previewSkin, m_dummyInventory);
                }

                // 3. Mini Animation Selector Pill for this component
                ImGui::SameLine(itemWidth - 100.0f);
                ImGui::SetNextItemWidth(95.0f);
                const char* animList[] = { "No Anim", "FadeIn", "SlideUp", "Bounce", "GlowPulse", "Shimmer", "Breathing" };
                int currentAnim = static_cast<int>(comp.anim);
                if (ImGui::Combo("##AnimSelect", &currentAnim, animList, 7)) {
                    comp.anim = static_cast<ComponentAnimation>(currentAnim);
                    Audio::PlayClick();
                }

                Widgets::Spacing(6.0f);
                ImGui::PopID();
            }

            ImGui::EndChild();
            Widgets::EndCard();
        }
    }

    void StudioApp::RenderAICopilot() {
        ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "SOLAR AI COPILOT [BETA]");
        ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "Décrivez votre interface en français ou anglais :");
        Widgets::Spacing(6.0f);

        // Suggested Prompt Chips
        ImGui::TextColored(ThemeManager::Get().GetPalette().TextSecondary, "Actions Rapides (Cliquez pour exécuter) :");
        
        if (Widgets::Button("Crée un loader moderne", ImVec2(0, 26.0f), ButtonStyle::Secondary)) {
            SolarAI::Get().AddUserMessage("Crée un loader moderne");
            auto resp = SolarAI::Get().ProcessPrompt("loader", *this);
            SolarAI::Get().AddAIMessage(resp.text, resp.actions);
            Audio::PlayClick();
        }
        ImGui::SameLine();
        if (Widgets::Button("Thème Obsidian Or", ImVec2(0, 26.0f), ButtonStyle::Secondary)) {
            SolarAI::Get().AddUserMessage("Passe en thème Obsidian Or");
            auto resp = SolarAI::Get().ProcessPrompt("or luxe", *this);
            SolarAI::Get().AddAIMessage(resp.text, resp.actions);
            Audio::PlayClick();
        }

        if (Widgets::Button("Cyberpunk Rose Neon", ImVec2(0, 26.0f), ButtonStyle::Secondary)) {
            SolarAI::Get().AddUserMessage("Passe en thème Cyberpunk Rose Neon");
            auto resp = SolarAI::Get().ProcessPrompt("cyberpunk", *this);
            SolarAI::Get().AddAIMessage(resp.text, resp.actions);
            Audio::PlayClick();
        }
        ImGui::SameLine();
        if (Widgets::Button("Coins ronds 18px", ImVec2(0, 26.0f), ButtonStyle::Secondary)) {
            SolarAI::Get().AddUserMessage("Arrondis les coins à 18px");
            auto resp = SolarAI::Get().ProcessPrompt("arrondi 18", *this);
            SolarAI::Get().AddAIMessage(resp.text, resp.actions);
            Audio::PlayClick();
        }

        if (Widgets::Button("Anime tout en Glow Pulse", ImVec2(0, 26.0f), ButtonStyle::Secondary)) {
            SolarAI::Get().AddUserMessage("Anime tout en Glow Pulse");
            auto resp = SolarAI::Get().ProcessPrompt("anime pulse", *this);
            SolarAI::Get().AddAIMessage(resp.text, resp.actions);
            Audio::PlayClick();
        }
        ImGui::SameLine();
        if (Widgets::Button("Anime Shimmer Wave", ImVec2(0, 26.0f), ButtonStyle::Secondary)) {
            SolarAI::Get().AddUserMessage("Anime tout en Shimmer Wave");
            auto resp = SolarAI::Get().ProcessPrompt("anime shimmer", *this);
            SolarAI::Get().AddAIMessage(resp.text, resp.actions);
            Audio::PlayClick();
        }

        Widgets::Separator();

        // Conversation History Feed
        ImGui::BeginChild("##AIConversationFeed", ImVec2(0, 240.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        const auto& history = SolarAI::Get().GetHistory();
        for (const auto& msg : history) {
            if (msg.isUser) {
                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "Vous [%s] :", msg.timestamp.c_str());
                ImGui::TextWrapped("%s", msg.content.c_str());
            } else {
                ImGui::TextColored(ImVec4(0.39f, 0.70f, 1.0f, 1.0f), "Solar AI [%s] :", msg.timestamp.c_str());
                ImGui::TextWrapped("%s", msg.content.c_str());
                if (!msg.actionsExecuted.empty()) {
                    for (const auto& act : msg.actionsExecuted) {
                        ImGui::Bullet();
                        ImGui::TextColored(ImVec4(0.31f, 0.90f, 0.55f, 0.95f), "%s", act.c_str());
                    }
                }
            }
            Widgets::Spacing(4.0f);
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

        Widgets::Spacing(6.0f);
        // Prompt input and submit
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 70.0f);
        bool enterPressed = ImGui::InputText("##AIPromptInput", m_aiPromptBuf, sizeof(m_aiPromptBuf), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();
        if (Widgets::Button("Envoyer", ImVec2(60.0f, 26.0f), ButtonStyle::Primary) || (enterPressed && strlen(m_aiPromptBuf) > 0)) {
            if (strlen(m_aiPromptBuf) > 0) {
                std::string prompt(m_aiPromptBuf);
                SolarAI::Get().AddUserMessage(prompt);
                auto resp = SolarAI::Get().ProcessPrompt(prompt, *this);
                SolarAI::Get().AddAIMessage(resp.text, resp.actions);
                m_aiPromptBuf[0] = '\0';
                Audio::PlayClick();
            }
        }
    }

    void StudioApp::RenderInspector(float width) {
        if (Widgets::BeginCard("##InspectorCard", "Inspector & Copilot", IconType::Sliders, ImVec2(width, 0.0f))) {
            Widgets::SubTab("Layout & Style", 0, &m_inspectorTab);
            Widgets::SubTab("MSBuild Build", 1, &m_inspectorTab);
            Widgets::SubTab("Solar AI [BETA]", 2, &m_inspectorTab);
            Widgets::Spacing(8.0f);

            if (m_inspectorTab == 0) {
                // Properties & Styling Tab
                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "PIXEL-PRECISE ROUNDING");
                if (Widgets::SliderFloat("Border Rounding", &m_config.borderRounding, 0.0f, 30.0f, "%.0f", "px")) {
                    SetBorderRounding(m_config.borderRounding);
                }

                Widgets::Separator();
                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "GLOBAL ANIMATIONS");
                const char* globalAnims[] = { "None", "FadeIn Entrance", "SlideUp Physics", "Spring Bounce (RK4)", "Glow Pulse Bloom", "Shimmer Wave", "Breathing Sine" };
                if (Widgets::Combo("Apply Animation to All", &m_activeAnimationIdx, globalAnims, 7)) {
                    ApplyAnimationToAll(static_cast<ComponentAnimation>(m_activeAnimationIdx));
                }

                Widgets::Separator();
                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "WINDOW TITLES");
                if (Widgets::InputText("App Title", m_windowTitleBuf, sizeof(m_windowTitleBuf))) {
                    m_config.windowTitle = m_windowTitleBuf;
                }
                if (Widgets::InputText("App Subtitle", m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf))) {
                    m_config.windowSubtitle = m_windowSubtitleBuf;
                }

                Widgets::Separator();
                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "PALETTE CALIBRATION");
                Widgets::ColorPicker("Accent Theme Color", m_config.accentColor);
                Widgets::ColorPicker("Card Body Color", m_config.cardColor);

                Widgets::Separator();
                Widgets::Toggle("Particle Physics Backdrop", &m_config.enableParticles);
                Widgets::Toggle("Multi-pass Ambient Glow", &m_config.enableGlow);
                Widgets::Toggle("Creamy Mechanical Switch Audio", &m_config.enableCreamyAudio);
            } else if (m_inspectorTab == 1) {
                // MSBuild & Executable Generator Tab
                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "PROJECT CONFIGURATION");
                if (Widgets::InputText("Project Name", m_projectNameBuf, sizeof(m_projectNameBuf))) {
                    m_config.projectName = m_projectNameBuf;
                }
                if (Widgets::InputText("Output Folder", m_outputDirBuf, sizeof(m_outputDirBuf))) {
                    m_config.outputDir = m_outputDirBuf;
                }

                const char* targetModes[] = { "Standalone Executable (.exe)", "Internal SwapChain DLL (.dll)" };
                int modeIdx = static_cast<int>(m_config.outputType);
                if (Widgets::Combo("Build Target", &modeIdx, targetModes, 2)) {
                    m_config.outputType = static_cast<TargetOutputType>(modeIdx);
                }

                Widgets::Separator();
                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "MSBUILD COMPILER PATH");
                Widgets::InputText("MSBuild Path", m_msbuildPathBuf, sizeof(m_msbuildPathBuf));
                m_config.msbuildPath = m_msbuildPathBuf;

                if (Widgets::Button("Auto-Detect MSBuild.exe", ImVec2(0, 30.0f), ButtonStyle::Secondary)) {
                    std::string detected = ProjectGenerator::AutoDetectMSBuild();
                    strncpy_s(m_msbuildPathBuf, sizeof(m_msbuildPathBuf), detected.c_str(), _TRUNCATE);
                    m_config.msbuildPath = detected;
                    m_buildLogs.push_back("[Solar Studio] Auto-detected MSBuild at: " + detected);
                    Audio::PlayClick();
                }

                Widgets::Spacing(8.0f);
                if (Widgets::Button(m_isBuilding ? "Building Artifact..." : "Generate & Compile Project", ImVec2(0, 42.0f), ButtonStyle::Primary)) {
                    if (!m_isBuilding) {
                        TriggerMSBuild();
                    }
                }

                Widgets::Separator();
                ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "MSBUILD TERMINAL OUTPUT:");

                ImGui::BeginChild("##BuildConsoleChild", ImVec2(0, 200.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
                for (const auto& line : m_buildLogs) {
                    if (line.find("ERROR") != std::string::npos) {
                        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", line.c_str());
                    } else if (line.find("SUCCESS") != std::string::npos || line.find("ready") != std::string::npos) {
                        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.5f, 1.0f), "%s", line.c_str());
                    } else {
                        ImGui::TextColored(ImVec4(0.7f, 0.75f, 0.85f, 1.0f), "%s", line.c_str());
                    }
                }
                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                    ImGui::SetScrollHereY(1.0f);
                }
                ImGui::EndChild();
            } else {
                // Tab 2: Solar AI Copilot
                RenderAICopilot();
            }

            Widgets::EndCard();
        }
    }

    void StudioApp::Render() {
        bool open = true;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(displaySize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 16.0f));

        if (ImGui::Begin("##SolarStudioWindow", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
            RenderHeaderBar();

            float availW = ImGui::GetContentRegionAvail().x;
            float leftW = 240.0f;
            float rightW = 380.0f;
            float centerW = availW - leftW - rightW - 24.0f;

            RenderToolbox(leftW);
            ImGui::SameLine(0, 12.0f);
            RenderCanvas(centerW);
            ImGui::SameLine(0, 12.0f);
            RenderInspector(rightW);

            ImGui::End();
        }

        ImGui::PopStyleVar(3);
    }

} // namespace Solar::Studio
