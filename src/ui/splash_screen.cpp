#include "solar/ui/splash_screen.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/core/math.hpp"
#include "solar/render/glow_engine.hpp"
#include <imgui.h>
#include <cmath>
#include <cstdio>

namespace Solar::UI {

    static const StageTelemetry s_stages[] = {
        { SplashStage::CoreInit,          "Kernel & Memory Pools",         "Allocating arena allocators & SIMD vector pools...", 0.18f },
        { SplashStage::DirectXInit,       "Direct3D 11 Pipeline",          "Verifying SwapChain buffers & blend state cache...",  0.40f },
        { SplashStage::FontRasterization, "FreeType Glyph Rasterizer",     "Pre-rendering font atlas (3x oversampling, 144 DPI)...", 0.65f },
        { SplashStage::AudioSynthesis,    "Procedural Wave Synthesizer",   "Generating audio buffers & sinusoidal waveforms...",  0.84f },
        { SplashStage::ComputeParticles,  "Particle Simulation Compute",   "Warming up GPU point emitter particle buffers...",   0.96f },
        { SplashStage::Ready,             "Pipeline Initialized",          "Solar Framework ready. Transitioning to viewport...", 1.00f }
    };

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
        m_currentStage = SplashStage::CoreInit;
    }

    bool SplashScreen::Render() {
        if (!m_active) return false;

        float dt = ImGui::GetIO().DeltaTime;
        m_elapsed += dt;

        float progress = Math::Clamp(m_elapsed / m_duration, 0.0f, 1.0f);

        // Update active stage
        for (const auto& stageInfo : s_stages) {
            if (progress <= stageInfo.progressThreshold) {
                m_currentStage = stageInfo.stage;
                break;
            }
        }

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

        // Fullscreen overlay backdrop
        draw->AddRectFilled(screenMin, screenMax, Color(0.03f, 0.035f, 0.048f, 0.96f * m_alpha).ToU32());

        float cardW = 460.0f;
        float cardH = 280.0f;
        ImVec2 cardMin(center.x - cardW * 0.5f, center.y - cardH * 0.5f);
        ImVec2 cardMax(center.x + cardW * 0.5f, center.y + cardH * 0.5f);

        // Card container
        draw->AddRectFilled(cardMin, cardMax, pal.Card.WithAlpha(0.96f * m_alpha).ToU32(), 14.0f);
        draw->AddRect(cardMin, cardMax, pal.Border.WithAlpha(m_alpha).ToU32(), 14.0f, 0, 1.5f);

        Render::GlowEngine::DrawGlowRect(draw, cardMin, cardMax, pal.Accent, 24.0f, 14.0f, 0.65f * m_alpha);

        // Rotating ring spinner
        float spinRadius = 22.0f;
        ImVec2 spinnerCenter(center.x, cardMin.y + 54.0f);
        float angle = m_elapsed * 5.5f;

        draw->AddCircle(spinnerCenter, spinRadius, pal.Border.WithAlpha(0.35f * m_alpha).ToU32(), 32, 2.0f);
        draw->PathArcTo(spinnerCenter, spinRadius, angle, angle + 2.2f, 24);
        draw->PathStroke(pal.Accent.WithAlpha(m_alpha).ToU32(), 0, 3.0f);
        draw->AddCircleFilled(spinnerCenter, 6.0f, pal.Accent.WithAlpha(m_alpha).ToU32(), 16);

        // Framework Branding
        const char* brand = "SOLAR FRAMEWORK";
        ImVec2 brandSize = ImGui::CalcTextSize(brand);
        draw->AddText(ImVec2(center.x - brandSize.x * 0.5f, cardMin.y + 94.0f),
                      pal.TextPrimary.WithAlpha(m_alpha).ToU32(), brand);

        // Telemetry readout
        char telemetryBuf[128];
        snprintf(telemetryBuf, sizeof(telemetryBuf), "STAGE %d/6  •  %.0f ms  •  %.0f%%",
                 static_cast<int>(m_currentStage) + 1, m_elapsed * 1000.0f, progress * 100.0f);
        ImVec2 telSize = ImGui::CalcTextSize(telemetryBuf);
        draw->AddText(ImVec2(center.x - telSize.x * 0.5f, cardMin.y + 116.0f),
                      pal.Accent.WithAlpha(m_alpha).ToU32(), telemetryBuf);

        // Current Stage Details
        const auto& currentInfo = s_stages[static_cast<int>(m_currentStage)];
        ImVec2 stageNameSz = ImGui::CalcTextSize(currentInfo.name);
        draw->AddText(ImVec2(center.x - stageNameSz.x * 0.5f, cardMin.y + 144.0f),
                      pal.TextPrimary.WithAlpha(m_alpha).ToU32(), currentInfo.name);

        ImVec2 detailSz = ImGui::CalcTextSize(currentInfo.detail);
        draw->AddText(ImVec2(center.x - detailSz.x * 0.5f, cardMin.y + 164.0f),
                      pal.TextSecondary.WithAlpha(0.85f * m_alpha).ToU32(), currentInfo.detail);

        // Progress bar
        float barW = 380.0f;
        float barH = 4.0f;
        ImVec2 barMin(center.x - barW * 0.5f, cardMin.y + 196.0f);
        ImVec2 barMax(barMin.x + barW, barMin.y + barH);

        draw->AddRectFilled(barMin, barMax, pal.Background.WithAlpha(m_alpha).ToU32(), barH * 0.5f);

        float fillW = barW * progress;
        ImVec2 fillMax(barMin.x + fillW, barMin.y + barH);
        draw->AddRectFilled(barMin, fillMax, pal.Accent.WithAlpha(m_alpha).ToU32(), barH * 0.5f);

        if (progress > 0.04f && progress < 0.98f) {
            Render::GlowEngine::DrawGlowCircle(draw, ImVec2(fillMax.x, barMin.y + barH * 0.5f), 4.0f, pal.Accent, 6.0f, m_alpha);
        }

        // Hardware Capability Badges on Bottom
        const char* badges[] = { "D3D11 FL 11_1", "AVX2 SIMD", "RK4 SOLVER", "x86_64" };
        float totalBadgesW = 0.0f;
        float badgeWidths[4];
        for (int i = 0; i < 4; ++i) {
            badgeWidths[i] = ImGui::CalcTextSize(badges[i]).x + 16.0f;
            totalBadgesW += badgeWidths[i];
        }
        float spacing = 8.0f;
        totalBadgesW += spacing * 3.0f;

        float badgeStartX = center.x - totalBadgesW * 0.5f;
        float badgeY = cardMin.y + 224.0f;
        float badgeH = 22.0f;

        for (int i = 0; i < 4; ++i) {
            ImVec2 bMin(badgeStartX, badgeY);
            ImVec2 bMax(badgeStartX + badgeWidths[i], badgeY + badgeH);

            draw->AddRectFilled(bMin, bMax, pal.Card.WithAlpha(0.8f * m_alpha).ToU32(), 4.0f);
            draw->AddRect(bMin, bMax, pal.Border.WithAlpha(0.6f * m_alpha).ToU32(), 4.0f, 0, 1.0f);

            ImVec2 txtSz = ImGui::CalcTextSize(badges[i]);
            draw->AddText(ImVec2(bMin.x + (badgeWidths[i] - txtSz.x) * 0.5f, bMin.y + (badgeH - txtSz.y) * 0.5f),
                          pal.TextSecondary.WithAlpha(0.9f * m_alpha).ToU32(), badges[i]);

            badgeStartX += badgeWidths[i] + spacing;
        }

        return true;
    }

} // namespace Solar::UI
