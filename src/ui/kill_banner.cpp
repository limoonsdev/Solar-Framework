#include "solar/ui/kill_banner.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/shadow_caster.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/font_awesome.hpp"
#include <algorithm>
#include <cmath>

namespace Solar::UI {

    KillBanner& KillBanner::Get() {
        static KillBanner instance;
        return instance;
    }

    void KillBanner::TriggerKill(const std::string& victim, const std::string& weapon,
                                 int damage, bool isHeadshot, int killStreak) {
        m_event.victimName = victim;
        m_event.weaponName = weapon;
        m_event.damage = damage;
        m_event.isHeadshot = isHeadshot;
        m_event.killStreak = killStreak;
        m_event.currentTimer = 0.0f;
        m_event.active = true;
        m_event.animProgress = 0.0f;

        // Play high-impact kill confirmation chime
        Audio::PlayNotification();
    }

    void KillBanner::Render() {
        if (!m_event.active) return;

        ImGuiIO& io = ImGui::GetIO();
        float dt = io.DeltaTime;
        m_event.currentTimer += dt;

        // Spring animation target
        float targetAnim = (m_event.currentTimer < (m_event.totalDuration - 0.4f)) ? 1.0f : 0.0f;
        m_event.animProgress += (targetAnim - m_event.animProgress) * (std::min)(1.0f, dt * 14.0f);

        if (m_event.currentTimer >= m_event.totalDuration && m_event.animProgress < 0.02f) {
            m_event.active = false;
            return;
        }

        if (m_event.animProgress < 0.01f) return;

        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        float bannerW = 390.0f;
        float bannerH = 62.0f;
        float posX = (io.DisplaySize.x - bannerW) * 0.5f;
        float targetY = 38.0f;
        float posY = -bannerH + (targetY + bannerH) * m_event.animProgress;

        ImVec2 min(posX, posY);
        ImVec2 max(posX + bannerW, posY + bannerH);
        float rounding = 8.0f;

        // 1. Velvet Drop Shadow
        Render::ShadowCaster::DrawShadow(draw, min, max, 22.0f, rounding, Color(0, 0, 0, 0.65f * m_event.animProgress), ImVec2(0, 6.0f));

        // 2. Obsidian Glass Fill
        u32 bgCol = pal.Card.WithAlpha(0.96f * m_event.animProgress).ToU32();
        draw->AddRectFilled(min, max, bgCol, rounding);

        // Specular edge
        Render::ImGuiExt::DrawSpecularEdge(draw, min, max, IM_COL32(255, 255, 255, static_cast<int>(35 * m_event.animProgress)), rounding, 1.0f);

        // Neon Border (Red/Gold on Headshot, Accent on Normal)
        Color borderNeon = m_event.isHeadshot ? Color(1.0f, 0.22f, 0.28f, 0.85f) : pal.Accent;
        Render::ImGuiExt::AddSmoothBorder(draw, min, max, borderNeon.WithAlpha(borderNeon.a * m_event.animProgress).ToU32(), rounding, 1.2f);

        // 3. Left Skull / Headshot Emblem Badge
        float badgeSize = 42.0f;
        ImVec2 bMin(min.x + 10.0f, min.y + 10.0f);
        ImVec2 bMax(bMin.x + badgeSize, bMin.y + badgeSize);
        u32 badgeBg = (m_event.isHeadshot ? Color(1.0f, 0.15f, 0.25f, 0.25f) : pal.Accent.WithAlpha(0.20f)).ToU32();
        draw->AddRectFilled(bMin, bMax, badgeBg, 6.0f);
        draw->AddRect(bMin, bMax, borderNeon.WithAlpha(0.6f * m_event.animProgress).ToU32(), 6.0f, 1.0f);

        // Skull Icon inside badge
        draw->AddText(ImVec2(bMin.x + 12.0f, bMin.y + 11.0f),
                      (m_event.isHeadshot ? IM_COL32(255, 75, 85, 255) : pal.Accent.ToU32()),
                      ICON_FA_SKULL);

        // 4. Center Typography: Victim Name & Weapon Telemetry
        float textX = bMax.x + 14.0f;
        float textY = min.y + 10.0f;

        // Victim Name (Bold White)
        u32 nameCol = IM_COL32(255, 255, 255, static_cast<int>(255 * m_event.animProgress));
        draw->AddText(ImVec2(textX, textY), nameCol, m_event.victimName.c_str());

        // Subtitle: Headshot / Damage / Weapon
        char statsBuf[128];
        if (m_event.isHeadshot) {
            snprintf(statsBuf, sizeof(statsBuf), "CRITICAL HEADSHOT // %d DMG // %s", m_event.damage, m_event.weaponName.c_str());
        } else {
            snprintf(statsBuf, sizeof(statsBuf), "ELIMINATED // %d DMG // %s", m_event.damage, m_event.weaponName.c_str());
        }
        u32 statsCol = m_event.isHeadshot ? IM_COL32(255, 100, 110, static_cast<int>(240 * m_event.animProgress))
                                          : pal.Accent.WithAlpha(0.90f * m_event.animProgress).ToU32();
        draw->AddText(ImVec2(textX, textY + 18.0f), statsCol, statsBuf);

        // 5. Right Streak / Ace Badge
        if (m_event.killStreak > 1) {
            const char* streakName = (m_event.killStreak == 2) ? "DOUBLE KILL" :
                                     (m_event.killStreak == 3) ? "TRIPLE KILL" :
                                     (m_event.killStreak == 4) ? "QUADRA KILL" : "ACE // PENTAKILL";
            ImVec2 stSize = ImGui::CalcTextSize(streakName);
            ImVec2 stPos(max.x - stSize.x - 14.0f, min.y + 12.0f);
            draw->AddRectFilled(ImVec2(stPos.x - 6.0f, stPos.y - 2.0f), ImVec2(stPos.x + stSize.x + 6.0f, stPos.y + stSize.y + 2.0f),
                                IM_COL32(255, 200, 40, static_cast<int>(45 * m_event.animProgress)), 4.0f);
            draw->AddRect(ImVec2(stPos.x - 6.0f, stPos.y - 2.0f), ImVec2(stPos.x + stSize.x + 6.0f, stPos.y + stSize.y + 2.0f),
                          IM_COL32(255, 200, 40, static_cast<int>(180 * m_event.animProgress)), 4.0f, 1.0f);
            draw->AddText(stPos, IM_COL32(255, 220, 80, static_cast<int>(255 * m_event.animProgress)), streakName);
        }

        // 6. Lifetime Depletion Progress Bar at Bottom
        float lifeFraction = (std::clamp)(1.0f - (m_event.currentTimer / m_event.totalDuration), 0.0f, 1.0f);
        float barY = max.y - 3.0f;
        draw->AddRectFilled(ImVec2(min.x + 8.0f, barY), ImVec2(max.x - 8.0f, max.y - 1.0f),
                            IM_COL32(255, 255, 255, static_cast<int>(25 * m_event.animProgress)), 1.5f);
        float fillW = (bannerW - 16.0f) * lifeFraction;
        draw->AddRectFilled(ImVec2(min.x + 8.0f, barY), ImVec2(min.x + 8.0f + fillW, max.y - 1.0f),
                            borderNeon.WithAlpha(0.95f * m_event.animProgress).ToU32(), 1.5f);
    }

} // namespace Solar::UI
