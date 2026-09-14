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
[![Version](https://img.shields.io/badge/Version-v1.0.1%20dev-orange?style=flat-square)](https://github.com/mp0b/Solar-Framework/releases)
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
4. [Internal DirectX 11 Hook Guide (15 Lines)](#-internal-directx-11-hook-guide-15-lines)
5. [In-Game ESP & Visuals Engine](#-in-game-esp--visuals-engine)
6. [AOB Memory Pattern Scanner (`PatternScanner`)](#-aob-memory-pattern-scanner-patternscanner)
7. [Recoil Pattern & Ballistics Engines](#-recoil-pattern--ballistics-engines)
8. [UI Widget Suite & Card Pagination](#-ui-widget-suite--card-pagination)
9. [Modded ImGui Engine (`ImGuiExt`)](#-modded-imgui-engine-imguiext)
10. [Procedural Audio Engine (Zero WAV Files)](#-procedural-audio-engine-zero-wav-files)
11. [HWID Spoofer & Trace Cleaner](#-hwid-spoofer--trace-cleaner)
12. [Repository Layout & Documentation](#-repository-layout--documentation)

---

## 📖 Overview

**Solar Framework** is an engineered C++20 application framework built on top of **Dear ImGui (docking branch)** and native **DirectX 11 / 12**.

Unlike typical ImGui themes that merely recolor widgets, Solar provides an entire native ecosystem for cheat developers, reverse engineers, and tool creators:
- **Zero-DWM Blur**: Physical 1:1 Per-Monitor DPI Aware v2 scaling with 3x subpixel font oversampling.
- **Zero-Crash Drawing Engine**: Pre-allocated vertex/index buffer management (`ImGuiExt::ReserveDrawlistBuffers`) and format-string-safe text wrappers (`SafeText`).
- **Complete In-Game Visuals API**: World-to-Screen ready drawing functions for 2D corner/full boxes, health/armor/ammo bars, bone skeletons, offscreen radar arrows, and animated hitmarkers.
- **Cheating & Reversing Subsystems**: Multi-byte IDA signature scanner (`PatternScanner`), weapon recoil visualizer with mouse compensation curves (`RecoilVisualizer`), and parabolic grenade trajectory predictor (`TrajectoryRenderer`).
- **Procedural Audio**: In-memory 16-bit 44.1 kHz PCM synthesizer (`winmm.lib`) producing click, toggle, and alert chimes without shipping a single `.wav` file on disk.

---

## 🧩 Key Architecture Subsystems

| Subsystem | Namespace | Description |
| :--- | :--- | :--- |
| **DirectX Hook Bridge** | `Solar::Render::HookBridge` | Thread-safe D3D11/D3D12 render target, rasterizer, blend state, and viewport preservation. |
| **In-Game Visuals** | `Solar::Visuals` | Comprehensive ESP primitives: 2D boxes, health bars, skeleton bones, snaplines, hitmarkers. |
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

## 🔌 Internal DirectX 11 Hook Guide (15 Lines)

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

## 🎯 In-Game ESP & Visuals Engine

Solar contains an extensive visuals rendering suite located in `Solar::Visuals` (`solar/game/visuals_renderer.hpp`). All methods draw directly to any `ImDrawList*` (foreground overlay or background).

```cpp
#include <solar/solar.hpp>

void DrawPlayerESP(ImDrawList* drawList, const Vector2& screenMin, const Vector2& screenMax) {
    using namespace Solar;

    // 1. 2D Bounding Box (Corner Style with thin black outline)
    Visuals::DrawBoundingBox2D(drawList, screenMin, screenMax, BoxStyle::Corner, Color(1.0f, 0.35f, 0.35f, 1.0f));

    // 2. Head Marker
    Vector2 headPos(screenMin.x + (screenMax.x - screenMin.x) * 0.5f, screenMin.y + 8.0f);
    Visuals::DrawHeadCircle(drawList, headPos, 6.5f, Color(1.0f, 1.0f, 1.0f, 0.9f));

    // 3. Dynamic Health Bar (Green -> Yellow -> Red gradient based on HP)
    Visuals::DrawHealthBar(drawList, screenMin, screenMax, 72.0f, 100.0f, BarPosition::Left, true, true);

    // 4. Armor & Ammo Bars
    Visuals::DrawArmorBar(drawList, screenMin, screenMax, 50.0f, 100.0f, BarPosition::Left);
    Visuals::DrawAmmoBar(drawList, screenMin, screenMax, 24, 30, BarPosition::Bottom);

    // 5. Name, Weapon & Badge Flags
    Visuals::DrawNameTag(drawList, Vector2(headPos.x, screenMin.y - 4.0f), "Target_Enemy");
    Visuals::DrawWeaponTag(drawList, Vector2(headPos.x, screenMax.y + 6.0f), "AK-47", 24, 30);

    std::vector<std::pair<std::string, Color>> flags = {
        { "SCOPED", Color(0.2f, 0.7f, 1.0f, 1.0f) },
        { "ARMOR",  Color(0.4f, 0.9f, 0.4f, 1.0f) }
    };
    Visuals::DrawFlagTags(drawList, screenMax, flags);

    // 6. Snapline to target from screen bottom
    Visuals::DrawSnapline(drawList, Vector2(headPos.x, screenMax.y), SnaplineOrigin::BottomCenter, Color(1.0f, 0.8f, 0.2f, 0.8f), 1.2f, true);

    // 7. Offscreen Indicator Arrow (when target is outside camera frustum)
    Vector2 screenCenter(960.0f, 540.0f);
    Visuals::DrawOffscreenIndicator(drawList, screenCenter, 1.25f /* radians */, 120.0f /* radius */, Color(1.0f, 0.2f, 0.2f, 1.0f), 45.0f /* distance meters */);
}
```

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

// Render spray pattern inside your menu card
Solar::Game::RecoilVisualizer::Render(
    "##RecoilView",
    ImVec2(320.0f, 320.0f),
    Solar::Game::RecoilWeapon::AK47,
    -1,    // activeBullet: -1 for full spray, 0..30 to highlight current shot
    true,  // showCompensation: draws inverted mouse movement compensation line
    Solar::Color(1.0f, 0.72f, 0.15f, 1.0f)
);
```

### 2. Ballistic Trajectory Predictor (`TrajectoryRenderer`)
Simulates parabolic arcs, gravity factors, drag, bounce collisions, and blast radius circles for throwables (HE Grenades, Smokes, Flashbangs, Molotovs).

```cpp
#include <solar/solar.hpp>

Solar::Game::TrajectoryRenderer::Render(
    "##GrenadeArc",
    ImVec2(320.0f, 320.0f),
    Solar::Game::ProjectileType::HighExplosive,
    48.0f, // throw angle in degrees
    1.0f   // throw power multiplier (0.5x..2.0x)
);
```

---

## 📦 UI Widget Suite & Card Pagination

### Card Pagination Helper (`[< Prev] Page X/Y [Next >]`)
Organize large configurations within fixed cards using page navigation:

```cpp
#include <solar/solar.hpp>

static int currentPage = 0;
const int totalPages = 2;

if (Solar::Widgets::BeginCard("##AimbotCard", "Target Acquisition", Solar::IconType::Crosshair, ImVec2(350, 480))) {
    if (currentPage == 0) {
        static bool aimbot = true;
        static float fov = 12.0f;
        Solar::Widgets::Toggle("Enable Aimbot", &aimbot);
        Solar::Widgets::SliderFloat("FOV", &fov, 1.0f, 180.0f, "%.1f", "deg");
    } else if (currentPage == 1) {
        static bool triggerbot = false;
        static int delay = 40;
        Solar::Widgets::Toggle("Triggerbot", &triggerbot);
        Solar::Widgets::SliderInt("Reaction Delay", &delay, 0, 200, "%d ms");
    }

    // Pagination Footer Controls
    ImGui::Spacing();
    ImGui::Separator();
    
    if (currentPage == 0) ImGui::BeginDisabled();
    if (Solar::Widgets::Button("< Prev", ImVec2(75, 26), Solar::ButtonStyle::Secondary)) currentPage--;
    if (currentPage == 0) ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::Text("Page %d / %d", currentPage + 1, totalPages);
    ImGui::SameLine();

    if (currentPage == totalPages - 1) ImGui::BeginDisabled();
    if (Solar::Widgets::Button("Next >", ImVec2(75, 26), Solar::ButtonStyle::Secondary)) currentPage++;
    if (currentPage == totalPages - 1) ImGui::EndDisabled();

    Solar::Widgets::EndCard();
}
```

### Complete Widget Catalog
- `Widgets::Toggle(label, &boolVal, tooltip)`
- `Widgets::SliderFloat(label, &floatVal, min, max, format, unit)`
- `Widgets::SliderInt(label, &intVal, min, max, format)`
- `Widgets::Combo(label, &selectedIdx, items, count)`
- `Widgets::MultiCombo(label, flagsVector, items, count)`
- `Widgets::Keybind(label, &vKey, &keyMode)`
- `Widgets::ColorPicker(label, floatCol[4])`
- `Widgets::HitboxPicker(hitboxBools, size)`
- `Widgets::RadarPreview(id, size)`
- `Widgets::Button(label, size, style)`

---

## 🛡️ Modded ImGui Engine (`ImGuiExt`)

To eliminate runtime heap reallocations and crashes during heavy ESP workloads, Solar provides `ImGuiExt`:

```cpp
#include <solar/solar.hpp>

ImDrawList* draw = ImGui::GetWindowDrawList();

// 1. Pre-allocate buffer capacity ahead of the frame (zero mid-frame reallocations)
Solar::Render::ImGuiExt::ReserveDrawlistBuffers(draw, 4096, 6144);

// 2. Crash-proof string rendering (prevents format-string crashes & null dereferencing)
Solar::Render::ImGuiExt::SafeText(draw, ImVec2(100, 100), 0xFFFFFFFF, "Player Name: %s");

// 3. Custom geometric primitives
Solar::Render::ImGuiExt::AddPill(draw, ImVec2(20, 20), ImVec2(120, 50), 0x88FF9900, 0xFFFF9900, 1.5f);
Solar::Render::ImGuiExt::AddGlowCapsule(draw, ImVec2(30, 80), ImVec2(180, 80), 12.0f, Solar::Color(1, 0.5f, 0, 1), 10.0f);
Solar::Render::ImGuiExt::AddConicGradientRect(draw, ImVec2(20, 120), ImVec2(150, 220), 8.0f, Solar::Color(1, 0, 0.5f, 1), Solar::Color(0, 0.5f, 1, 1), 0.0f);
```

---

## 🔊 Procedural Audio Engine (Zero WAV Files)

Solar synthesizes sound waves in memory using `winmm.lib`. No external files are required.

```cpp
#include <solar/solar.hpp>

// Trigger audio events
Solar::Audio::PlayClick();        // High-frequency tactile mechanical click
Solar::Audio::PlayToggle(true);   // Ascending pitch tone for toggling ON
Solar::Audio::PlayToggle(false);  // Descending pitch tone for toggling OFF
Solar::Audio::PlayNotification(); // Harmonic notification chime
Solar::Audio::PlayError();        // Low warning buzz tone
```

---

## 🔒 HWID Spoofer & Trace Cleaner

Integrated modules for device identifier masking and anti-cheat telemetry scrubbing:
- **SMBIOS**: Motherboard UUID, Serial Number, BIOS Release Date.
- **Disk**: Physical storage drive serials (`IOCTL_STORAGE_QUERY_PROPERTY`).
- **Network**: MAC addresses across all physical NIC adapters.
- **Display**: EDID display serial hashes and GPU device instance paths.
- **Log Cleaning**: BattlEye, EasyAntiCheat, Vanguard, FiveM, Discord cache, and Windows USN journal sanitization.

---

## 📁 Repository Layout & Documentation

```
Solar Framework/
├── CMakeLists.txt              # Standard build configuration
├── README.md                   # This developer documentation
├── docs/                       # Detailed manuals
│   ├── GETTING_STARTED.md      # Detailed compilation & setup guide
│   ├── COMPONENTS.md           # 14-subsystem API reference manual
│   ├── THEMING.md              # Palette customization & style tokens
│   └── INTEGRATION_GUIDE.md    # D3D11 / D3D12 DLL injection guide
├── include/solar/              # 83 C++20 Header files
│   ├── solar.hpp               # Master umbrella header
│   ├── core/                   # Math, Vector2, Color, Timer, Logger
│   ├── render/                 # HookBridge, StateGuard, ImGuiExt, FontManager
│   ├── game/                   # VisualsRenderer, RecoilVisualizer, TrajectoryRenderer, HitboxPicker, Radar
│   ├── security/               # PatternScanner, HWID Spoofer, Trace Cleaner
│   ├── audio/                  # WaveSynth, AudioEngine
│   ├── ui/                     # Window, Titlebar, Sidebar, Card, SubTabs
│   └── widgets/                # Toggle, Slider, Combo, Keybind, ColorPicker, Button
├── src/                        # 72 C++20 Implementation files
├── demo/                       # Standalone DirectX 11 Showcase Application
└── vendor/                     # Dear ImGui v1.91 (docking) & vector font assets
```

---

## ⚖️ License

Distributed under the **MIT License**. Maintained by **SolarDev** for the game security, reverse engineering, and developer tooling communities.
