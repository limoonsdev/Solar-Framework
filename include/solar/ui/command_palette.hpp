#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <string>
#include <vector>
#include <functional>
#include <imgui.h>

namespace Solar::UI {

    struct CommandItem {
        std::string title;
        std::string category;
        std::string shortcut;
        const char* icon = nullptr;
        std::function<void()> action;
    };

    /**
     * @brief High-tech Quick Command Palette (Ctrl + P)
     * Provides instant fuzzy-searchable access to all tabs, theme presets,
     * visual effects, combat triggers, and system actions with smooth keyboard navigation.
     */
    class CommandPalette {
    public:
        static CommandPalette& Get();

        void Toggle();
        void Open();
        void Close();
        bool IsOpen() const { return m_open; }

        void RegisterCommand(const std::string& title, const std::string& category,
                             const std::string& shortcut, const char* icon, std::function<void()> action);
        void ClearCommands();

        /**
         * @brief Main render function. Call every frame.
         * Handles Ctrl+P shortcut, overlay dimming, animations, keyboard navigation, and execution.
         */
        void Render();

    private:
        CommandPalette();
        void PopulateDefaultCommands();

        bool m_open = false;
        float m_animProgress = 0.0f;
        char m_filter[128] = "";
        int m_selectedIndex = 0;
        bool m_needFocus = false;
        std::vector<CommandItem> m_commands;
    };

    inline void ToggleCommandPalette() {
        CommandPalette::Get().Toggle();
    }

} // namespace Solar::UI
