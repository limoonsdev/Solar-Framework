#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <string>
#include <imgui.h>

namespace Solar::UI {

    struct KillEvent {
        std::string victimName;
        std::string weaponName;
        int damage = 150;
        bool isHeadshot = true;
        int killStreak = 1;
        float totalDuration = 3.6f;
        float currentTimer = 0.0f;
        bool active = false;
        float animProgress = 0.0f; // 0.0f = offscreen, 1.0f = fully visible
    };

    /**
     * @brief Esports Animated Kill Banner & Frag Notification Popup
     * Displays a high-impact combat killfeed banner with spring slide-in,
     * headshot badge, damage telemetry, lifetime progress, and sound FX.
     */
    class KillBanner {
    public:
        static KillBanner& Get();

        void TriggerKill(const std::string& victim, const std::string& weapon = "VANDAL PRIME",
                         int damage = 150, bool isHeadshot = true, int killStreak = 1);

        void Render();
        bool IsActive() const { return m_event.active; }

    private:
        KillBanner() = default;
        KillEvent m_event;
    };

    inline void TriggerKillBanner(const std::string& victim, const std::string& weapon = "VANDAL PRIME",
                                 int damage = 150, bool isHeadshot = true, int killStreak = 1) {
        KillBanner::Get().TriggerKill(victim, weapon, damage, isHeadshot, killStreak);
    }

} // namespace Solar::UI
