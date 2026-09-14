# 🎨 Theming & Styling in Solar Framework

Solar Framework features a highly customizable, hardware-accelerated aesthetic engine inspired by PastOwl cheat designs.

---

## 🌈 Built-in Color Presets

Solar comes out-of-the-box with 6 professionally tuned high-contrast dark theme presets:

| Preset Name | Accent Hex | Gradient Secondary | Look & Feel |
| :--- | :--- | :--- | :--- |
| **Solar Flare** *(Default)* | `#FF7A00` | `#FFAE26` | Luxurious Amber & Solar Gold glow |
| **Cyber Neon** | `#00E5FF` | `#00FFB2` | Electric Cyan & Mint green neon |
| **Void Amethyst** | `#A855F7` | `#C084FC` | Deep purple & lilac twilight |
| **Emerald Matrix** | `#10B981` | `#34D399` | Crisp emerald & matrix green |
| **Blood Ruby** | `#F43F5E` | `#FB7185` | Aggressive crimson & rose |
| **Arctic White** | `#E2E8F0` | `#60A5FA` | Clean frost slate with ice-blue glow |

### Switching Presets Programmatically

```cpp
#include <solar/theme.hpp>

// Switch to Cyber Neon:
Solar::ThemeManager::Get().ApplyPreset(Solar::ThemePreset::CyberNeon);

// Switch to Void Amethyst:
Solar::ThemeManager::Get().ApplyPreset(Solar::ThemePreset::VoidAmethyst);
```

---

## 🖌️ Custom RGB Colors

You can set any custom accent color in real time:

```cpp
ImVec4 primaryAccent(1.0f, 0.2f, 0.5f, 1.0f);   // Hot Pink
ImVec4 gradientAccent(1.0f, 0.4f, 0.7f, 1.0f);  // Light Pink

Solar::ThemeManager::Get().SetCustomAccent(primaryAccent, gradientAccent);
```

---

## 📐 Roundings & Visual Effects

Customize geometry, particle density, and glow through `Solar::StyleSettings`:

```cpp
auto& style = Solar::ThemeManager::Get().GetStyle();

// Roundings
style.WindowRounding    = 12.0f; // Main window corners
style.ChildRounding     = 10.0f; // Card / container corners
style.FrameRounding     = 7.0f;  // Sliders, inputs, toggles
style.PillRounding      = 18.0f; // Badges and tab pills

// Glow & Shadows
style.EnableGlow        = true;  // Multi-pass glow around active elements
style.GlowIntensity     = 1.2f;  // Multiplier for glow size/alpha
style.EnableShadows     = true;  // Multi-pass drop shadows under windows

// Particles
style.EnableParticles   = true;  // Interactive background plexus mesh
style.ParticleCount     = 50;    // Number of floating particles

// Apply modifications to ImGui
Solar::ThemeManager::Get().ApplyToImGui();
```

---

## ⚡ Animation Easing Curves

Solar's animation engine (`Solar::AnimationManager`) supports framerate-independent easing curves:

- `EasingType::Linear`
- `EasingType::InQuad`
- `EasingType::OutQuad` (Smooth deceleration for tabs and hovers)
- `EasingType::InOutQuad`
- `EasingType::OutCubic`
- `EasingType::OutBack` (Signature bouncy toggle switch effect)

To animate custom variables:
```cpp
// Returns smoothly interpolated float towards 1.0f or 0.0f
float animProgress = Solar::AnimLerp(widgetID, targetValue, 14.0f, Solar::EasingType::OutQuad);
```
