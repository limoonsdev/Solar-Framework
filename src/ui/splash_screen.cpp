#include "solar/ui/splash_screen.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/core/math.hpp"
#include "solar/render/glow_engine.hpp"
#include <imgui.h>
#include <cmath>

namespace Solar::UI {

    SplashScreen& SplashScreen::Get() {
        static SplashScreen instance;
        return instance;
    }

    void SplashScreen::Start(float durationSeconds) {
        m_duration = durationSeconds;
        m_elapsed = 0.0f;
        m_active = true;
        m_finished = false;
        m_alpha = 1.0f;
    }

    bool SplashScreen::Render() {
        if (!m_active) return false;

        float dt = ImGui::GetIO().DeltaTime;
        m_elapsed += dt;

        float progress = Math::Clamp(m_elapsed / m_duration, 0.0f, 1.0f);

        if (m_duration - m_elapsed < 0.35f) {
            m_alpha = Math::Clamp((m_duration - m_elapsed) / 0.35f, 0.0f, 1.0f);
        }

        if (m_elapsed >= m_duration) {
            m_active = false;
            m_finished = true;
            return false;
        }

        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        ImVec2 screenMin(0, 0);
        ImVec2 screenMax(io.DisplaySize.x, io.DisplaySize.y);
        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

        draw->AddRectFilled(screenMin, screenMax, Color(0.035f, 0.040f, 0.055f, 0.97f * m_alpha).ToU32());

        float cardW = 420.0f;
        float cardH = 260.0f;
        ImVec2 cardMin(center.x - cardW * 0.5f, center.y - cardH * 0.5f);
        ImVec2 cardMax(center.x + cardW * 0.5f, center.y + cardH * 0.5f);

        draw->AddRectFilled(cardMin, cardMax, pal.Card.WithAlpha(0.96f * m_alpha).ToU32(), 12.0f);
        draw->AddRect(cardMin, cardMax, pal.Border.WithAlpha(m_alpha).ToU32(), 12.0f, 0, 1.5f);

        Render::GlowEngine::DrawGlowRect(draw, cardMin, cardMax, pal.Accent, 20.0f, 12.0f, 0.75f * m_alpha);

        float spinRadius = 24.0f;
        ImVec2 spinnerCenter(center.x, cardMin.y + 64.0f);
        float angle = m_elapsed * 5.0f;

        draw->AddCircle(spinnerCenter, spinRadius, pal.Border.WithAlpha(0.4f * m_alpha).ToU32(), 32, 2.0f);
        draw->PathArcTo(spinnerCenter, spinRadius, angle, angle + 2.2f, 24);
        draw->PathStroke(pal.Accent.WithAlpha(m_alpha).ToU32(), 0, 3.0f);
        draw->AddCircleFilled(spinnerCenter, 7.0f, pal.Accent.WithAlpha(m_alpha).ToU32(), 16);

        const char* brand = "SOLAR FRAMEWORK";
        ImVec2 brandSize = ImGui::CalcTextSize(brand);
        draw->AddText(ImVec2(center.x - brandSize.x * 0.5f, cardMin.y + 110.0f),
                      pal.TextPrimary.WithAlpha(m_alpha).ToU32(), brand);

        const char* sub = "v1.0.1  •  Under Development";
        ImVec2 subSize = ImGui::CalcTextSize(sub);
        draw->AddText(ImVec2(center.x - subSize.x * 0.5f, cardMin.y + 130.0f),
                      pal.Accent.WithAlpha(m_alpha).ToU32(), sub);

        const char* status = "Initializing Pipeline...";
        if (progress < 0.25f) status = "Configuring Direct3D 11 Render Target...";
        else if (progress < 0.50f) status = "Rasterizing Font Atlas (3x Oversampled)...";
        else if (progress < 0.75f) status = "Synthesizing Procedural Audio Bank...";
        else if (progress < 0.92f) status = "Virtualizing SMBIOS Hardware Tables...";
        else status = "Solar Framework Initialized.";

        ImVec2 statusSize = ImGui::CalcTextSize(status);
        draw->AddText(ImVec2(center.x - statusSize.x * 0.5f, cardMin.y + 162.0f),
                      pal.TextSecondary.WithAlpha(m_alpha).ToU32(), status);

        float barW = 320.0f;
        float barH = 5.0f;
        ImVec2 barMin(center.x - barW * 0.5f, cardMin.y + 195.0f);
        ImVec2 barMax(barMin.x + barW, barMin.y + barH);

        draw->AddRectFilled(barMin, barMax, pal.Background.WithAlpha(m_alpha).ToU32(), barH * 0.5f);

        float fillW = barW * progress;
        ImVec2 fillMax(barMin.x + fillW, barMax.y);
        draw->AddRectFilled(barMin, fillMax, pal.Accent.WithAlpha(m_alpha).ToU32(), barH * 0.5f);

        if (progress > 0.05f && progress < 0.98f) {
            Render::GlowEngine::DrawGlowCircle(draw, ImVec2(fillMax.x, barMin.y + barH * 0.5f), 4.0f, pal.Accent, 6.0f, m_alpha);
        }

        return true;
    }

} // namespace Solar::UI
