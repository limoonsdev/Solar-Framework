#pragma once
#include <solar/solar.hpp>
#include "project_generator.hpp"
#include <string>
#include <vector>

namespace Solar::Studio {

    struct StudioComponent {
        std::string type; // "button", "slider", "toggle", "combo", "radial_gauge", "chip_selector", "stat_card", "fov_reticle", "skin_changer", "watermark"
        std::string label;
        std::string subtitle;
        float floatVal = 50.0f;
        int intVal = 1;
        bool boolVal = true;
    };

    class StudioApp {
    public:
        static StudioApp& Get();

        void Initialize();
        void Render();

    private:
        StudioApp() = default;

        void RenderHeaderBar();
        void RenderToolbox(float width);
        void RenderCanvas(float width);
        void RenderInspector(float width);
        void ApplyTemplate(int templateIndex);

        ProjectConfig m_config;
        std::vector<StudioComponent> m_components;
        int m_selectedTemplate = 0;
        int m_inspectorTab = 0; // 0=Properties, 1=Compiler & MSBuild

        // Live build log
        std::vector<std::string> m_buildLogs;
        bool m_isBuilding = false;
        char m_projectNameBuf[128] = "SolarCustomApp";
        char m_outputDirBuf[256] = "C:\\SolarProjects\\CustomApp";
        char m_msbuildPathBuf[256] = "MSBuild.exe";
        char m_windowTitleBuf[128] = "Solar Engine Application";
        char m_windowSubtitleBuf[128] = "v1.0.0 • High Performance C++20";

        // Simulated canvas states
        Game::FOVSettings m_previewFov;
        Game::SkinItem m_previewSkin = { "Vandal", "Prime 2.0", Game::SkinRarity::Ultra, 0.021f, 661, true, 1337, ImVec4(1, 0.28f, 0.55f, 1), true };
        std::vector<Game::SkinItem> m_dummyInventory;
    };

} // namespace Solar::Studio
