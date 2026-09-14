# ☀️ Solar Framework

<div align="center">

```
  ____   ___  _        _    ____     _____ ____      _    __  __ _______        _____  ____  _  __
 / ___| / _ \| |      / \  |  _ \   |  ___|  _ \    / \  |  \/  | ____\ \      / / _ \|  _ \| |/ /
 \___ \| | | | |     / _ \ | |_) |  | |_  | |_) |  / _ \ | |\/| |  _|  \ \ /\ / / | | | |_) | ' / 
  ___) | |_| | |___ / ___ \|  _ <   |  _| |  _ <  / ___ \| |  | | |___  \ V  V /| |_| |  _ <| . \ 
 |____/ \___/|_____/_/   \_\_| \_\  |_|   |_| \_\/_/   \_\_|  |_|_____|  \_/\_/  \___/|_| \_\_|\_\
```

**High-Performance C++20 Dear ImGui & DirectX 11/12 Engine for Game Overlays, Developer Tooling, and Telemetry Systems**

[![Release](https://img.shields.io/badge/Release-v1.0.0--release-brightgreen?style=flat-square)](https://github.com/mp0b/Solar-Framework/releases)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen?style=flat-square&logo=github)](https://github.com/mp0b/Solar-Framework)
[![Standard](https://img.shields.io/badge/Standard-C%2B%2B20-blue?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/20)
[![Graphics](https://img.shields.io/badge/Backend-DirectX%2011%20%2F%2012-informational?style=flat-square&logo=windows)](https://learn.microsoft.com/en-us/windows/win32/direct3d11/)
[![GUI Core](https://img.shields.io/badge/Core-Dear%20ImGui%20Docking-purple?style=flat-square)](https://github.com/ocornut/imgui)
[![License](https://img.shields.io/badge/License-MIT-lightgrey?style=flat-square)](LICENSE)

</div>

---

## 📢 Public Release Notice (v1.0.0-release)

> [!IMPORTANT]
> **Solar Framework v1.0.0 is currently undergoing final release staging.**
> Packaged binary releases (MSVC pre-built static libraries, CMake install packages, and standalone showcase executables) will be published directly to [GitHub Releases](https://github.com/mp0b/Solar-Framework/releases) shortly.
>
> Active development, bleeding-edge feature additions, and source commits are maintained on the [`dev`](https://github.com/mp0b/Solar-Framework/tree/dev) branch.

---

## 📌 Index

1. [Architectural Overview](#-architectural-overview)
2. [Subsystem Breakdown](#-subsystem-breakdown)
3. [Building from Source](#-building-from-source)
4. [DirectX 11 Integration Guide](#-directx-11-integration-guide)
5. [Visuals & Overlay Pipeline](#-visuals--overlay-pipeline)
6. [Motion Dynamics Engine (`Solar::Anim`)](#-motion-dynamics-engine-solaranim)
7. [Component Widget Library](#-component-widget-library)
8. [Procedural Audio Subsystem](#-procedural-audio-subsystem)
9. [Branch Structure & Versioning](#-branch-structure--versioning)
10. [License & Acknowledgements](#-license--acknowledgements)

---

## 📖 Architectural Overview

**Solar Framework** is a modular C++20 engine designed for building production-grade game overlays, interactive developer tooling, diagnostic HUDs, and telemetry dashboards on Microsoft Windows. Built on top of **Dear ImGui (docking branch)** with hardware-accelerated **DirectX 11 and DirectX 12** backends, Solar is engineered for deterministic framerates, zero heap fragmentation, and strict rendering stability.

### Technical Highlights
- **Subpixel Font Oversampling & DPI Scaling**: High-DPI support with 3x oversampled FreeType font atlases to eliminate text rasterization artifacts.
- **Hardware-Accelerated 2D/3D Overlay Primitives**: 3D Oriented Bounding Boxes (OBB), gradient rectangles, glow contours, and isometric wave rings.
- **RK4 Numerical Physics Solver**: 4th-order Runge-Kutta integration for natural UI motion dynamics and critically damped camera smoothing (`SmoothDamp`).
- **Zero-Crash Drawing Architecture**: Automatic drawlist vertex/index buffer pre-allocation (`ReserveDrawlistBuffers`) and bounds-checked safe text formatting (`SafeText`).
- **Self-Contained Audio Synthesis**: Algorithmic in-memory waveform synthesis producing interface sound effects with zero external audio assets.

---

## 🧩 Subsystem Breakdown

| Subsystem | Namespace | Description |
| :--- | :--- | :--- |
| **Pipeline Bridge** | `Solar::Render::HookBridge` | Thread-safe D3D11/D3D12 render target, rasterizer, blend state, and viewport preservation. |
| **Overlay Graphics** | `Solar::Visuals` | Comprehensive primitives for 2D/3D bounding boxes, health/armor/ammo meters, skeletal linkages, and tactical indicators. |
| **3D Spatial Studio** | `Solar::Game::ESPPreview` | Real-time 360-degree orbital interactive viewport with multi-stance skeletal modeling. |
| **Motion Physics** | `Solar::Anim` | RK4 numerical spring solver, 2D vector springs, critically damped `SmoothDamp`, and timeline keyframe sequencer. |
| **Widget Library** | `Solar::Widgets` | Segmented switches, dual-thumb range sliders, numeric steppers, search inputs, tag multi-selectors, rotary dials, accordions, and KPI stat cards. |
| **Extended ImGui** | `Solar::Render::ImGuiExt` | Anti-crash vertex buffer reservation, memory protection, custom capsule pills, and conic gradients. |
| **Procedural Audio** | `Solar::Audio` | Algorithmic in-memory waveform generator (Square, Sine, Sawtooth) via `winmm.lib`. |
| **Theme & Styling** | `Solar::ThemeManager` | Luxury obsidian dark palettes, dynamic dual-stop RGB accent calibration, and background particle compute systems. |

---

## ⚡ Building from Source

### Prerequisites
- Windows 10 / 11 64-bit
- Visual Studio 2022 (MSVC v143 or newer) with C++20 toolchain
- CMake 3.16 or newer
- Windows 10 / 11 SDK

### Build Commands
```powershell
# Clone the repository
git clone https://github.com/mp0b/Solar-Framework.git
cd "Solar Framework"

# Generate build configuration (x64 Release)
cmake -B build -A x64

# Compile static library and demo showcase
cmake --build build --config Release
```

The build outputs:
- `build/Release/solar_lib.lib`: Reusable static library for integration into third-party tools.
- `build/Release/solar_demo.exe`: Standalone interactive showcase application.

---

## 🔌 DirectX 11 Integration Guide

Solar includes `StateGuard` and `HookBridge` to preserve native graphics pipeline states when embedding into external host applications:

```cpp
#include <solar/solar.hpp>
#include <d3d11.h>

HRESULT __stdcall OnPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    // 1. Guard native DirectX 11 states (OMRenderTargets, BlendState, Viewports)
    Solar::Render::StateGuard guard(pSwapChain);

    // 2. One-time initialization of Solar rendering context
    static bool initialized = false;
    if (!initialized) {
        Solar::Render::HookBridge::Get().Initialize(pSwapChain);
        Solar::Initialize();
        initialized = true;
    }

    // 3. Render Solar overlay pass
    Solar::Render::HookBridge::Get().BeginFrame();
    
    // Draw user interface and HUD overlays
    Solar::DemoApp::Get().Render();

    Solar::Render::HookBridge::Get().EndFrame();

    return OriginalPresent(pSwapChain, SyncInterval, Flags);
}
```

---

## 🎯 Visuals & Overlay Pipeline

All drawing primitives output directly to standard `ImDrawList*` surfaces:

```cpp
#include <solar/solar.hpp>

void RenderHUDOverlay(ImDrawList* drawList, const Vector2& screenMin, const Vector2& screenMax) {
    using namespace Solar;

    // Multi-Stop Gradient Bounding Box
    Visuals::DrawGradientBox2D(drawList, screenMin, screenMax, 
                               Color(1.0f, 0.45f, 0.1f, 1.0f), 
                               Color(1.0f, 0.15f, 0.1f, 1.0f));

    // Outer Contoured Glow
    Visuals::DrawGlowOutlineBox2D(drawList, screenMin, screenMax, 
                                  Color(1.0f, 0.45f, 0.1f, 1.0f), 
                                  Color(1.0f, 0.45f, 0.1f, 0.4f), 10.0f);

    // Dynamic Health and Status Bars
    Visuals::DrawHealthBar(drawList, screenMin, screenMax, 85.0f, 100.0f, BarPosition::Left, true, true);
    Visuals::DrawArmorBar(drawList, screenMin, screenMax, 60.0f, 100.0f, BarPosition::Left);

    // Floating Damage Feedback
    Visuals::DrawFloatingDamage(drawList, screenMax, 84.0f, Color(1.0f, 0.25f, 0.25f, 1.0f), 1.0f, true);
}
```

---

## 🌀 Motion Dynamics Engine (`Solar::Anim`)

```cpp
#include <solar/solar.hpp>

// 4th-Order Runge-Kutta Spring Solver
Solar::Anim::Spring spring(0.0f);
spring.stiffness = 180.0f;
spring.damping = 22.0f;
spring.mass = 1.0f;
spring.SetTarget(100.0f);
spring.Update(deltaTime);

// Critically Damped SmoothDamp
float current = 0.0f;
float velocity = 0.0f;
current = Solar::Anim::SmoothDamp(current, 100.0f, velocity, 0.25f, 1000.0f, deltaTime);
```

---

## 🎛️ Component Widget Library

The widget library provides 100+ production-tested UI controls:
- **`SegmentedControl`**: Animated pill selector with sliding highlight indicator.
- **`RangeSlider`**: Dual-thumb range slider for min/max value bounds.
- **`NumberStepper`**: Compact numeric stepper with decrement/increment buttons.
- **`SearchInput`**: Text filter field with icon and clear action button.
- **`DropdownMultiSelect`**: Multi-tag popover selector with active chip badges.
- **`KnobSlider`**: 270-degree rotary dial with radial indicator arc.
- **`BeginAccordion` / `EndAccordion`**: Collapsible container sections with animated rotation.
- **`StatCard`**: KPI metric card with value readout, delta badge, and trend sparklines.

---

## 🔊 Procedural Audio Subsystem

Solar generates user interface sound effects entirely in memory via mathematical waveform synthesis, requiring zero external audio assets on disk:

```cpp
#include <solar/solar.hpp>

Solar::Audio::PlayClick();
Solar::Audio::PlayToggle(true);
Solar::Audio::PlayNotification();
Solar::Audio::PlayError();
```

---

## 🌿 Branch Structure & Versioning

- **`release`** (Default / Stable): Sanitized public releases and tagged version distributions (`v1.0.0-release`).
- **`dev`** (Active Development): Bleeding-edge experimental features, continuous improvements, and internal development builds (`v0.0.2-dev`).

---

## 📄 License & Acknowledgements

Solar Framework is distributed under the [MIT License](LICENSE).

<div align="center">
  <sub>Developed by <b>SolarDev</b> &bull; Built with C++20, Dear ImGui & DirectX</sub>
</div>
