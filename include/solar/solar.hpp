#pragma once

// Dear ImGui Core
#include <imgui.h>
#include <imgui_internal.h>

// Standard C++20 Core
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>

// ==============================================================================
// 1. Core Foundations
// ==============================================================================
#include "core/types.hpp"
#include "core/math.hpp"
#include "core/color.hpp"
#include "core/timer.hpp"
#include "core/string_utils.hpp"
#include "core/crypto.hpp"
#include "core/logger.hpp"

// ==============================================================================
// 2. Rendering Pipeline & DirectX Abstractions
// ==============================================================================
#include "render/irenderer.hpp"
#include "render/dx11_renderer.hpp"
#include "render/dx12_renderer.hpp"
#include "render/drawlist_utils.hpp"
#include "render/shadow_caster.hpp"
#include "render/glow_engine.hpp"
#include "render/font_manager.hpp"

// ==============================================================================
// 3. Audio Engine & Procedural Sound FX
// ==============================================================================
#include "audio/wave_synth.hpp"
#include "audio/sound_effects.hpp"
#include "audio/audio_engine.hpp"

// ==============================================================================
// 4. Animation Dynamics & Physics Springs
// ==============================================================================
#include "anim/easing.hpp"
#include "anim/spring.hpp"
#include "anim/transitions.hpp"
#include "anim/animation_manager.hpp"

// ==============================================================================
// 5. Themes, Color Theory & Palettes
// ==============================================================================
#include "theme/palette.hpp"
#include "theme/style.hpp"
#include "theme/presets.hpp"
#include "theme/theme_manager.hpp"

// ==============================================================================
// 6. UI Structure & Containers
// ==============================================================================
#include "ui/window.hpp"
#include "ui/titlebar.hpp"
#include "ui/sidebar.hpp"
#include "ui/subtabs.hpp"
#include "ui/card.hpp"
#include "ui/groupbox.hpp"
#include "ui/modal.hpp"
#include "ui/tooltip.hpp"
#include "ui/splash_screen.hpp"

// ==============================================================================
// 7. Interactive Widgets Suite
// ==============================================================================
#include "widgets/toggle.hpp"
#include "widgets/slider.hpp"
#include "widgets/combo.hpp"
#include "widgets/multi_combo.hpp"
#include "widgets/keybind.hpp"
#include "widgets/color_picker.hpp"
#include "widgets/button.hpp"
#include "widgets/input_text.hpp"
#include "widgets/checkbox.hpp"
#include "widgets/progress_bar.hpp"
#include "widgets/badge.hpp"
#include "widgets/separator.hpp"

// ==============================================================================
// 8. Game, Visuals & ESP Components
// ==============================================================================
#include "game/esp_preview.hpp"
#include "game/hitbox_picker.hpp"
#include "game/radar.hpp"
#include "game/spectator_list.hpp"
#include "game/keybind_list.hpp"
#include "game/watermark.hpp"
#include "game/chams_preview.hpp"
#include "game/visuals_renderer.hpp"

// ==============================================================================
// 9. Security & Anti-Cheat Utilities
// ==============================================================================
#include "security/hwid_spoofer.hpp"
#include "security/trace_cleaner.hpp"
#include "security/memory_tools.hpp"

// ==============================================================================
// 10. Authentication & Loader Entitlements
// ==============================================================================
#include "auth/license_screen.hpp"
#include "auth/hwid_generator.hpp"
#include "auth/auth_manager.hpp"

// ==============================================================================
// 11. Profile Configuration & Storage
// ==============================================================================
#include "config/json_serializer.hpp"
#include "config/preset_storage.hpp"
#include "config/config_manager.hpp"

// ==============================================================================
// 12. Notification Toasts
// ==============================================================================
#include "notifications/toast.hpp"
#include "notifications/notification_system.hpp"

// ==============================================================================
// 13. Visual FX & Ambient Particles
// ==============================================================================
#include "fx/particle_system.hpp"
#include "fx/glow_fx.hpp"

// ==============================================================================
// 14. Developer Telemetry & Profiler
// ==============================================================================
#include "tools/profiler.hpp"
#include "tools/inspector.hpp"

// Fonts & Icons
#include "font_awesome.hpp"
#include "icons.hpp"

// Backward-Compatibility Aliases
namespace Solar {

    using ESPPreviewSettings = Game::ESPSettings;
    using ESPPreview = Game::ESPPreview;
    using WatermarkInfo = Game::WatermarkInfo;
    using Watermark = Game::Watermark;
    using Visuals = Game::VisualsRenderer;
    using VisualsRenderer = Game::VisualsRenderer;
    using BoxStyle = Game::BoxStyle;
    using BarPosition = Game::BarPosition;
    using SnaplineOrigin = Game::SnaplineOrigin;
    using SpooferState = Security::SpooferState;

    namespace Widgets {
        inline bool BeginWindow(const char* name, bool* p_open, const ImVec2& size = ImVec2(940, 620)) {
            return UI::Window::Begin(name, p_open, size);
        }
        inline void EndWindow() {
            UI::Window::End();
        }

        inline void RenderTitlebar(const char* title, const char* subtitle, bool* p_open, bool* p_minimized = nullptr) {
            UI::Titlebar::Render(title, subtitle, p_open, p_minimized);
        }

        inline void BeginSidebar(float width = 210.0f) {
            UI::Sidebar::Begin(width);
        }
        inline void EndSidebar() {
            UI::Sidebar::End();
        }
        inline void SidebarCategory(const char* title) {
            UI::Sidebar::Category(title);
        }
        inline bool SidebarTab(const char* label, IconType icon, int tabIndex, int* selectedTab, int badgeCount = 0, const char* fontAwesomeIcon = nullptr) {
            return UI::Sidebar::Tab(label, tabIndex, selectedTab, fontAwesomeIcon);
        }

        inline bool SubTab(const char* label, int subIndex, int* selectedSub) {
            return UI::Subtabs::Tab(label, subIndex, selectedSub);
        }

        inline bool BeginCard(const char* id, const char* title, IconType icon = IconType::None, const ImVec2& size = ImVec2(0, 0), const char* fontAwesomeIcon = nullptr) {
            return UI::Card::Begin(id, title, size, fontAwesomeIcon);
        }
        inline void EndCard() {
            UI::Card::End();
        }

        inline void HitboxPicker(std::vector<bool>& hitboxes, const ImVec2& size = ImVec2(0, 220.0f)) {
            Game::HitboxPicker::Render(hitboxes, size);
        }

        inline void RadarPreview(const char* id, const ImVec2& size = ImVec2(240, 240)) {
            std::vector<Game::RadarBlip> blips = {
                { ImVec2(35, -45), 0, true, false, 1 },
                { ImVec2(-60, 20), 0, true, false, 0 },
                { ImVec2(15, 75), 0, false, false, -1 },
                { ImVec2(-25, -30), 0, true, false, 0 }
            };
            Game::Radar::Render(id, size, blips);
        }

        inline void SpectatorList(bool* p_open, const std::vector<std::string>& spectators) {
            Game::SpectatorList::Render(p_open, spectators);
        }

        inline void KeybindList(bool* p_open, const std::vector<std::pair<std::string, std::string>>& binds) {
            Game::KeybindList::Render(p_open, binds);
        }

        inline void LicenseScreen(char* keyBuf, size_t bufSize, const char* hwid, bool* rememberMe, bool* loggedIn) {
            Auth::LicenseScreen::Render(keyBuf, bufSize, hwid, rememberMe, loggedIn);
        }

        inline void SpooferPanel(Security::SpooferState& state) {
            Security::SpooferPanel::Render(state);
        }
    }

    // Engine Lifecycle
    void Initialize();
    void NewFrame();
    void Shutdown();

} // namespace Solar
