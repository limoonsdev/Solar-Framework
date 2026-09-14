#pragma once

#include <imgui.h>
#include <string>
#include <vector>

namespace Solar {

    enum class NotificationType {
        Success,
        Info,
        Warning,
        Error
    };

    struct Notification {
        std::string title;
        std::string message;
        NotificationType type = NotificationType::Info;
        float duration = 4.0f; // seconds
        float lifetime = 0.0f;
        float animAlpha = 0.0f;
        float animSlide = 0.0f;
        bool dismissed = false;
    };

    class NotificationSystem {
    public:
        static NotificationSystem& Get();

        void Post(const std::string& title, const std::string& message, NotificationType type = NotificationType::Info, float duration = 4.0f);
        void Success(const std::string& title, const std::string& message, float duration = 4.0f);
        void Info(const std::string& title, const std::string& message, float duration = 4.0f);
        void Warning(const std::string& title, const std::string& message, float duration = 4.0f);
        void Error(const std::string& title, const std::string& message, float duration = 4.0f);

        void Render();

    private:
        NotificationSystem() = default;
        std::vector<Notification> m_notifications;
    };

    namespace Notify {
        inline void Success(const std::string& title, const std::string& message, float duration = 4.0f) {
            NotificationSystem::Get().Success(title, message, duration);
        }
        inline void Info(const std::string& title, const std::string& message, float duration = 4.0f) {
            NotificationSystem::Get().Info(title, message, duration);
        }
        inline void Warning(const std::string& title, const std::string& message, float duration = 4.0f) {
            NotificationSystem::Get().Warning(title, message, duration);
        }
        inline void Error(const std::string& title, const std::string& message, float duration = 4.0f) {
            NotificationSystem::Get().Error(title, message, duration);
        }
    }

} // namespace Solar
