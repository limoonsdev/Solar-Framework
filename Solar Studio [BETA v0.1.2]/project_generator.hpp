#pragma once
#include <string>
#include <vector>
#include <functional>

namespace Solar::Studio {

    enum class TargetOutputType {
        StandaloneExe = 0, // Standalone Win32 / DirectX 11 Executable (.exe)
        InternalDll = 1    // In-process SwapChain Hook DLL (.dll) with Solar-Render DX11 isolation
    };

    struct ProjectConfig {
        std::string projectName = "SolarCustomApp";
        TargetOutputType outputType = TargetOutputType::StandaloneExe;
        std::string outputDir = "C:\\SolarProjects\\CustomApp";
        std::string msbuildPath = "MSBuild.exe";
        std::string frameworkRootDir = "C:\\Users\\bruck\\Downloads\\Solar Framework";
        
        // Visual styling configuration for generated code
        float borderRounding = 10.0f;
        float windowWidth = 960.0f;
        float windowHeight = 640.0f;
        float accentColor[4] = { 0.20f, 0.65f, 1.00f, 1.00f };
        float cardColor[4] = { 0.07f, 0.09f, 0.13f, 0.95f };
        std::string windowTitle = "Solar Engine Application";
        std::string windowSubtitle = "v1.0.0 • High Performance C++20";
        bool enableParticles = true;
        bool enableGlow = true;
        bool enableCreamyAudio = true;
    };

    class ProjectGenerator {
    public:
        // Try to auto-detect MSBuild.exe on the local Windows installation
        static std::string AutoDetectMSBuild();

        // Generate complete C++20 source files and build configurations
        static bool GenerateProjectFiles(const ProjectConfig& config, std::string& outErrorMessage);

        // Build the project using MSBuild.exe and stream output to a callback
        static bool RunMSBuild(const ProjectConfig& config, const std::function<void(const std::string&)>& logCallback);
    };

} // namespace Solar::Studio
