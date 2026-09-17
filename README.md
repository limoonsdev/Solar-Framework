# ☀️ Solar Framework

<div align="center">

```
  ____   ___  _        _    ____     _____ ____      _    __  __ _______        _____  ____  _  __
 / ___| / _ \| |      / \  |  _ \   |  ___|  _ \    / \  |  \/  | ____\ \      / / _ \|  _ \| |/ /
 \___ \| | | | |     / _ \ | |_) |  | |_  | |_) |  / _ \ | |\/| |  _|  \ \ /\ / / | | | |_) | ' / 
  ___) | |_| | |___ / ___ \|  _ <   |  _| |  _ <  / ___ \| |  | | |___  \ V  V /| |_| |  _ <| . \ 
 |____/ \___/|_____/_/   \_\_| \_\  |_|   |_| \_\/_/   \_\_|  |_|_____|  \_/\_/  \___/|_| \_\_|\_\
```

**Production-grade C++20 Dear ImGui & DirectX 11/12 GUI & Simulation Engine for Games, Overlays, Telemetry, and Interactive Tools**

[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen?style=flat-square&logo=github)](https://github.com/mp0b/Solar-Framework)
[![Version](https://img.shields.io/badge/Version-v1.0.2--dev-orange?style=flat-square)](https://github.com/mp0b/Solar-Framework/tree/dev)
[![Standard](https://img.shields.io/badge/Standard-C%2B%2B20-blue?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/20)
[![Graphics](https://img.shields.io/badge/Backend-DirectX%2011%20%2F%2012-informational?style=flat-square&logo=windows)](https://learn.microsoft.com/en-us/windows/win32/direct3d11/)
[![GUI Core](https://img.shields.io/badge/Core-Dear%20ImGui%20Docking-purple?style=flat-square)](https://github.com/ocornut/imgui)
[![License](https://img.shields.io/badge/License-MIT-lightgrey?style=flat-square)](LICENSE)

</div>

---

## 📌 Index

1. [Overview](#-overview)
2. [What's New in v1.0.2](#-whats-new-in-v102)
3. [Solar Studio [BETA v0.1.2]](#-solar-studio-beta-v012)
4. [Solar-Render & Crash-Proof UE4/UE5 Engine](#-solar-render--crash-proof-ue4ue5-engine)
5. [Creamy Tactile Audio Engine](#-creamy-tactile-audio-engine)
6. [Dynamic FOV & Reticle Engine (`FOVRenderer`)](#-dynamic-fov--reticle-engine-fovrenderer)
7. [Cosmetic & Skin Changer Suite (`SkinChangerPreview`)](#-cosmetic--skin-changer-suite-skinchangerpreview)
8. [Visuals & Simulation Subsystems](#-visuals--simulation-subsystems)
9. [UI Widget Suite (100+ Controls)](#-ui-widget-suite-100-controls)
10. [Quickstart & Build Instructions](#-quickstart--build-instructions)
11. [Repository Layout](#-repository-layout)

---

## 📖 Overview

**Solar Framework** is an engineered, industrial-strength C++20 user interface and simulation engine built on top of **Dear ImGui (docking branch)** and native **DirectX 11 & DirectX 12**.

Designed for telemetry dashboards, game tools, simulation software, and high-performance interactive graphics:
- **Zero-DWM Blur**: Physical 1:1 Per-Monitor DPI Aware v2 scaling with 3x subpixel font oversampling.
- **Solar-Render Pipeline**: Full 6-stage DirectX 11 pipeline isolation protecting Vertex, Pixel, Geometry, Hull, Domain, and Compute shaders along with 14 constant buffer slots for zero-crash stability in Unreal Engine 4/5 and modern game engines.
- **Solar Studio [BETA v0.1.2]**: Built-in native C++20 studio application with an interactive drag-and-drop widget palette, live pixel-accurate canvas, pre-designed templates, and MSBuild code generator.
- **Creamy Tactile Audio**: Rich harmonic in-memory sound synthesis replicating lubricated premium mechanical switches (warm pops, smooth glides, velvet triad chimes).
- **Motion Physics Dynamics (`Solar::Anim`)**: 4th-order Runge-Kutta (RK4) numerical spring solver, multi-dimensional `Spring2D`, game-camera `SmoothDamp`, and multi-keyframe `Timeline` sequencer.

---

## 🚀 What's New in v1.0.2

- **Solar Studio [BETA v0.1.2]**: Standalone native IDE tool located in `Solar Studio [BETA v0.1.2]/`. Build your interfaces visually, configure pixel rounding from 0px to 30px, adjust palettes, and compile directly with MSBuild.
- **UE4 / UE5 Crash Prevention**: Complete rewrite of `DX11StateGuard` to unbind and restore all 6 shader stages (VS, PS, GS, HS, DS, CS), 14 constant buffers, 16 SRVs/samplers, and Compute UAVs.
- **FrameworkMode API**: Seamless switching between `FrameworkMode::External` (desktop window) and `FrameworkMode::Internal` (in-process hook DLL).
- **Creamy Acoustic Synthesis**: Completely revamped sound bank replacing harsh tones with warm, low-resonance linear mechanical switch acoustics.
- **Dynamic FOV Engine (`FOVRenderer`)**: Radial gradient falloff, multi-pass Gaussian glow bloom, cyber tick notches, and target acquisition states.
- **Cosmetic & Skin Changer Suite (`SkinChangerPreview`)**: Weapon inspect card with silhouette rendering, procedural holographic shimmer, float wear degradation bar, and rarity tiers.

---

## 🎨 Solar Studio [BETA v0.1.2]

`Solar Studio` is a dedicated standalone C++20 / DirectX 11 application for designing and compiling Solar Framework user interfaces.

### Core Features:
- **Component Palette**: Click to add interactive widgets directly to the layout (Buttons, Sliders, Toggles, Combos, Radial Gauges, Chip Selectors, Watermarks, FOV Circles, and Skin Changers).
- **Live WYSIWYG Canvas**: Real-time blueprint canvas showing your interface with pixel-precise borders, ambient drop shadows, and responsive controls.
- **Property Inspector**:
  - Border Rounding Slider: Adjust corner curvature with 1px precision (0px sharp to 30px rounded).
  - Theme Nuancier: Real-time RGB calibration for accents, card backgrounds, and specular sheen.
  - Typography: Live editing of window titles, subtitles, and labels.
- **Pre-Built Design Templates**:
  - *Tactical Overlay* (Cyber Blue & High Contrast)
  - *Obsidian Luxury* (Deep Gold & Smoked Glass)
  - *Cyberpunk 2077 Neon* (Vivid Pink & Dark Grid)
  - *Minimalist Streamer HUD* (Emerald Green & Clean Lines)
  - *Esports Pro Telemetry* (Safety Orange & Angular Accents)
- **MSBuild Compiler Pipeline**:
  - Auto-detection of `MSBuild.exe` across Visual Studio 2022 and 2019 installations.
  - Generates production-ready C++20 `main.cpp`, `CMakeLists.txt`, and project configurations.
  - Targets either **Standalone Executable (.exe)** or **Internal In-Process Hook (.dll)**.
  - Built-in live compilation terminal displaying real-time compiler logs.

---

## 🛡️ Solar-Render & Crash-Proof UE4/UE5 Engine

In complex rendering engines like **Unreal Engine 4 & 5** (Fortnite, Arc Raiders, Valorant), rendering an in-process ImGui menu typically crashes the GPU driver (`DXGI_ERROR_DEVICE_REMOVED`) due to residual geometry or compute shaders.

**Solar-Render** resolves this by completely isolating the DirectX 11 pipeline:
1. **6-Stage Shader Preservation**: Captures and restores `VS`, `PS`, `GS`, `HS`, `DS`, and `CS`.
2. **Hull/Domain/Geometry Unbinding**: Explicitly detaches active tessellation and geometry shaders before ImGui draw calls.
3. **14 Constant Buffer Slots**: Captures all 14 D3D11 constant buffers (`D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT`) across Vertex, Pixel, and Geometry stages.
4. **Compute Shader UAV Isolation**: Captures and restores up to 8 Unordered Access Views.

```cpp
#include <solar/solar.hpp>
#include <d3d11.h>

void RenderHook(IDXGISwapChain* pSwapChain) {
    static Solar::Hook::DX11StateGuard stateGuard;
    
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device);
    device->GetImmediateContext(&context);

    // 1. Full 6-Stage DirectX 11 Pipeline Isolation
    stateGuard.Capture(context);

    // 2. Solar Rendering Pass
    Solar::NewFrame();
    bool open = true;
    if (Solar::Widgets::BeginWindow("##Overlay", &open)) {
        Solar::Widgets::RenderTitlebar("Telemetry HUD", "UE5 Engine Compatible", &open);
        // Render UI...
        Solar::Widgets::EndWindow();
    }

    // 3. Clean State Restoration
    stateGuard.Restore();

    context->Release();
    device->Release();
}
```

---

## 🔊 Creamy Tactile Audio Engine

Solar synthesizes sound procedurally in RAM via 16-bit 44.1 kHz PCM with zero external audio assets:
- **Creamy Mechanical Click**: 440 Hz -> 240 Hz warm low-mid sweep replicating lubricated linear switch bottom-out.
- **Creamy Toggle Slide**: Warm rising switch glide (340 Hz -> 520 Hz).
- **Muted Bottom-Out Thock**: Gentle descending tactile feedback (480 Hz -> 260 Hz).
- **Velvet Triad Chime**: Warm harmonic chord (Ab4, C5, Eb5) for non-piercing notifications.
- **Creamy Slider Micro-Tap**: 380 Hz tactile buttery micro-step without high-pitch clicks.

```cpp
#include <solar/solar.hpp>

Solar::Audio::PlayClick();        // Creamy switch pop
Solar::Audio::PlayToggle(true);   // Rising switch slide
Solar::Audio::PlayNotification(); // Warm velvet chime
Solar::Audio::PlaySliderTick();   // Micro-tap
```

---

## 🎯 Dynamic FOV & Reticle Engine (`FOVRenderer`)

Vector-smooth field-of-view indicators and tactical reticles:
- **Radial Gradient Disk**: Volumetric colored falloff fading into the center.
- **Multi-Pass Gaussian Bloom**: Radiant glow rings expanding around the perimeter.
- **Cyber Tick Notches**: Cardinal and diagonal ticks with distance/degree badges.
- **Dynamic Breathing Pulse**: Smooth sine-wave rhythmic breathing.
- **Target Lock-On**: Dynamic color shift and rapid status feedback upon target acquisition.

```cpp
#include <solar/solar.hpp>

Solar::Game::FOVSettings fovSettings;
fovSettings.radius = 125.0f;
fovSettings.enableGlow = true;
fovSettings.enableCyberMarks = true;

// Render standalone preview or on custom draw list
Solar::Game::FOVRenderer::RenderPreview("##FovPreview", ImVec2(320, 320), fovSettings);
```

---

## 🗡️ Cosmetic & Skin Changer Suite (`SkinChangerPreview`)

Comprehensive weapon cosmetic customization widget:
- **Vector Silhouette Rendering**: Scalable polygonal rendering of rifles, snipers, and curved blades.
- **Procedural Shimmer**: Dynamic holographic sheen sweeping across the showcase card.
- **Rarity Tier Badging**: Select (Blue), Deluxe (Green), Premium (Purple), Ultra (Pink), Exclusive (Gold).
- **Float Wear Degradation Bar**: Visual indicator covering Factory New, Minimal Wear, Field-Tested, Well-Worn, and Battle-Scarred tiers with real-time needle indicator.
- **StatTrak Module**: Integrated kill counter with illuminated LED styling.

```cpp
#include <solar/solar.hpp>

Solar::Game::SkinItem activeSkin;
activeSkin.weapon = "Vandal";
activeSkin.skinName = "Prime 2.0";
activeSkin.rarity = Solar::Game::SkinRarity::Ultra;
activeSkin.floatWear = 0.0210f; // Factory New

Solar::Game::SkinChangerPreview::Render("##SkinCustomizer", ImVec2(0, 420), activeSkin, inventory);
```

---

## ⚡ Quickstart & Build Instructions

### Prerequisites
- Windows 10 / 11 64-bit
- Visual Studio 2022 (MSVC v143 / v144) with C++20 toolset
- CMake 3.16 or newer

### Build Everything (Library, Demo, and Solar Studio)
```powershell
# Clone repository
git clone https://github.com/mp0b/Solar-Framework.git
cd "Solar Framework"

# Generate build files (x64 Release)
cmake -B build -A x64

# Compile static library, demonstration app, and Solar Studio
cmake --build build --config Release
```

The build produces:
- `build/Release/solar_lib.lib`: Core static library.
- `build/Release/solar_demo.exe`: Full feature demonstration application.
- `build/Release/solar_studio.exe`: Standalone Solar Studio [BETA v0.1.2] IDE.

---

## 📂 Repository Layout

```
Solar Framework/
├── CMakeLists.txt                      # Root build configuration
├── README.md                           # Documentation & specifications
├── Solar Studio [BETA v0.1.2]/        # Standalone Native Studio Application
│   ├── main.cpp                        # Studio WinMain & DirectX 11 runner
│   ├── studio_app.hpp / .cpp           # Studio canvas, toolbox, inspector
│   └── project_generator.hpp / .cpp    # MSBuild runner & C++20 code generator
├── include/solar/
│   ├── solar.hpp                       # Umbrella master include
│   ├── core/                           # Math, Color, Timer, Logger, Crypto, Types
│   ├── render/                         # D3D11/D3D12 renderers, ImGuiExt, GlowEngine
│   ├── anim/                           # RK4 Spring, Spring2D, SmoothDamp, Timeline
│   ├── audio/                          # WaveSynth, SoundEffects, AudioEngine
│   ├── theme/                          # Palette, Style, Presets, ThemeManager
│   ├── ui/                             # Window, Titlebar, Sidebar, Card, Groupbox, Modal
│   ├── widgets/                        # 100+ UI controls (Slider, Toggle, Combo, etc.)
│   ├── game/                           # FOVRenderer, SkinChanger, ESPPreview, OBBRenderer
│   ├── hook/                           # DX11StateGuard (6-stage isolation), HookBridge
│   └── notifications/                  # Toast & NotificationSystem
├── src/                                # Subsystem implementation sources
├── demo/                               # Demonstration application
└── vendor/imgui/                       # Dear ImGui docking source distribution
```

---

<div align="center">
  <sub>Developed with precision by <b>SolarDev</b> &bull; Licensed under the MIT License</sub>
</div>
