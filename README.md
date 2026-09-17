# Solar Framework

**Production-grade C++20 Dear ImGui & DirectX 11/12 GUI & Simulation Engine**

[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen?style=flat-square&logo=github)](https://github.com/mp0b/Solar-Framework)
[![Version](https://img.shields.io/badge/Version-v1.1.0-blue?style=flat-square)](https://github.com/mp0b/Solar-Framework/tree/release)
[![Standard](https://img.shields.io/badge/Standard-C%2B%2B20-blue?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/20)
[![Graphics](https://img.shields.io/badge/Backend-DirectX%2011%20%2F%2012-informational?style=flat-square&logo=windows)](https://learn.microsoft.com/en-us/windows/win32/direct3d11/)
[![GUI Core](https://img.shields.io/badge/Core-Dear%20ImGui%20Docking-purple?style=flat-square)](https://github.com/ocornut/imgui)
[![License](https://img.shields.io/badge/License-MIT-lightgrey?style=flat-square)](LICENSE)

---

## Index

1. [Overview](#overview)
2. [What's New in v1.1.0](#whats-new-in-v110)
3. [Core Architecture](#core-architecture)
4. [Solar-Render & Crash-Proof Pipeline](#solar-render--crash-proof-pipeline)
5. [SolarWorld: Unreal Engine 5 Sandbox](#solarworld-unreal-engine-5-sandbox)
6. [Component & Widget Suite](#component--widget-suite)
7. [Tactile Audio Engine](#tactile-audio-engine)
8. [Quickstart & Build Instructions](#quickstart--build-instructions)
9. [Repository Layout](#repository-layout)
10. [License](#license)

---

## Overview

**Solar Framework** is an engineered C++20 user interface and simulation framework built on **Dear ImGui (docking branch)** with native **DirectX 11** and **DirectX 12** backends.

Targeting game overlays, telemetry dashboards, developer tools, and real-time graphics applications, Solar Framework prioritizes rendering isolation, subpixel typography, low CPU/GPU overhead, and native stream protection.

### Key Highlights
- **High-DPI Per-Monitor v2 Awareness**: True 1:1 physical pixel rendering with subpixel font rasterization, eliminating Windows DWM bilinear scaling blur.
- **Solar-Render Pipeline**: Full 6-stage DirectX 11 pipeline isolation protecting Vertex, Pixel, Geometry, Hull, Domain, and Compute shaders along with 14 constant buffer slots for crash-proof in-process hooking.
- **Hardware Streamproof Protection**: Native integration with Windows Display Affinity (`WDA_EXCLUDEFROMCAPTURE 0x00000011` and fallback `0x00000001`), hiding the overlay from OBS, Discord, Medal, and screen capture tools.
- **SolarWorld UE5 Testbed**: Standalone 3D simulation sandbox emulating Unreal Engine 5 internals with an interactive developer console (`Delete` / `~`), Slate pause menu (`Escape`), and tactical HUD.
- **Universal Interactive Resizing**: Built-in corner resize grips on all auxiliary windows and satellite panels.
- **Synthesized Tactile Audio**: Procedural in-memory sound synthesis replicating lubricated mechanical switch acoustics with zero external audio assets.

---

## What's New in v1.1.0

- **SolarWorld UE5 Developer Console**: Drop-down command terminal triggered by `Delete` / `Suppr` or `~` (Tilde) with command history, autocomplete, and commands (`stat fps`, `stat unit`, `stat hud`, `viewmode wireframe/lit`, `r.SetRes`, `fov`, `gamma`, `slomo`, `god`, `teleport`, `spawn`, `modules`, `inject`).
- **SolarWorld Slate Pause Menu**: Unreal Engine-styled frosted glass menu triggered by `Escape` offering session controls, viewport settings, actor management, and live injected DLL inspection.
- **In-Game Tactical HUD**: Real-time compass tape synchronized with camera yaw, precision 4-axis crosshair, vitality gauges, weapon ammo HUD, and 3D-to-2D world space target nameplates.
- **Atmospheric Sky & Scene Lighting**: Procedural celestial gradient with sun disc glow, Blinn-Phong specular lighting model, tactical cover crates, elevated platforms, and ground contact shadows.
- **Universal Window Resize Grips**: Integrated `RenderResizeGrip` across `KeybindList`, `SpectatorList`, `RadarWindow`, and `SatelliteWindow`.
- **Streamproof Mode**: Integrated hardware capture masking switchable at runtime.
- **Continuous Rotating Borders**: Full perimeter arc-length parameterization ensuring even and continuous distribution around all four rounded sides.
- **Render Engine Optimizations**: Streamlined 3-pass quadratic falloff shadow casting and distance-squared constellation particle system delivering 300+ FPS.
- **Automated Layout Auditor**: Built-in `LayoutAuditor` verifying margin compliance (>=8px) and exporting `solar_layout_audit.log`.
- **Automated Showcase**: Command-line flag `--showcase-all` generating sequential high-resolution verification screenshots across all features.

---

## Core Architecture

### Display Affinity & Streamproof Mode
Solar Framework provides direct integration with the Windows Desktop Window Manager (DWM) display affinity API. When enabled, the application window remains interactive to the user but is excluded from desktop duplicate engines, window capture, and hook capture APIs:

```cpp
#include <solar/solar.hpp>

// Toggle hardware capture exclusion (OBS, Discord, Medal)
bool streamproof = true;
Solar::ApplyWindowStreamproof(streamproof);
```

### High-Precision Motion Physics
The animation subsystem (`Solar::Anim`) provides physical motion solvers:
- **RK4 Spring Solver**: 4th-order Runge-Kutta numerical integration for natural bounce and spring dynamics.
- **Spring2D**: Coupled dual-axis spring physics for floating windows and cursor trailing.
- **SmoothDamp**: Critically damped spring-damper tracking algorithm.
- **Timeline**: Multi-track keyframe sequencer for complex UI transitions.

---

## Solar-Render & Crash-Proof Pipeline

In modern engines like **Unreal Engine 4 & 5**, rendering an in-process ImGui menu typically causes driver crashes (`DXGI_ERROR_DEVICE_REMOVED`) due to residual compute shaders or unmanaged state changes.

**Solar-Render** resolves this by isolating the DirectX 11 pipeline:
1. **6-Stage Shader Preservation**: Captures and restores `VS`, `PS`, `GS`, `HS`, `DS`, and `CS`.
2. **Hull/Domain/Geometry Unbinding**: Explicitly detaches active tessellation and geometry shaders before ImGui draw calls.
3. **14 Constant Buffer Slots**: Captures all 14 D3D11 constant buffers across Vertex, Pixel, and Geometry stages.
4. **Compute UAV Isolation**: Captures and restores Unordered Access Views.

```cpp
#include <solar/solar.hpp>
#include <d3d11.h>

void RenderHook(IDXGISwapChain* pSwapChain) {
    static Solar::Hook::DX11StateGuard stateGuard;

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device);
    device->GetImmediateContext(&context);

    // 1. Capture and isolate full pipeline
    stateGuard.Capture(context);

    // 2. Solar Rendering Pass
    Solar::NewFrame();
    bool open = true;
    if (Solar::Widgets::BeginWindow("Tactical Overlay", &open)) {
        Solar::Widgets::RenderTitlebar("SOLAR", "v1.1.0", &open);
        // Render widgets...
        Solar::Widgets::EndWindow();
    }

    // 3. Clean State Restoration
    stateGuard.Restore();

    context->Release();
    device->Release();
}
```

---

## SolarWorld: Unreal Engine 5 Sandbox

`solar_world.exe` is a standalone 3D DirectX 11 sandbox that emulates Unreal Engine 5 internal structures. It allows developers to test DLL injectors, memory scanners, and overlay hooks in a realistic environment without risking game bans or client crashes.

### Exported Symbols
```cpp
extern "C" {
    __declspec(dllexport) UWorld* GWorld;
    __declspec(dllexport) void* GObjects;
    __declspec(dllexport) void* GNames;
    __declspec(dllexport) UWorld* GetUWorld();
    __declspec(dllexport) IDXGISwapChain* g_ExportedSwapChain;
}
```

### Unreal Engine Developer Console
Press **Delete** (`Suppr`) or **~** (Tilde) to open the console:

| Command | Description |
|---|---|
| `help` | Lists all available console commands |
| `stat fps` | Toggles the real-time framerate display |
| `stat unit` | Toggles CPU, GPU, and frame time telemetry |
| `stat hud` | Toggles the tactical combat HUD |
| `viewmode wireframe` | Renders the scene geometry in wireframe mode |
| `viewmode lit` | Restores standard lit shading |
| `r.SetRes <W>x<H>` | Resizes the viewport window (e.g. `r.SetRes 1920x1080`) |
| `fov <degrees>` | Updates camera field of view (30° to 140°) |
| `gamma <value>` | Adjusts scene exposure and brightness |
| `slomo <factor>` | Adjusts game time dilation (e.g. `slomo 0.5`) |
| `god` | Toggles infinite health on actors |
| `teleport` | Resets camera location to spawn origin |
| `spawn` | Spawns an additional target dummy actor |
| `modules` | Lists all injected non-baseline DLL modules |
| `inject <dll>` | Loads a DLL module via `LoadLibraryA` |
| `cls` / `clear` | Clears console log buffer |
| `quit` / `exit` | Exits the simulation |

### Unreal Engine Pause Menu
Press **Escape** to pause simulation and access:
- **Session Controls**: Resume simulation, spawn dummies, teleport to spawn.
- **Graphics & Viewport**: FOV slider, mouse sensitivity, render mode, atmosphere toggle, gamma.
- **Injected Modules Inspector**: Real-time table of detected external modules with base memory addresses.

### Tactical Combat HUD
- **Top Compass Tape**: Cardinal marks (`N`, `NE`, `E`, `SE`, `S`, `SW`, `W`, `NW`) and digital bearing synchronized with camera yaw.
- **Crosshair Reticle**: High-precision 4-axis reticle with center dot.
- **Vitality & Ammo Gauges**: Health bar (100 HP), Armor (100), Weapon label (`VANDAL PRIME`), ammo counter (`25 / 75`).
- **3D World Nameplates**: 3D-to-2D projected nameplates over target bots with health bars, team indicators, and distance in meters.

---

## Component & Widget Suite

Solar Framework provides an extensive set of custom ImGui widgets:

- **Cards & Groupboxes**: Rounded cards with customizable header icons and accent borders.
- **Buttons**: `Primary`, `Secondary`, `Danger`, and `Ghost` button styles.
- **Inputs**: `SearchInput` with clear button, `InputText` with floating labels.
- **Navigation**: Segmented pill controls, subtabs, and breadcrumb navigation history.
- **Sliders & Gauges**: Standard sliders, dual-thumb `RangeSlider`, and circular `RadialGauge`.
- **Keybind Selectors**: Interactive hotkey binder with `Hold`, `Toggle`, and `Always` modes.
- **Watermarks**: HUD status bar with 5 styles (`CyberBar`, `MinimalPill`, `NeonTerminal`, `HoloBadge`, `DiscreteCorner`) and clickable browser URL launch.
- **Command Palette (`Ctrl + P`)**: Floating search modal with category filtering and instant command execution.
- **ESports Kill Banner**: Animated frag popup with weapon icons, victim name, and killstreak badges.
- **Custom Cursors**: 5 hardware-smooth cursor presets (`ClassicVector`, `CyberTriangle`, `CrosshairDot`, `SciFiChevron`, `MinimalNeedle`).
- **Telemetry Visualizers**: Spline graph with gradient fill, KPI stat cards with mini sparklines, and 2D tactical radar.

---

## Tactile Audio Engine

Solar synthesizes sound procedurally in memory via 16-bit 44.1 kHz PCM without external audio files:
- **Mechanical Click**: Warm low-mid sweep replicating lubricated linear switch bottom-out.
- **Toggle Slide**: Rising switch glide (340 Hz -> 520 Hz).
- **Muted Thock**: Gentle descending tactile feedback (480 Hz -> 260 Hz).
- **Velvet Triad Chime**: Warm harmonic chord for non-intrusive notifications.
- **Slider Micro-Tap**: Subtle tactile micro-step feedback.

```cpp
#include <solar/solar.hpp>

Solar::Audio::PlayClick();        // Mechanical switch pop
Solar::Audio::PlayToggle(true);   // Rising switch slide
Solar::Audio::PlayNotification(); // Velvet harmonic chime
Solar::Audio::PlaySliderTick();   // Micro-tap
```

---

## Quickstart & Build Instructions

### Prerequisites
- Windows 10 / 11 64-bit
- Visual Studio 2022 (MSVC v143 / v144) with C++20 toolset
- CMake 3.16 or newer

### Build Everything
```powershell
# Clone repository
git clone https://github.com/mp0b/Solar-Framework.git
cd "Solar Framework"

# Generate build files (x64 Release)
cmake -B build -A x64

# Compile static library, demonstration app, and SolarWorld sandbox
cmake --build build --config Release
```

### Build Artifacts
The build produces:
- `build/Release/solar_lib.lib`: Core static framework library.
- `build/Release/solar_demo.exe`: Full feature demonstration application.
- `build/Release/solar_world.exe`: Unreal Engine 5 simulated testbed sandbox.

### Running Applications
```powershell
# Run feature demonstration
.\build\Release\solar_demo.exe

# Run automated showcase (exports 8 screenshots to screenshots/)
.\build\Release\solar_demo.exe --showcase-all

# Run Unreal Engine 5 simulated sandbox
.\build\Release\solar_world.exe
```

---

## Repository Layout

```
Solar Framework/
├── CMakeLists.txt                      # Root CMake build configuration
├── README.md                           # Technical documentation
├── patchmess.txt                       # Global release patchnotes
├── include/solar/
│   ├── solar.hpp                       # Umbrella master include
│   ├── core/                           # Math, Color, Timer, Logger, Crypto, Types
│   ├── render/                         # D3D11/D3D12 renderers, ImGuiExt, FontManager, GlowEngine
│   ├── anim/                           # RK4 Spring, Spring2D, SmoothDamp, Timeline
│   ├── audio/                          # WaveSynth, SoundEffects, AudioEngine
│   ├── theme/                          # Palette, Style, Presets, ThemeManager
│   ├── ui/                             # Window, Titlebar, Sidebar, Card, CommandPalette, KillBanner
│   ├── widgets/                        # Complete UI widget suite (100+ controls)
│   ├── game/                           # FOVRenderer, SkinChanger, Watermark, RadarWindow
│   ├── hook/                           # DX11StateGuard (6-stage isolation), HookBridge
│   ├── security/                       # PatternScanner, HwidSpoofer, MemoryTools
│   └── tools/                          # LayoutAuditor, Profiler, Inspector
├── src/                                # Subsystem implementation sources
├── demo/                               # Full demonstration application
├── tools/solar_world/                  # Unreal Engine 5 simulated sandbox testbed
└── vendor/imgui/                       # Dear ImGui docking source distribution
```

---

## License

Solar Framework is licensed under the [MIT License](LICENSE).
You are free to use, modify, distribute, and integrate this software in commercial and non-commercial projects.
