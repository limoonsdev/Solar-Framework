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
5. [Component & Widget Suite](#component--widget-suite)
6. [Tactile Audio Engine](#tactile-audio-engine)
7. [Quickstart & Build Instructions](#quickstart--build-instructions)
8. [Repository Layout](#repository-layout)
9. [License](#license)

---

## Overview

**Solar Framework** is an engineered C++20 user interface and simulation framework built on **Dear ImGui (docking branch)** with native **DirectX 11** and **DirectX 12** backends.

Targeting game overlays, telemetry dashboards, developer tools, and real-time graphics applications, Solar Framework prioritizes rendering isolation, subpixel typography, low CPU/GPU overhead, and native stream protection.

### Key Highlights
- **High-DPI Per-Monitor v2 Awareness**: True 1:1 physical pixel rendering with subpixel font rasterization, eliminating Windows DWM bilinear scaling blur.
- **Solar-Render Pipeline**: Full 6-stage DirectX 11 pipeline isolation protecting Vertex, Pixel, Geometry, Hull, Domain, and Compute shaders along with 14 constant buffer slots for crash-proof in-process hooking.
- **Hardware Streamproof Protection**: Native integration with Windows Display Affinity (`WDA_EXCLUDEFROMCAPTURE 0x00000011` and fallback `0x00000001`), hiding the overlay from OBS, Discord, Medal, and screen capture tools.
- **Interactive Watermark & Web Integration**: HUD watermark supporting 5 visual styles (`CyberBar`, `MinimalPill`, `NeonTerminal`, `HoloBadge`, `DiscreteCorner`) and native browser launch via `ShellExecuteA`.
- **Command Palette (`Ctrl + P`)**: Smooth floating search and command launcher with category filtering, shortcuts, and instant action execution.
- **Universal Interactive Resizing**: Built-in corner resize grips (`RenderResizeGrip`) on all auxiliary windows and satellite panels.
- **Synthesized Tactile Audio**: Procedural in-memory sound synthesis replicating lubricated mechanical switch acoustics with zero external audio assets.

---

## What's New in v1.1.0

- **Universal Window Resize Grips**: Integrated `RenderResizeGrip` across `KeybindList`, `SpectatorList`, `RadarWindow`, and `SatelliteWindow`.
- **Streamproof Mode**: Integrated hardware capture masking switchable at runtime with automatic `SetWindowDisplayAffinity` management.
- **Interactive Watermark**: 5 custom visual presets with real-time framerate telemetry and clickable external link launch.
- **Modernized User Card & Context Menu**: Real-time pulsing radar ring, VIP badge, streamproof switch, and secure machine HWID clipboard copy.
- **Command Palette (`Ctrl + P`)**: Smooth keyboard-driven launcher with background dimming and direct feature execution.
- **ESports Frag Banner**: High-intensity kill notification banner with smooth spring animations, weapon icons, and killstreak multipliers.
- **5 Custom Cursor Styles**: Hardware-interpolated cursors (`ClassicVector`, `CyberTriangle`, `CrosshairDot`, `SciFiChevron`, `MinimalNeedle`).
- **Continuous Rotating Borders**: Full perimeter arc-length parameterization ensuring even and continuous distribution around all four rounded sides.
- **FPS Cap Limiter & Chroma Mode**: Configurable frame rate targets (VSync, 30, 60, 120, 144, 240, 360 FPS, Unlimited) and dynamic RGB Chroma spectrum mode.
- **6 Premium Theme Presets**: Onyx Chroma, Synthwave Sunset, Glacier Titanium, Kintsugi Gold, Nebula Void, and Blood Moon.
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

# Compile static library and demonstration app
cmake --build build --config Release
```

### Build Artifacts
The build produces:
- `build/Release/solar_lib.lib`: Core static framework library.
- `build/Release/solar_demo.exe`: Full feature demonstration application.

### Running Applications
```powershell
# Run feature demonstration
.\build\Release\solar_demo.exe

# Run automated showcase (exports 8 screenshots to screenshots/)
.\build\Release\solar_demo.exe --showcase-all
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
└── vendor/imgui/                       # Dear ImGui docking source distribution
```

---

## License

Solar Framework is licensed under the [MIT License](LICENSE).
You are free to use, modify, distribute, and integrate this software in commercial and non-commercial projects.
