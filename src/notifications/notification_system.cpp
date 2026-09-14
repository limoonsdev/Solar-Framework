#include "solar/notifications/notification_system.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/core/math.hpp"
#include "solar/render/shadow_caster.hpp"
#include "solar/font_awesome.hpp"
#include <imgui.h>

namespace Solar {

    NotificationSystem& NotificationSystem::Get() {
        static NotificationSystem instance;
        return instance;
    }

    void NotificationSystem::Push(ToastType type, const std::string& title, const std::string& message, f32 duration) {
        Toast t;
        t.type = type;
        t.title = title;
        t.message = message;
        t.duration = duration;
        m_toasts.push_back(t);

        switch (type) {
        case ToastType::Success:
        case ToastType::Info:
            Audio::PlayNotification();
            break;
        case ToastType::Warning:
            Audio::PlayWarning();
            break;
        case ToastType::Error:
            Audio::PlayError();
            break;
        }
    }

    void NotificationSystem::Render() {
        if (m_toasts.empty()) return;

        ImGuiIO& io = ImGui::GetIO();
        f32 dt = io.DeltaTime;
        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        float toastW = 310.0f;
        float toastH = 64.0f;
        float spacing = 10.0f;
        float rightMargin = 20.0f;
        float bottomMargin = 20.0f;

        float currentY = io.DisplaySize.y - bottomMargin;

        for (auto it = m_toasts.begin(); it != m_toasts.end();) {
            it->timeElapsed += dt;

            // Slide in animation
            if (it->animProgress < 1.0f) {
                it->animProgress += dt * 8.0f;
                if (it->animProgress > 1.0f) it->animProgress = 1.0f;
            }

            // Slide out at end
            float fadeProgress = 1.0f;
            if (it->duration - it->timeElapsed < 0.35f) {
                fadeProgress = (it->duration - it->timeElapsed) / 0.35f;
                if (fadeProgress < 0.0f) fadeProgress = 0.0f;
            }

            if (it->IsExpired()) {
                it = m_toasts.erase(it);
                continue;
            }

            float animX = io.DisplaySize.x - rightMargin - (toastW * it->animProgress * fadeProgress);
            currentY -= (toastH + spacing);

            ImVec2 min(animX, currentY);
            ImVec2 max(animX + toastW, currentY + toastH);

            Color typeCol = pal.Info;
            const char* typeIcon = ICON_FA_CIRCLE_INFO;

            switch (it->type) {
            case ToastType::Success:
                typeCol = pal.Success;
                typeIcon = ICON_FA_CHECK;
                break;
            case ToastType::Warning:
                typeCol = pal.Warning;
                typeIcon = ICON_FA_TRIANGLE_EXCLAMATION;
                break;
            case ToastType::Error:
                typeCol = pal.Danger;
                typeIcon = ICON_FA_XMARK;
                break;
            case ToastType::Info:
                typeCol = pal.Info;
                typeIcon = ICON_FA_CIRCLE_INFO;
                break;
            }

            // Shadow
            Render::ShadowCaster::DrawShadow(draw, min, max, 16.0f, 6.0f, Color(0, 0, 0, 0.45f * fadeProgress));

            // Background
            draw->AddRectFilled(min, max, pal.Header.WithAlpha(0.96f * fadeProgress).ToU32(), 6.0f);
            draw->AddRect(min, max, pal.Border.WithAlpha(fadeProgress).ToU32(), 6.0f);

            // Left accent stripe
            draw->AddRectFilled(min, ImVec2(min.x + 4.0f, max.y), typeCol.WithAlpha(fadeProgress).ToU32(), 6.0f, ImDrawFlags_RoundCornersLeft);

            // Icon
            draw->AddText(ImVec2(min.x + 14.0f, min.y + 14.0f), typeCol.WithAlpha(fadeProgress).ToU32(), typeIcon);

            // Title & Message
            draw->AddText(ImVec2(min.x + 36.0f, min.y + 12.0f), pal.TextPrimary.WithAlpha(fadeProgress).ToU32(), it->title.c_str());
            draw->AddText(ImVec2(min.x + 36.0f, min.y + 30.0f), pal.TextSecondary.WithAlpha(fadeProgress).ToU32(), it->message.c_str());

            // Countdown progress bar at bottom
            float prog = 1.0f - (it->timeElapsed / it->duration);
            prog = Math::Clamp(prog, 0.0f, 1.0f);
            ImVec2 barMin(min.x + 4.0f, max.y - 2.5f);
            ImVec2 barMax(min.x + 4.0f + (toastW - 4.0f) * prog, max.y);
            draw->AddRectFilled(barMin, barMax, typeCol.WithAlpha(0.8f * fadeProgress).ToU32());

            ++it;
        }
    }

} // namespace Solar
