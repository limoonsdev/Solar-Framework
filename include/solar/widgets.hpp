#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include "theme.hpp"
#include "anim.hpp"
#include "icons.hpp"
#include "audio.hpp"

namespace Solar {

    enum class ButtonStyle {
        Primary,    // Solar Gradient fill + active glow
        Secondary,  // Dark outline card with hover border glow
        Danger,     // Crimson gradient
        Ghost       // Transparent flat button
    };

    enum KeybindMode {
        Always = 0,
        Hold = 1,
        ToggleMode = 2
    };

    struct HitboxSelection {
        bool head = true;
        bool neck = true;
        bool chest = true;
        bool stomach = false;
        bool arms = false;
        bool legs = false;
    };

    struct SpooferState {
        char moboUUID[48]   = "04B2-881A-CC49-EE12-0091";
        char diskSerial[48] = "WDC-WD10EZEX-75M2NA0";
        char macAddress[32] = "00:1A:7D:DA:71:04";
        char gpuUUID[48]    = "GPU-3B91-A894-CC22";
        bool cleanDiscord   = true;
        bool cleanVanguard  = true;
        bool cleanEAC       = true;
        bool cleanBattlEye  = true;
        bool cleanFiveM     = true;
        float progress      = 0.0f;
        bool isSpoofing     = false;
    };

    namespace Widgets {

        // ==========================================
        // Window & Layout
        // ==========================================
        bool BeginWindow(const char* name, bool* p_open, const ImVec2& size = ImVec2(940, 620), ImGuiWindowFlags extraFlags = 0);
        void EndWindow();

        // Custom Titlebar Header info
        void RenderTitlebar(const char* title, const char* subtitle, bool* p_open, bool* p_minimized = nullptr);

        // Navigation Sidebar
        void BeginSidebar(float width = 200.0f);
        void SidebarCategory(const char* title);
        bool SidebarTab(const char* label, IconType icon, int tabIndex, int* selectedTab, int badgeCount = 0, const char* fontAwesomeIcon = nullptr);
        void EndSidebar();

        // Sub-tabs (horizontal pills)
        bool SubTab(const char* label, int subIndex, int* selectedSub);

        // Modern Card / Groupbox Container
        bool BeginCard(const char* id, const char* title, IconType icon = IconType::None, const ImVec2& size = ImVec2(0, 0), const char* fontAwesomeIcon = nullptr);
        void EndCard();

        // ==========================================
        // Form & Input Controls
        // ==========================================
        bool Toggle(const char* label, bool* v, const char* description = nullptr);
        bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.1f", const char* suffix = nullptr);
        bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", const char* suffix = nullptr);

        bool Combo(const char* label, int* current_item, const char* const items[], int items_count);
        bool MultiCombo(const char* label, std::vector<bool>& states, const std::vector<std::string>& items);

        bool Keybind(const char* label, int* key, int* mode = nullptr);
        bool ColorPicker(const char* label, float col[4]);
        bool InputText(const char* label, char* buf, size_t buf_size, const char* placeholder = nullptr);

        bool Button(const char* label, const ImVec2& size = ImVec2(0, 0), ButtonStyle style = ButtonStyle::Primary);
        bool IconButton(const char* id, IconType icon, const ImVec2& size = ImVec2(32, 32), const char* tooltip = nullptr);

        // ==========================================
        // Specialized Cheat & Tool Widgets
        // ==========================================
        // Interactive 2D Hitbox Picker Matrix
        void HitboxPicker(HitboxSelection& hitboxes, const ImVec2& size = ImVec2(180, 220));

        // 2D Tactical Mini-Radar
        void RadarPreview(const char* id, const ImVec2& size = ImVec2(240, 240));

        // Floating HUD Overlays
        void SpectatorList(bool* p_open, const std::vector<std::string>& spectators);
        void KeybindList(bool* p_open, const std::vector<std::pair<std::string, std::string>>& binds);

        // Auth & Loader Screen
        bool LicenseScreen(char* keyBuf, size_t bufSize, const char* hwid, bool* rememberMe, bool* loggedIn);

        // HWID Spoofer & Trace Cleaner Panel
        void SpooferPanel(SpooferState& state);

        // ==========================================
        // Visual Utilities
        // ==========================================
        void Badge(const char* text, const ImVec4& bgColor, const ImVec4& textColor = ImVec4(1, 1, 1, 1));
        void Separator(float thickness = 1.0f);
        void Spacing(float height = 8.0f);

        // Custom DrawList Helpers
        void DrawGlow(ImDrawList* drawList, const ImVec2& min, const ImVec2& max, ImU32 color, float size = 10.0f, float rounding = 6.0f);
        void DrawGradientRect(ImDrawList* drawList, const ImVec2& min, const ImVec2& max, ImU32 colLeft, ImU32 colRight, float rounding = 6.0f);
        void DrawShadowRect(ImDrawList* drawList, const ImVec2& min, const ImVec2& max, ImU32 shadowColor, float shadowSize = 16.0f, float rounding = 10.0f);

    } // namespace Widgets

} // namespace Solar
