#include "solar/ui/welcome_screen.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/fx/rotating_border.hpp"
#include "solar/icons/vector_icons.hpp"
#include <imgui.h>
#include <cmath>
#include <algorithm>

namespace Solar::UI {

    WelcomeScreen& WelcomeScreen::Get() {
        static WelcomeScreen instance;
        return instance;
    }

    bool WelcomeScreen::Render() {
        if (!m_visible) return false;

        ImGuiIO& io = ImGui::GetIO();
        float dt = io.DeltaTime;
        m_alpha = (std::min)(1.0f, m_alpha + dt * 4.0f);

        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();
        float timeSec = static_cast<float>(ImGui::GetTime());

        // 1. Fullscreen Dimmer Backdrop
        draw->AddRectFilled(
            ImVec2(0, 0), io.DisplaySize,
            IM_COL32(5, 6, 9, static_cast<int>(210 * m_alpha))
        );

        // 2. Modal Box Dimensions
        float modalW = 540.0f;
        float modalH = 360.0f;
        ImVec2 min((io.DisplaySize.x - modalW) * 0.5f, (io.DisplaySize.y - modalH) * 0.5f);
        ImVec2 max(min.x + modalW, min.y + modalH);
        float rounding = 10.0f;

        // Soft layered shadow
        for (int i = 1; i <= 4; ++i) {
            float exp = static_cast<float>(i) * 3.0f;
            draw->AddRectFilled(
                ImVec2(min.x - exp, min.y - exp + 4.0f),
                ImVec2(max.x + exp, max.y + exp + 4.0f),
                IM_COL32(0, 0, 0, static_cast<int>((25 / i) * m_alpha)),
                rounding + exp
            );
        }

        // Obsidian Glass Container
        draw->AddRectFilled(min, max, IM_COL32(13, 14, 19, static_cast<int>(250 * m_alpha)), rounding);

        // Rotating Glowing Solar Border
        FX::RotatingBorder::Draw(
            draw, min, max, rounding,
            pal.Accent, Color(1.0f, 0.35f, 0.08f, 1.0f),
            1.8f, 1.6f, 1.2f * m_alpha
        );

        // 3. Central Brand Cyber Emblem
        float centerX = min.x + modalW * 0.5f;
        float curY = min.y + 36.0f;

        {
            ImVec2 emblemC(centerX, curY);
            float pulse = 0.92f + 0.08f * std::sin(timeSec * 3.0f);
            float sz = 18.0f * pulse;

            // Ambient glow behind emblem
            draw->AddCircleFilled(emblemC, sz * 1.5f, pal.Accent.WithAlpha(0.25f * m_alpha).ToU32(), 24);

            // Sleek Cyber Diamond
            ImVec2 pTop(emblemC.x, emblemC.y - sz);
            ImVec2 pRight(emblemC.x + sz, emblemC.y);
            ImVec2 pBottom(emblemC.x, emblemC.y + sz);
            ImVec2 pLeft(emblemC.x - sz, emblemC.y);

            draw->AddQuadFilled(pTop, pRight, pBottom, pLeft, pal.Accent.WithAlpha(0.20f * m_alpha).ToU32());
            draw->AddQuad(pTop, pRight, pBottom, pLeft, pal.Accent.WithAlpha(0.90f * m_alpha).ToU32(), 1.5f);

            // Core center dot
            draw->AddCircleFilled(emblemC, 3.5f, IM_COL32(255, 255, 255, static_cast<int>(240 * m_alpha)), 12);

            curY += 34.0f;
        }

        // 4. Welcome Title & Subtitle
        const char* titleText = "WELCOME TO SOLAR FRAMEWORK";
        ImVec2 tsTitle = ImGui::CalcTextSize(titleText);
        draw->AddText(ImVec2(centerX - tsTitle.x * 0.5f, curY), pal.TextPrimary.WithAlpha(m_alpha).ToU32(), titleText);
        curY += tsTitle.y + 6.0f;

        const char* subText = "DirectX 11 / 12 High-Performance Visuals & Kernel Security";
        ImVec2 tsSub = ImGui::CalcTextSize(subText);
        draw->AddText(ImVec2(centerX - tsSub.x * 0.5f, curY), pal.TextSecondary.WithAlpha(m_alpha).ToU32(), subText);
        curY += tsSub.y + 24.0f;

        // 5. Entitlement Pills
        struct BadgeItem {
            const char* label;
            const char* val;
            Color col;
        };

        BadgeItem badges[] = {
            { "LICENSE", "LIFETIME DEV", Color(0.20f, 0.85f, 0.35f, 1.0f) },
            { "HWID SPOOFER", "ACTIVE & MASKED", Color(0.25f, 0.70f, 1.0f, 1.0f) },
            { "RENDER PIPELINE", "DIRECT3D 11/12", Color(1.0f, 0.75f, 0.15f, 1.0f) }
        };

        float badgeW = 150.0f;
        float badgeH = 46.0f;
        float spacingX = 12.0f;
        float startBadgeX = centerX - (badgeW * 3.0f + spacingX * 2.0f) * 0.5f;

        for (int i = 0; i < 3; ++i) {
            ImVec2 bMin(startBadgeX + i * (badgeW + spacingX), curY);
            ImVec2 bMax(bMin.x + badgeW, bMin.y + badgeH);

            draw->AddRectFilled(bMin, bMax, IM_COL32(18, 20, 26, static_cast<int>(240 * m_alpha)), 6.0f);
            draw->AddRect(bMin, bMax, badges[i].col.WithAlpha(0.35f * m_alpha).ToU32(), 6.0f, 0, 1.0f);

            ImVec2 tsL = ImGui::CalcTextSize(badges[i].label);
            ImVec2 tsV = ImGui::CalcTextSize(badges[i].val);

            draw->AddText(ImVec2(bMin.x + (badgeW - tsL.x) * 0.5f, bMin.y + 7.0f),
                          pal.TextDisabled.WithAlpha(m_alpha).ToU32(), badges[i].label);
            draw->AddText(ImVec2(bMin.x + (badgeW - tsV.x) * 0.5f, bMin.y + 22.0f),
                          badges[i].col.WithAlpha(m_alpha).ToU32(), badges[i].val);
        }

        curY += badgeH + 28.0f;

        // 6. Interactive CTA Button "ENTER WORKSPACE"
        float btnW = 200.0f;
        float btnH = 38.0f;
        ImVec2 btnMin(centerX - btnW * 0.5f, curY);
        ImVec2 btnMax(btnMin.x + btnW, btnMin.y + btnH);

        bool hovered = io.MousePos.x >= btnMin.x && io.MousePos.x <= btnMax.x &&
                       io.MousePos.y >= btnMin.y && io.MousePos.y <= btnMax.y;
        bool clicked = hovered && io.MouseClicked[0];

        u32 btnBg = hovered ? pal.AccentHover.WithAlpha(m_alpha).ToU32() : pal.Accent.WithAlpha(m_alpha).ToU32();
        draw->AddRectFilled(btnMin, btnMax, btnBg, 6.0f);

        const char* btnLabel = "ENTER WORKSPACE";
        ImVec2 tsBtn = ImGui::CalcTextSize(btnLabel);
        draw->AddText(
            ImVec2(centerX - tsBtn.x * 0.5f, btnMin.y + (btnH - tsBtn.y) * 0.5f),
            IM_COL32(255, 255, 255, static_cast<int>(255 * m_alpha)),
            btnLabel
        );

        if (clicked) {
            Audio::PlayClick();
            m_visible = false;
        }

        return true;
    }

} // namespace Solar::UI
