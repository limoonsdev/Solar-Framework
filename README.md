# ☀️ Solar Framework

<div align="center">

```
  ____   ___  _        _    ____     _____ ____      _    __  __ _______        _____  ____  _  __
 / ___| / _ \| |      / \  |  _ \   |  ___|  _ \    / \  |  \/  | ____\ \      / / _ \|  _ \| |/ /
 \___ \| | | | |     / _ \ | |_) |  | |_  | |_) |  / _ \ | |\/| |  _|  \ \ /\ / / | | | |_) | ' / 
  ___) | |_| | |___ / ___ \|  _ <   |  _| |  _ <  / ___ \| |  | | |___  \ V  V /| |_| |  _ <| . \ 
 |____/ \___/|_____/_/   \_\_| \_\  |_|   |_| \_\/_/   \_\_|  |_|_____|  \_/\_/  \___/|_| \_\_|\_\
```

**Production-grade C++20 Dear ImGui & DirectX 11/12 Engine for Game Security Tools, Cheats, and Loaders**

[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen?style=flat-square&logo=github)](https://github.com/mp0b/Solar-Framework)
[![Version](https://img.shields.io/badge/Version-v0.0.2--dev-orange?style=flat-square)](https://github.com/mp0b/Solar-Framework/tree/dev)
[![Standard](https://img.shields.io/badge/Standard-C%2B%2B20-blue?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/20)
[![Graphics](https://img.shields.io/badge/Backend-DirectX%2011%20%2F%2012-informational?style=flat-square&logo=windows)](https://learn.microsoft.com/en-us/windows/win32/direct3d11/)
[![GUI Core](https://img.shields.io/badge/Core-Dear%20ImGui%20Docking-purple?style=flat-square)](https://github.com/ocornut/imgui)
[![License](https://img.shields.io/badge/License-MIT-lightgrey?style=flat-square)](LICENSE)

</div>

---

## 📌 Index

1. [Overview](#-overview)
2. [Key Architecture Subsystems](#-key-architecture-subsystems)
3. [Quickstart & Build](#-quickstart--build)
4. [Internal DirectX 11 Hook Guide](#-internal-directx-11-hook-guide)
5. [Visuals 2.0 & ESP Engine](#-visuals-20--esp-engine)
6. [Studio 3D Mannequin Preview (`ESPPreview`)](#-studio-3d-mannequin-preview-esppreview)
7. [Motion Physics Engine (`Solar::Anim`)](#-motion-physics-engine-solaranim)
8. [UI Widget Suite (100+ Controls)](#-ui-widget-suite-100-controls)
9. [AOB Memory Pattern Scanner (`PatternScanner`)](#-aob-memory-pattern-scanner-patternscanner)
10. [Recoil Pattern & Ballistics Engines](#-recoil-pattern--ballistics-engines)
11. [Modded ImGui Engine (`ImGuiExt`)](#-modded-imgui-engine-imguiext)
12. [Procedural Audio Engine (Zero WAV Files)](#-procedural-audio-engine-zero-wav-files)
13. [HWID Spoofer & Trace Cleaner](#-hwid-spoofer--trace-cleaner)
14. [Repository Layout](#-repository-layout)

---

## 📖 Overview

**Solar Framework** is an engineered C++20 application framework built on top of **Dear ImGui (docking branch)** and native **DirectX 11 / 12**.

Unlike basic UI skins, Solar provides a complete native ecosystem for cheat developers, reverse engineers, and tool creators:
- **Zero-DWM Blur**: Physical 1:1 Per-Monitor DPI Aware v2 scaling with 3x subpixel font oversampling.
- **Visuals 2.0 Engine**: World-to-Screen primitives including 3D Oriented Bounding Boxes (OBB), gradient boxes, glow contours, capsule hitboxes, acoustic wave rings, dynamic spread crosshairs, and floating damage numbers.
- **Studio 3D ESP Preview**: Real-time 360-degree interactive orbital preview via mouse drag with a 5-stance mannequin state machine.
- **Motion Physics Solver (`Solar::Anim`)**: 4th-order Runge-Kutta (RK4) spring integration, multi-dimensional `Spring2D`, game-camera `SmoothDamp`, and keyframe `Timeline` sequencer.
- **Expanded UI Widgets**: Segmented controls, range sliders, number steppers, search filters, tag multi-selectors, 270° rotary knobs, collapsible accordions, and KPI stat cards with live sparklines.
- **Cheating & Reversing Subsystems**: Multi-byte IDA signature scanner (`PatternScanner`), weapon recoil visualizer with mouse compensation curves (`RecoilVisualizer`), and parabolic grenade trajectory predictor (`TrajectoryRenderer`).
- **Procedural Audio**: In-memory 16-bit 44.1 kHz PCM synthesizer (`winmm.lib`) producing click, toggle, and alert chimes without shipping a single `.wav` file on disk.

---

## 🧩 Key Architecture Subsystems

| Subsystem | Namespace | Description |
| :--- | :--- | :--- |
| **DirectX Hook Bridge** | `Solar::Render::HookBridge` | Thread-safe D3D11/D3D12 render target, rasterizer, blend state, and viewport preservation. |
| **Visuals 2.0 Engine** | `Solar::Visuals` | 3D OBBs, gradient boxes, glow outlines, capsule hitboxes, acoustic sound waves, spread crosshairs, floating damage. |
| **3D ESP Studio** | `Solar::Game::ESPPreview` | 360° orbital rotation mannequin with standing, crouching, scoped, jumping, and defusing stances. |
| **Motion Dynamics** | `Solar::Anim` | RK4 numerical spring solver, 2D vector springs, critically damped `SmoothDamp`, and keyframe timeline sequencer. |
| **Widget Suite** | `Solar::Widgets` | Segmented controls, dual-thumb range sliders, number steppers, search inputs, dropdown multi-selects, rotary knobs, accordions, and stat cards. |
| **Modded ImGui** | `Solar::Render::ImGuiExt` | Anti-crash vertex reservation, crash-proof text, custom pills, glowing capsules, conic gradient rects. |
| **AOB Pattern Scanner** | `Solar::Security::PatternScanner`| IDA pattern scanner (`"48 8B ? ? 74"`), relative displacement resolver, module bounds query. |
| **Recoil Engine** | `Solar::Game::RecoilVisualizer` | Ballistic spray pattern tracker for AK-47, M4A4, Vandal, Phantom with mouse compensation curves. |
| **Trajectory Simulator**| `Solar::Game::TrajectoryRenderer`| Parabolic grenade arc simulator with surface bounce collision points and blast radiuses. |
| **Procedural Audio** | `Solar::Audio` | Algorithmic in-memory waveform generator (Square, Sine, Sawtooth) via `winmm`. |
| **Security Suite** | `Solar::Security` | HWID Spoofer (SMBIOS, MAC, Disk Serial, GPU GUID) and Anti-Cheat log cleaner. |
| **Theme Engine** | `Solar::ThemeManager` | 8 PastOwl-inspired obsidian presets, live dual-color RGB accent tuner, particle backgrounds. |

---

## ⚡ Quickstart & Build

### Prerequisites
- Windows 10 / 11 64-bit
- Visual Studio 2022 (MSVC v143 / v144) with C++20 toolset
- CMake 3.16 or newer
- Windows SDK (DirectX 11 headers included by default)

### Compilation
```powershell
# Clone repository
git clone https://github.com/mp0b/Solar-Framework.git
cd "Solar Framework"

# Generate build files (x64 Release)
cmake -B build -A x64

# Compile static library and standalone demonstration binary
cmake --build build --config Release
```

The build produces:
- `build/Release/solar_lib.lib`: Reusable static library.
- `build/Release/solar_demo.exe`: Standalone showcase application.

---

## 🔌 Internal DirectX 11 Hook Guide

Solar provides `StateGuard` and `HookBridge` to safely hijack a game's DirectX 11 swap chain without breaking the game's internal render states.

```cpp
#include <solar/solar.hpp>
#include <d3d11.h>

// Called inside your hooked IDXGISwapChain::Present
HRESULT __stdcall HookedPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    // 1. Automatically saves and restores all D3D11 state (OMRenderTargets, BlendState, Viewports)
    Solar::Render::StateGuard guard(pSwapChain);

    // 2. Initialize ImGui and Solar on first call
    static bool initialized = false;
    if (!initialized) {
        Solar::Render::HookBridge::Get().Initialize(pSwapChain);
        Solar::Initialize();
        initialized = true;
    }

    // 3. Render Solar Menu and Game Visuals
    Solar::Render::HookBridge::Get().BeginFrame();
    
    // Draw in-game ESP and menu widgets here
    Solar::DemoApp::Get().Render();

    Solar::Render::HookBridge::Get().EndFrame();

    return oPresent(pSwapChain, SyncInterval, Flags);
}
```

---

## 🎯 Visuals 2.0 & ESP Engine

Solar contains an extensive visuals rendering suite located in `Solar::Visuals` (`solar/game/visuals_renderer.hpp`). All methods draw directly to any `ImDrawList*`:

```cpp
#include <solar/solar.hpp>

void DrawGameVisuals(ImDrawList* drawList, const Vector2& screenMin, const Vector2& screenMax) {
    using namespace Solar;

    // 1. Multi-Stop Gradient Bounding Box
    Visuals::DrawGradientBox2D(drawList, screenMin, screenMax, Color(1.0f, 0.45f, 0.1f, 1.0f), Color(1.0f, 0.15f, 0.1f, 1.0f));

    // 2. Multi-Pass Outer Contoured Glow
    Visuals::DrawGlowOutlineBox2D(drawList, screenMin, screenMax, Color(1.0f, 0.45f, 0.1f, 1.0f), Color(1.0f, 0.45f, 0.1f, 0.4f), 10.0f);

    // 3. 3D Oriented Bounding Box (OBB)
    Matrix3x3 rot = Matrix3x3::FromEuler(0.0f, 45.0f, 0.0f);
    Game::OBB3D obb(Vector3(0, 0, 0), Vector3(32, 72, 32), rot);
    Game::OBBRenderer::DrawOBB(drawList, obb, viewProjectionMatrix, Color(1.0f, 0.8f, 0.2f, 0.9f), true, 0.15f);

    // 4. Acoustic Ground Wave Radar Ring
    Visuals::DrawAcousticWave(drawList, screenCenter, 35.0f, 18.0f, 0.0f, Color(1.0f, 0.45f, 0.1f, 0.7f));

    // 5. Dynamic Spread Crosshair
    Visuals::DrawSpreadCrosshair(drawList, screenCenter, 4.0f, 24.0f, 8.0f, Color(1.0f, 0.75f, 0.15f, 1.0f), true);

    // 6. Physics Floating Damage Numbers
    Visuals::DrawFloatingDamage(drawList, damagePos, 84.0f, Color(1.0f, 0.25f, 0.25f, 1.0f), 0.9f, true);
}
```

---

## 🧍 Studio 3D Mannequin Preview (`ESPPreview`)

The `ESPPreview` widget supports real-time 360-degree orbital rotation via left-click mouse dragging and a 5-stance skeletal state machine:
- **Standing**: Standard combat pose with upright spine and balanced stance.
- **Crouching**: Compressed torso, bent knees, and lower center of mass.
- **Scoped**: Tilted upper torso with rifle raised to eye level.
- **Jumping**: Tucked legs and elevated arms for airborne entities.
- **Defusing**: Kneeling angle facing downward with interaction hands.

```cpp
#include <solar/solar.hpp>

// Render preview in your ESP settings tab
Solar::Game::ESPSettings settings;
settings.enable3DBox = true;
settings.enableGlowOutline = true;
settings.enableBarrelRay = true;
settings.stance = 1; // Crouching

Solar::Game::ESPPreview::Render("##StudioMannequin", ImVec2(340.0f, 320.0f), settings);
```

---

## 🌀 Motion Physics Engine (`Solar::Anim`)

Solar replaces basic linear easing with numerical physics solvers:
- **RK4 Numerical Spring Solver**: 4-substep Runge-Kutta numerical integration for mass-spring-damper equations without overshoot instability.
- **`Spring2D`**: Dual independent RK4 solver for smooth 2D canvas translation, reticle lead, and cursor tracking.
- **`SmoothDamp`**: Critically damped spring solver ensuring smooth game-camera tracking without oscillation.
- **`Timeline`**: Multi-keyframe animation sequencer supporting custom easing curves (`Linear`, `InQuad`, `OutQuad`, `InOutQuad`, `InCubic`, `OutCubic`, `InOutCubic`, `OutBack`, `OutExpo`).

```cpp
#include <solar/solar.hpp>

// RK4 Spring Dynamics
Solar::Anim::Spring spring(0.0f);
spring.stiffness = 180.0f;
spring.damping = 22.0f;
spring.mass = 1.0f;
spring.SetTarget(100.0f);
spring.Update(deltaTime);

// Critically Damped SmoothDamp
float currentPos = 0.0f;
float velocity = 0.0f;
currentPos = Solar::Anim::SmoothDamp(currentPos, 100.0f, velocity, 0.25f, 1000.0f, deltaTime);
```

---

## 🎛️ UI Widget Suite (100+ Controls)

Solar provides a large selection of modern UI components styled with PastOwl aesthetics:
- **`SegmentedControl`**: Animated pill selector with smooth sliding highlights.
- **`RangeSlider`**: Dual-thumb range slider for min/max value selection.
- **`NumberStepper`**: Compact numeric stepper with `[-]` and `[+]` action buttons.
- **`SearchInput`**: Search field with integrated magnifying glass and `[X]` clear button.
- **`DropdownMultiSelect`**: Multi-tag popover dropdown with active chip badges.
- **`KnobSlider`**: 270-degree rotary dial with radial progress arc and drag tuning.
- **`BeginAccordion` / `EndAccordion`**: Collapsible container sections with animated chevron rotation.
- **`StatCard`**: KPI metric card with title, numeric readout, delta percentage badge, and mini sparkline trend.

---

## 🔍 AOB Memory Pattern Scanner (`PatternScanner`)

High-speed byte pattern scanning supporting standard IDA signatures (`?` and `??` wildcards) and relative RIP displacement calculation.

```cpp
#include <solar/solar.hpp>

void ScanGameOffsets() {
    using namespace Solar::Security;

    // 1. Retrieve module boundaries without opening external handles
    uintptr_t base = 0;
    size_t size = 0;
    if (PatternScanner::GetModuleBounds("client.dll", base, size)) {
        printf("client.dll mapped at 0x%llX (Size: %zu bytes)\n", base, size);
    }

    // 2. Scan IDA-style pattern across module
    uintptr_t match = PatternScanner::FindPattern("client.dll", "48 8B 05 ? ? ? ? 48 85 C0 74 12");
    if (match) {
        // 3. Resolve 32-bit RIP-relative displacement: mov rax, [rip + offset]
        uintptr_t resolvedAddress = PatternScanner::ResolveRelative(match, 3, 7);
        printf("Pattern found at: 0x%llX -> Target: 0x%llX\n", match, resolvedAddress);
    }
}
```

---

## 🔫 Recoil Pattern & Ballistics Engines

### 1. Recoil Spray Visualizer (`RecoilVisualizer`)
Renders weapon recoil patterns with shot indices and mouse compensation paths for AK-47, M4A4, Vandal, and Phantom.

```cpp
#include <solar/solar.hpp>

Solar::Game::RecoilVisualizer::Render(
    "##RecoilView",
    ImVec2(320.0f, 320.0f),
    Solar::Game::RecoilWeapon::AK47,
    currentBulletIndex,
    true /* showCompensationPath */
);
```

### 2. Ballistic Trajectory Predictor (`TrajectoryRenderer`)
Calculates projectile arcs taking gravity, velocity scale, throw angle, and collision bounce damping into account.

```cpp
#include <solar/solar.hpp>

Solar::Game::TrajectoryRenderer::Render(
    "##TrajSimCanvas",
    ImVec2(320.0f, 320.0f),
    Solar::Game::ProjectileType::HighExplosive,
    45.0f /* pitchAngle */,
    1.0f  /* velocityScale */
);
```

---

## 🛡️ Modded ImGui Engine (`ImGuiExt`)

Custom render primitives and stability enhancements that extend Dear ImGui's native pipeline:
- `ImGuiExt::ReserveDrawlistBuffers(drawList, vtxCount, idxCount)`: Pre-allocates buffer memory, eliminating mid-frame reallocation spikes.
- `ImGuiExt::SafeText(drawList, pos, color, text)`: Strict bounds-checked string drawer immune to format exploits.
- `ImGuiExt::AddPill`: High-definition anti-aliased capsule primitive.
- `ImGuiExt::AddGlowCapsule`: Multi-pass blurred volumetric glow capsule.
- `ImGuiExt::AddConicGradientRect`: Dynamic rotating angular gradient shader rectangle.

---

## 🔊 Procedural Audio Engine (Zero WAV Files)

Generates audio purely through mathematical synthesis in system RAM:
- **Harmonics**: Sine, triangle, square, and sawtooth waveform algorithms.
- **Audio Primitives**: Mechanical click, toggle switch, success harmonic chime, alert notification, and error frequencies.
- **Zero Assets**: Operates without external audio files on disk.

```cpp
#include <solar/solar.hpp>

Solar::Audio::PlayClick();
Solar::Audio::PlayToggle(true);
Solar::Audio::PlayNotification();
Solar::Audio::PlayError();
```

---

## 🔒 HWID Spoofer & Trace Cleaner

- **Hardware ID Randomizer**: Dynamic generation of SMBIOS UUIDs, Motherboard Serial Numbers, Network MAC addresses, Physical Drive serials, and GPU Device GUIDs.
- **Trace Cleaning Engine**: Eradication of DirectX shader caches, crash dumps, telemetry ETW logs, USN journals, and prefetch signatures.

---

## 📂 Repository Layout

```
Solar Framework/
├── CMakeLists.txt              # Unified build definition (solar_lib & solar_demo)
├── README.md                   # Technical documentation
├── include/solar/
│   ├── solar.hpp               # Master umbrella include
│   ├── core/                   # Math, Color, Timer, Logger, Crypto, Types
│   ├── render/                 # D3D11/D3D12 renderers, HookBridge, StateGuard, ImGuiExt
│   ├── anim/                   # RK4 Spring, Spring2D, SmoothDamp, Timeline, Easing
│   ├── audio/                  # In-memory WaveSynth, SoundEffects, AudioEngine
│   ├── theme/                  # Palette, Style, Presets, ThemeManager
│   ├── ui/                     # Window, Titlebar, Sidebar, Card, Modal, Splash, Welcome
│   ├── widgets/                # Segmented, RangeSlider, Stepper, Search, Dropdown, Knob, Accordion, StatCard
│   ├── game/                   # VisualsRenderer, ESPPreview, OBBRenderer, Radar, Recoil, Trajectory
│   ├── security/               # PatternScanner, HWIDSpoofer, TraceCleaner, MemoryTools
│   ├── auth/                   # LicenseScreen, HWIDGenerator, AuthManager
│   └── notifications/          # Toast, NotificationSystem
├── src/                        # Implementation files for all subsystems
├── demo/                       # Standalone showcase application (main.cpp, demo_app.cpp)
└── vendor/imgui/               # Dear ImGui docking source distribution
```

---

<div align="center">
  <sub>Developed with precision by <b>SolarDev</b> &bull; Licensed under the MIT License</sub>
</div>
