# ☀️ Solar Framework

<div align="center">

```
  ____   ___  _        _    ____     _____ ____      _    __  __ _______        _____  ____  _  __
 / ___| / _ \| |      / \  |  _ \   |  ___|  _ \    / \  |  \/  | ____\ \      / / _ \|  _ \| |/ /
 \___ \| | | | |     / _ \ | |_) |  | |_  | |_) |  / _ \ | |\/| |  _|  \ \ /\ / / | | | |_) | ' / 
  ___) | |_| | |___ / ___ \|  _ <   |  _| |  _ <  / ___ \| |  | | |___  \ V  V /| |_| |  _ <| . \ 
 |____/ \___/|_____/_/   \_\_| \_\  |_|   |_| \_\/_/   \_\_|  |_|_____|  \_/\_/  \___/|_| \_\_|\_\
```

### *The High-End C++20 / DirectX Dear ImGui Suite for Tools, Loaders & Game Security Engineering*

[![Status](https://img.shields.io/badge/Status-v1.0.1%20Under%20Development-ff7a00?style=for-the-badge&logo=git)](https://github.com/mp0b/Solar-Framework)
[![Architecture](https://img.shields.io/badge/Architecture-14%20Decoupled%20Subsystems-9333ea?style=for-the-badge)](https://github.com/mp0b/Solar-Framework)
[![Standard](https://img.shields.io/badge/C%2B%2B-20%20Standard-00599c?style=for-the-badge&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/20)
[![Graphics](https://img.shields.io/badge/DirectX-11%20%26%2012-0078d7?style=for-the-badge&logo=windows)](https://learn.microsoft.com/en-us/windows/win32/direct3d11/atcon-effects)
[![DPI](https://img.shields.io/badge/Rendering-Native%201%3A1%20DPI%20Per--Monitor%20v2-10b981?style=for-the-badge)](https://github.com/mp0b/Solar-Framework)
[![License](https://img.shields.io/badge/License-MIT-gray?style=for-the-badge)](LICENSE)

</div>

---

## 📖 Overview

**Solar Framework** is an industrial-grade, ultra-luxurious Dear ImGui enhancement engine written in modern **C++20** for **DirectX 11** and **DirectX 12**. 

Designed from the ground up for developer tools, loaders, HWID spoofers, and game reverse engineering suites, Solar replaces traditional flat, blurry ImGui boxes with deep obsidian glassmorphism, multi-pass volumetric drop shadows, responsive spring dynamics, and an in-memory procedural audio synthesizer.

> [!NOTE]
> **Version 1.0.1** is currently **Under Development**.  
> The codebase contains **147 modular files** across **14 decoupled subsystems**. Everything is self-contained: zero external audio assets, zero messy third-party font managers, and zero DWM scaling blur.

---

## ✨ Core Highlights

### 💎 Crystal-Clear 1:1 Physical DPI (Zero Blur)
- **Windows 11 Per-Monitor DPI Awareness v2** enabled via `SetProcessDpiAwarenessContext`.
- No DWM bilinear stretch: fonts and geometries scale dynamically with `GetDpiForWindow(hwnd) / 96.0f`.
- **3x Subpixel Oversampling** (`OversampleH = 3, OversampleV = 3`) with **1.18x rasterizer contrast** for razor-sharp typography.
- Native hardware anti-aliased lines, textures, and fills.

### 🔊 Procedural In-Memory Audio Engine (`Solar::Audio`)
- Real-time **16-bit 44.1 kHz PCM wave synthesis** generated directly in memory via `winmm.lib`.
- **Zero `.wav` files on disk**: click, toggle, chime, alert, and error sounds are synthesized algorithmically.

### 🎨 Obsidian Luxury Aesthetics (Inspired by PastOwl & LiquidDX11)
- Rich multi-layered dark palettes (`#0B0D13`, `#0F121C`, `#141724`).
- **8 Master presets**: Solar Flare (Amber Gold), Cyber Neon (Cyan), Void Amethyst (Purple), Emerald Matrix (Mint Green), Blood Ruby (Crimson), Arctic White (Ice Blue), Tokyo Night (Indigo), and Midnight Carbon (Slate).
- Custom RGB accent interpolation and runtime synchronization with ImGui.

### ❄️ Dynamic Background Particle Systems
- Selectable modes: **Falling Cyber Snow** (with organic wind sway), **Rising Amber Embers** (with turbulence), and **Star Constellations** (interconnected geometric network nodes with mouse repulsion).

### 🚀 Complete Suite for Cheats, Loaders & Security Tools
- **Live 2D Mannequin ESP Preview**: Interactive bone skeleton, bounding box styles (Corner, Full, Filled), health gradient bar, armor bar, distance tag, and weapon readout.
- **2D Anatomical Hitbox Matrix**: Clickable wireframe body zone selector with instant haptic audio feedback.
- **2D Tactical Mini-Radar**: Top-down radar minimap with concentric range circles, FOV cone, enemy/friendly blips, and altitude arrows.
- **Hardware ID Spoofer & Trace Cleaner**: Randomizes SMBIOS, MAC, Disk Serial, GPU GUID, and cleans traces for Vanguard, EAC, BattlEye, FiveM, Discord, and Steam journals.
- **Loader Authentication Screen**: Product key input, deterministic hardware fingerprint display, 1-click clipboard copy, and session entitlement tags.
- **Floating HUD Overlays**: Standalone draggable windows for active Keybinds, Spectator observers, and Status Watermark.
- **Telemetry Profiler**: Real-time FPS stream graph, frame delta time, draw calls, and vertex counters.
- **Animated Luxury Splash Screen**: Rotating solar corona spinner with progress breakdown and smooth alpha transition.

---

## 🏛️ Architecture Breakdown

Solar Framework is organized into **14 decoupled subsystems**:

```
Solar Framework/
├── CMakeLists.txt              # Production CMake configuration (C++20, DX11/DX12)
├── README.md                   # Documentation overview
├── docs/                       # Technical integration guides
│   ├── GETTING_STARTED.md      # Setup & compilation manual
│   ├── COMPONENTS.md           # Exhaustive 14-subsystem API reference
│   ├── THEMING.md              # Palette customization & style tokens
│   └── INTEGRATION_GUIDE.md    # Internal DLL injection & overlay guide
├── include/solar/              # 79 Public C++20 Headers
│   ├── solar.hpp               # Umbrella header
│   ├── core/                   # [1] Math, Color, Timer, StringUtils, Crypto, Logger
│   ├── render/                 # [2] DX11/12 Abstractions, DrawUtils, ShadowCaster, GlowEngine, FontManager
│   ├── audio/                  # [3] WaveSynth, SoundEffects, AudioEngine
│   ├── anim/                   # [4] Spring Dynamics, Easing curves, Transitions, AnimationManager
│   ├── theme/                  # [5] Obsidian Palette, Style metrics, Presets, ThemeManager
│   ├── ui/                     # [6] Window, Titlebar, Sidebar, Subtabs, Card, Groupbox, Modal, Tooltip, SplashScreen
│   ├── widgets/                # [7] Toggle, Slider, Combo, MultiCombo, Keybind, ColorPicker, Button, Checkbox
│   ├── game/                   # [8] ESP Preview, Hitbox Picker, Radar, Spectator List, Keybind List, Watermark
│   ├── security/               # [9] HWID Spoofer, Trace Cleaner, Memory Tools
│   ├── auth/                   # [10] License Screen, HWID Generator, Auth Manager
│   ├── config/                 # [11] JSON Serializer, Preset Storage, Config Manager
│   ├── notifications/          # [12] Toast Stack, Countdown Bars, Sound Triggers
│   ├── fx/                     # [13] Particle System (Snow, Embers, Constellations), Ambient Glow
│   └── tools/                  # [14] Real-Time Telemetry Profiler, Inspector
├── src/                        # 68 Implementation Sources
│   ├── solar.cpp
│   ├── icons.cpp
│   └── [subsystems]/           # Matching C++ implementations
├── demo/                       # Standalone DirectX 11 Showcase Application
│   ├── main.cpp                # Native Win32 + DX11 message loop with Per-Monitor DPI v2
│   ├── demo_app.hpp            # Multi-tab showcase dashboard
│   └── demo_app.cpp
└── vendor/
    ├── imgui/                  # Dear ImGui v1.91 (docking branch)
    └── fa-solid-900.ttf        # FontAwesome 6 Solid vectors
```

---

## ⚡ Quickstart

### Prerequisites
- **Windows 10 / 11 64-bit**
- **Visual Studio 2022 or newer** (with MSVC C++20 support)
- **CMake 3.16+**
- DirectX 11 SDK (included in Windows SDK)

### Building the Project
```powershell
# 1. Generate build tree
cmake -B build -A x64

# 2. Compile static library and standalone demo
cmake --build build --config Release
```

### Running the Standalone Demo
```powershell
./build/Release/solar_demo.exe
```

Artifacts:
- `build/Release/solar_lib.lib` (~7.5 MB static library)
- `build/Release/solar_demo.exe` (~700 KB standalone executable)

---

## 💻 Code Example

```cpp
#include <solar/solar.hpp>

// 1. Initialize Solar (Themes, Particle FX, Procedural Audio Bank)
Solar::Initialize();

// 2. Main Render Loop
Solar::NewFrame();

// 3. Status HUD Watermark
Solar::WatermarkInfo wm;
wm.frameworkName = "SOLAR";
wm.version = "v1.0.1";
wm.username = "SolarDev";
wm.pingMs = 12;
Solar::Watermark::Render(wm);

// 4. Main Menu Frame
static bool menuOpen = true;
if (Solar::UI::Window::Begin("Solar Framework", &menuOpen, ImVec2(960, 620))) {
    Solar::UI::Titlebar::Render("SOLAR", "PRO SUITE", &menuOpen);

    static int currentTab = 0;
    Solar::UI::Sidebar::Begin(210.0f);
    {
        Solar::UI::Sidebar::Category("MODULES");
        Solar::UI::Sidebar::Tab("Combat", 0, &currentTab, ICON_FA_CROSSHAIRS);
        Solar::UI::Sidebar::Tab("Visuals", 1, &currentTab, ICON_FA_EYE);
        Solar::UI::Sidebar::Tab("Security", 2, &currentTab, ICON_FA_SHIELD);
    }
    Solar::UI::Sidebar::End();

    ImGui::SameLine(0, 10.0f);

    if (currentTab == 0) {
        static bool aimbot = true;
        static float fov = 14.5f;
        Solar::Widgets::Toggle("Enable Aimbot", &aimbot);
        Solar::Widgets::SliderFloat("FOV Angle", &fov, 1.0f, 180.0f, "%.1f", "deg");
    }
}
Solar::UI::Window::End();
```

---

## 📄 Documentation

- [🚀 Getting Started Guide](docs/GETTING_STARTED.md)
- [🧩 Exhaustive Component Catalog](docs/COMPONENTS.md)
- [🎨 Theming & Presets Reference](docs/THEMING.md)
- [🔌 Integration Guide (DLL & External Overlay)](docs/INTEGRATION_GUIDE.md)

---

## ⚖️ License

Distributed under the **MIT License**. Created for tool developers, UI designers, and game security researchers.
