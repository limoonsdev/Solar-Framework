#include "studio_app.hpp"
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

        // Populate initial default layout components
        m_components.push_back({ "toggle", "High Performance Threading", "Enables multithreaded rendering pipeline", 0, 0, true });
        m_components.push_back({ "slider", "Target Processing Frequency", "", 75.0f, 0, false });
        m_components.push_back({ "combo", "DirectX Shader Precision", "", 0, 1, false });
        m_components.push_back({ "radial_gauge", "Core Compute Load", "", 82.5f, 0, false });
        m_components.push_back({ "button", "Execute Simulation Step", "", 0, 0, false });
        m_components.push_back({ "fov_reticle", "Dynamic FOV Reticle", "", 110.0f, 0, true });

        m_buildLogs.push_back("[Solar Studio BETA v0.1.2] Engine environment initialized.");
        m_buildLogs.push_back("[Solar Studio] MSBuild detected: " + detectedMSBuild);
        m_buildLogs.push_back("[Solar Studio] Ready to design and compile native C++20 projects.");
    }

    void StudioApp::ApplyTemplate(int templateIndex) {
        m_selectedTemplate = templateIndex;
        auto& theme = ThemeManager::Get();

        switch (templateIndex) {
        case 0: // Tactical Overlay
            m_config.accentColor[0] = 0.20f; m_config.accentColor[1] = 0.65f; m_config.accentColor[2] = 1.00f; m_config.accentColor[3] = 1.0f;
            m_config.borderRounding = 8.0f;
            strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Tactical Overlay Engine", _TRUNCATE);
            strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "DirectX 11 • Low Latency Pipeline", _TRUNCATE);
            break;
        case 1: // Obsidian Luxury
            m_config.accentColor[0] = 0.95f; m_config.accentColor[1] = 0.75f; m_config.accentColor[2] = 0.25f; m_config.accentColor[3] = 1.0f;
            m_config.borderRounding = 12.0f;
            strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Obsidian Gold Edition", _TRUNCATE);
            strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "Luxury Dashboard Suite", _TRUNCATE);
            break;
        case 2: // Cyberpunk 2077 Neon
            m_config.accentColor[0] = 1.00f; m_config.accentColor[1] = 0.20f; m_config.accentColor[2] = 0.55f; m_config.accentColor[3] = 1.0f;
            m_config.borderRounding = 4.0f;
            strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Cyberpunk Night City HUD", _TRUNCATE);
            strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "Synthetic Neon Subsystem", _TRUNCATE);
            break;
        case 3: // Minimalist Streamer HUD
            m_config.accentColor[0] = 0.18f; m_config.accentColor[1] = 0.85f; m_config.accentColor[2] = 0.55f; m_config.accentColor[3] = 1.0f;
            m_config.borderRounding = 16.0f;
            strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Minimal Streamer HUD", _TRUNCATE);
            strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "Clean Broadcast Telemetry", _TRUNCATE);
            break;
        case 4: // Esports Pro Telemetry
            m_config.accentColor[0] = 0.98f; m_config.accentColor[1] = 0.40f; m_config.accentColor[2] = 0.12f; m_config.accentColor[3] = 1.0f;
            m_config.borderRounding = 6.0f;
            strncpy_s(m_windowTitleBuf, sizeof(m_windowTitleBuf), "Apex Esports Telemetry", _TRUNCATE);
            strncpy_s(m_windowSubtitleBuf, sizeof(m_windowSubtitleBuf), "Competitive Ballistics & Stats", _TRUNCATE);
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

        // Action controls on the right
        ImGui::SetCursorPos(ImVec2(w - 530.0f, 8.0f));
        ImGui::SetNextItemWidth(170.0f);
        const char* templates[] = { "Tactical Overlay", "Obsidian Luxury", "Cyberpunk Neon", "Minimal Streamer", "Esports Pro" };
        if (Widgets::Combo("##TemplateSelect", &m_selectedTemplate, templates, 5)) {
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
            m_inspectorTab = 1; // Switch to compiler tab
            m_isBuilding = true;
            m_buildLogs.push_back("[Solar Studio] Triggered user build pass...");
            
            // Run build in a worker thread to keep GUI 60fps smooth
            std::thread([this]() {
                ProjectGenerator::RunMSBuild(m_config, [this](const std::string& line) {
                    m_buildLogs.push_back(line);
                });
                m_isBuilding = false;
            }).detach();
        }

        ImGui::SetCursorPosY(headerH + 12.0f);
    }

    void StudioApp::RenderToolbox(float width) {
        if (Widgets::BeginCard("##ToolboxCard", "Component Palette", IconType::Sliders, ImVec2(width, 0.0f))) {
            ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "CLICK TO ADD COMPONENT");
            Widgets::Spacing(6.0f);

            if (Widgets::Button("+ Toggle Switch", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "toggle", "New Feature Switch", "Configured in Solar Studio", 0, 0, true });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Linear Slider", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "slider", "Calibrated Dynamic Value", "", 50.0f, 0, false });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Dropdown Combo", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "combo", "Execution Mode Option", "", 0, 0, false });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Action Button", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "button", "Trigger Process", "", 0, 0, false });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Radial Gauge", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "radial_gauge", "Metric Telemetry", "", 78.0f, 0, false });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Dynamic FOV Reticle", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "fov_reticle", "Dynamic FOV Reticle", "", 120.0f, 0, true });
                Audio::PlayClick();
            }
            Widgets::Spacing(4.0f);

            if (Widgets::Button("+ Skin Changer Preview", ImVec2(width - 36.0f, 32.0f), ButtonStyle::Secondary)) {
                m_components.push_back({ "skin_changer", "Cosmetic Customizer", "", 0, 0, false });
                Audio::PlayClick();
            }

            Widgets::Separator();
            if (Widgets::Button("Clear All Components", ImVec2(width - 36.0f, 28.0f), ButtonStyle::Danger)) {
                m_components.clear();
                Audio::PlayClick();
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

            for (size_t i = 0; i < m_components.size(); ++i) {
                auto& comp = m_components[i];
                ImGui::PushID(static_cast<int>(i));

                if (comp.type == "toggle") {
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
                } else if (comp.type == "fov_reticle") {
                    m_previewFov.radius = comp.floatVal;
                    m_previewFov.color = ImVec4(m_config.accentColor[0], m_config.accentColor[1], m_config.accentColor[2], m_config.accentColor[3]);
                    Game::FOVRenderer::RenderPreview("##StudioFovPreview", ImVec2(0, 200.0f), m_previewFov);
                } else if (comp.type == "skin_changer") {
                    Game::SkinChangerPreview::Render("##StudioSkinPreview", ImVec2(0, 320.0f), m_previewSkin, m_dummyInventory);
                }

                Widgets::Spacing(6.0f);
                ImGui::PopID();
            }

            ImGui::EndChild();
            Widgets::EndCard();
        }
    }

    void StudioApp::RenderInspector(float width) {
        if (Widgets::BeginCard("##InspectorCard", "Inspector & Compiler", IconType::Sliders, ImVec2(width, 0.0f))) {
            Widgets::SubTab("Layout & Style", 0, &m_inspectorTab);
            Widgets::SubTab("MSBuild & Exe", 1, &m_inspectorTab);
            Widgets::Spacing(8.0f);

            if (m_inspectorTab == 0) {
                // Properties & Styling Tab
                ImGui::TextColored(ThemeManager::Get().GetPalette().Accent, "PIXEL-PRECISE ROUNDING");
                if (Widgets::SliderFloat("Border Rounding", &m_config.borderRounding, 0.0f, 30.0f, "%.0f", "px")) {
                    ThemeManager::Get().GetStyle().WindowRounding = m_config.borderRounding;
                    ThemeManager::Get().GetStyle().CardRounding = m_config.borderRounding;
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
            } else {
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
                        m_isBuilding = true;
                        std::thread([this]() {
                            ProjectGenerator::RunMSBuild(m_config, [this](const std::string& line) {
                                m_buildLogs.push_back(line);
                            });
                            m_isBuilding = false;
                        }).detach();
                    }
                }

                Widgets::Separator();
                ImGui::TextColored(ThemeManager::Get().GetPalette().TextDisabled, "MSBUILD TERMINAL OUTPUT:");
                
                ImGui::BeginChild("##BuildConsoleChild", ImVec2(0, 220.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
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
            float rightW = 340.0f;
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
