# 🧩 Solar Framework v1.0.1 - Complete 14 Subsystems Architecture Reference

Exhaustive API documentation for every component in Solar Framework.

---

## 🏛️ 1. Core Foundations (`Solar::Core` & `Solar::Math`)

### `Solar::Math`
- `Math::Lerp(a, b, t)`: Scalar, `ImVec2`, and `ImVec4` smooth interpolation.
- `Math::Clamp(val, min, max)`: Bound values safely.
- `Math::SmoothStep(e0, e1, x)`: Hermite polynomial transition.
- `Math::Distance(p1, p2)`: Euclidean 2D distance calculation.

### `Solar::Color`
- `Color::FromHex("#FF9E1B")`: 6 or 8 character hex color parsing.
- `Color::FromHSV(h, s, v, a)`: High precision HSV to RGB converter.
- `color.WithAlpha(a)`: Non-destructive alpha modulation.
- `color.ToU32()`: Direct `ImU32` conversion for `ImDrawList`.

### `Solar::Crypto`
- `Crypto::GenerateRandomGUID()`: Standard RFC 4122 compliant UUID generator for SMBIOS / hardware virtualization.
- `Crypto::GenerateMACAddress()`: Physical NIC hardware MAC generator.
- `Crypto::GenerateDiskSerial()`: Storage drive volume serial randomizer.

### `Solar::Timer` & `Solar::FrameTimeTracker`
- High resolution QPC time keeping, delta-time calculation, and rolling average FPS tracker.

---

## 🎨 2. Rendering Pipeline & DirectX Abstractions (`Solar::Render`)

### `Solar::Render::DX11Renderer`
- Complete DirectX 11 device, context, and swapchain encapsulation.
- Dynamic resize handling on `WM_SIZE`.

### `Solar::Render::DX12Renderer`
- Modern DirectX 12 descriptor heap and command list abstraction.

### `Solar::Render::ShadowCaster`
- Velvet drop shadows using 6 multi-pass gradient layers.
- Eliminates DWM bilinear blur and ugly box artifacts.
```cpp
Solar::Render::ShadowCaster::DrawShadow(draw, min, max, 24.0f, 8.0f, Color(0, 0, 0, 0.65f), ImVec2(0, 8.0f));
```

### `Solar::Render::GlowEngine`
- Volumetric neon glow and ambient halo effects for active widgets.
```cpp
Solar::Render::GlowEngine::DrawGlowRect(draw, min, max, accentColor, 12.0f, 6.0f, intensity);
```

### `Solar::Render::FontManager`
- High-DPI font atlas generator with 3x oversampling (`OversampleH = 3, OversampleV = 3`) and contrast boosting (`RasterizerMultiply = 1.18f`).
- Merges Segoe UI SemiBold with FontAwesome 6 Free Solid icons.

---

## 🔊 3. Procedural Audio Engine (`Solar::Audio`)

Real-time in-memory PCM 16-bit 44.1 kHz synthesizer (`winmm.lib`):
- `Audio::PlayClick()`: Crisp tactile mechanical switch click (2400Hz -> 800Hz).
- `Audio::PlayToggle(bool on)`: Ascending (580Hz -> 940Hz) or descending pitch chirp.
- `Audio::PlayNotification()`: 3-tone harmonious triad chime (C6, E6, G6).
- `Audio::PlayWarning()`: Alert tone (420Hz).
- `Audio::PlayError()`: Dissonant tritone chord (Eb5 + A4).
- `Audio::PlaySliderTick()`: Micro-click for increments.
- `Audio::PlayTabSwitch()`: Smooth page swoosh.

---

## 🌀 4. Animation Dynamics (`Solar::Anim`)

### `Solar::Anim::Spring`
Second-order harmonic oscillator simulation:
```cpp
Spring spring(0.0f);
spring.stiffness = 180.0f;
spring.damping = 18.0f;
spring.SetTarget(1.0f);
spring.Update(dt);
```

### `Solar::Anim::AnimationManager`
Persistent state machine keyed by `ImGuiID`:
```cpp
float anim = Solar::Anim::AnimationManager::Get().Transition(id, isHovered, 14.0f);
```

---

## 🎭 5. Theme & Color Presets (`Solar::ThemeManager`)

8 Master luxury dark presets:
1. `ThemePreset::SolarFlare` (Amber Gold `#FF9E1B`)
2. `ThemePreset::CyberNeon` (Electric Cyan `#00F0FF`)
3. `ThemePreset::VoidAmethyst` (Royal Purple `#A855F7`)
4. `ThemePreset::EmeraldMatrix` (Toxic Mint Green `#10B981`)
5. `ThemePreset::BloodRuby` (Crimson Rose `#F43F5E`)
6. `ThemePreset::ArcticWhite` (Platinum Ice Blue `#38BDF8`)
7. `ThemePreset::TokyoNight` (Midnight Indigo `#7AA2F7`)
8. `ThemePreset::MidnightCarbon` (Monochromatic Slate `#94A3B8`)

```cpp
Solar::ThemeManager::Get().ApplyPreset(Solar::ThemePreset::CyberNeon);
```

---

## 🖼️ 6. UI Structure & Containers (`Solar::UI`)

- `Solar::UI::Window::Begin(name, p_open, size)` / `End()`: Window frame with ambient drop shadow, obsidian background, particles, and border.
- `Solar::UI::Titlebar::Render(title, subtitle, p_open, p_min)`: PastOwl header with logo, live state, and control buttons.
- `Solar::UI::Sidebar::Begin(width)` / `Category()` / `Tab()` / `End()`: Vertical navigation with active sliding indicator.
- `Solar::UI::Subtabs::Tab(label, id, currentSub)`: Horizontal pill sub-navigation.
- `Solar::UI::Card::Begin(id, title, size, icon)` / `End()`: Elevated card container.
- `Solar::UI::Groupbox::Begin()` / `End()`: Compact group frame.
- `Solar::UI::Modal::Begin()` / `End()`: Centered overlay dialog with backdrop dimming.
- `Solar::UI::Tooltip::Show()`: Stylized tooltip with dark obsidian styling.

---

## 🎛️ 7. Interactive Widgets Suite (`Solar::Widgets`)

- `Widgets::Toggle(label, v, description)`: Modern switch with glowing pill knob and audio feedback.
- `Widgets::SliderFloat(label, v, min, max, format, suffix)`: Gradient fill slider with thumb glow and value readout.
- `Widgets::SliderInt(label, v, min, max, format, suffix)`: Integer slider.
- `Widgets::Combo(label, current, items, count)`: Dropdown list.
- `Widgets::MultiCombo(label, values, items)`: Multi-select dropdown with chip preview.
- `Widgets::Keybind(label, key, mode)`: Hotkey listener (`Always`, `Hold`, `Toggle`).
- `Widgets::ColorPicker(label, col)`: Color preview chip with HSV popover.
- `Widgets::Button(label, size, style)`: Primary, Secondary, Danger, or Ghost button with glow.
- `Widgets::InputText(label, buf, size, placeholder)`: Text input with placeholder.
- `Widgets::Checkbox(label, v)`: Classic checkbox with checkmark.
- `Widgets::ProgressBar(fraction, size, overlay)`: Animated progress bar.
- `Widgets::Badge(text, color)`: Status tag.
- `Widgets::Separator()` / `Spacing(h)`: Layout dividers.

---

## 🎯 8. Game & Cheat Visuals (`Solar::Game`)

- `Game::ESPPreview::Render(id, size, settings)`: Interactive 2D mannequin (Head, Torso, Spine, Limbs) with weapon tag, health bar, armor bar, distance tag, box styles (Full, Corner, Filled), and snaplines.
- `Game::HitboxPicker::Render(hitboxes, size)`: 2D human wireframe with clickable zones (Head, Neck, Chest, Stomach, Pelvis, Arms, Legs, Feet).
- `Game::Radar::Render(id, size, blips)`: 2D tactical minimap with distance rings, FOV cone, enemy/friendly blips, and altitude arrows.
- `Game::SpectatorList::Render(p_open, specs)`: Floating HUD window for observer detection.
- `Game::KeybindList::Render(p_open, binds)`: Floating HUD window for engaged hotkeys.
- `Game::Watermark::Render(info)`: Luxury status overlay with pulsing amber LED, username, FPS, and ping.

---

## 🛡️ 9. Security & HWID Spoofer (`Solar::Security`)

- `Security::SpooferPanel::Render(state)`: Interactive hardware spoofing dashboard.
  - SMBIOS & Motherboard UUID virtualizer
  - Physical MAC address randomizer
  - Disk serial number masker
  - Display adapter GUID spoofer
  - Trace cleaners: Vanguard, EAC, BattlEye, FiveM, Discord, Steam USN journals.

---

## 🔐 10. Authentication & Loader (`Solar::Auth`)

- `Auth::LicenseScreen::Render(key, size, hwid, remember, loggedIn)`: Loader login panel with hardware fingerprint display and 1-click clipboard copy.
- `Auth::HWIDGenerator::GetMachineFingerprint()`: Cryptographic hardware volume serial hash.

---

## 💾 11. Profile & Config Storage (`Solar::Config`)

- `Config::SimpleJson`: Lightweight self-contained JSON serializer and parser.
- `Config::PresetStorage`: Disk configuration persistence.
- `ConfigManager::Get().SaveConfig("Legit_MM")`: Save profile.
- `ConfigManager::Get().LoadConfig("Legit_MM")`: Load profile.

---

## 🔔 12. Notification Toasts (`Solar::Notify`)

- `Notify::Success(title, message)`
- `Notify::Warning(title, message)`
- `Notify::Error(title, message)`
- `Notify::Info(title, message)`

Features smooth slide-in, auto-dismissal progress countdown, and corresponding procedural audio cues.

---

## ✨ 13. FX & Particles (`Solar::FX`)

- `FX::ParticleSystem`: 45+ floating embers with cursor repulsion and sinusoidal alpha fading.
- `FX::GlowFX`: Ambient top radial lighting.

---

## 📊 14. Developer Telemetry & Profiler (`Solar::Tools`)

- `Tools::Profiler::Render(p_open)`: Live FPS stream graph, frame delta time in milliseconds, draw call count, vertex and index count.
- `Tools::Inspector::Render(p_open)`: Inspect active window name, hovered item ID, and active item ID.
