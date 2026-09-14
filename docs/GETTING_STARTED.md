# 🚀 Getting Started with Solar Framework

**Solar Framework** is an ultra-modern, production-grade Dear ImGui framework built with C++20 and DirectX 11, inspired by the high-end UI designs of **PastOwl** (Dribbble/Behance) and the legacy of **LiquidDX11**.

Designed specifically for cheat developers, loader engineers, and game tool creators who want an unmatched, luxurious dark aesthetic without writing tedious boilerplate rendering code.

---

## 🛠️ Prerequisites

- **Operating System**: Windows 10 or Windows 11 (64-bit)
- **Compiler**: Visual Studio 2022 / 2026 (MSVC v143+ or clang-cl) with C++20 support
- **Build System**: CMake 3.16+
- **DirectX**: Windows SDK (DirectX 11 & DXGI headers are included with Windows SDK)

---

## 📦 Building the Standalone Demo

Clone or extract the repository, then configure and build using CMake:

```powershell
# 1. Configure the project for 64-bit Windows
cmake -B build -A x64

# 2. Build Release configuration
cmake --build build --config Release
```

The compiled standalone executable will be generated at:
```
build/Release/solar_demo.exe
```

Run `solar_demo.exe` to explore all interactive widgets, animations, live 2D preview, and theme presets.

---

## 💡 Quick Integration into Your Project

To integrate Solar Framework into your own project (internal DLL or external overlay):

### 1. Add Include Directories
Add the following include paths to your project:
```
Solar Framework/include
Solar Framework/vendor/imgui
Solar Framework/vendor/imgui/backends
```

### 2. Link Dependencies
Ensure your project links with:
- `d3d11.lib`
- `dxgi.lib`
- `dwmapi.lib`

### 3. Initialize in your ImGui setup
```cpp
#include <solar/solar.hpp>

// Call once after ImGui::CreateContext() and backend initialization:
Solar::Initialize();
```

### 4. Render in your Frame Loop
```cpp
void RenderMyCheatMenu() {
    // 1. Tick Solar Framework (handles toast notifications & timers)
    Solar::NewFrame();

    // 2. Render optional HUD Watermark
    Solar::WatermarkInfo wm;
    wm.frameworkName = "SOLAR";
    wm.version = "v1.0";
    wm.username = "SolarDev";
    Solar::Watermark::Render(wm);

    // 3. Render your custom window
    static bool open = true;
    if (Solar::Widgets::BeginWindow("Solar Cheat", &open, ImVec2(890, 600))) {
        Solar::Widgets::RenderTitlebar("SOLAR", "PRIVATE BUILD", &open);

        // Sidebar Navigation
        static int currentTab = 0;
        Solar::Widgets::BeginSidebar(190.0f);
        {
            Solar::Widgets::SidebarTab("Aimbot", Solar::IconType::Crosshair, 0, &currentTab);
            Solar::Widgets::SidebarTab("Visuals", Solar::IconType::Eye, 1, &currentTab);
            Solar::Widgets::SidebarTab("Settings", Solar::IconType::Sliders, 2, &currentTab);
        }
        Solar::Widgets::EndSidebar();

        ImGui::SameLine();

        // Main Tab Area
        if (currentTab == 0) {
            static bool aimbot = true;
            static float fov = 12.0f;
            static int aimKey = 0x05; // M5

            Solar::Widgets::Toggle("Enable Aimbot", &aimbot);
            Solar::Widgets::SliderFloat("Field of View", &fov, 1.0f, 90.0f, "%.1f", "°");
            Solar::Widgets::Keybind("Aim Key", &aimKey);
        }
    }
    Solar::Widgets::EndWindow();
}
```
