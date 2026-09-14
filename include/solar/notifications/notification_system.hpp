#pragma once
#include "toast.hpp"
#include <vector>

namespace Solar {

    class NotificationSystem {
    public:
        static NotificationSystem& Get();

        void Push(ToastType type, const std::string& title, const std::string& message, f32 duration = 3.5f);
        void Render();

    private:
        NotificationSystem() = default;
        std::vector<Toast> m_toasts;
    };

    namespace Notify {
        inline void Success(const std::string& title, const std::string& msg) {
            NotificationSystem::Get().Push(ToastType::Success, title, msg);
        }
        inline void Warning(const std::string& title, const std::string& msg) {
            NotificationSystem::Get().Push(ToastType::Warning, title, msg);
        }
        inline void Error(const std::string& title, const std::string& msg) {
            NotificationSystem::Get().Push(ToastType::Error, title, msg);
        }
        inline void Info(const std::string& title, const std::string& msg) {
            NotificationSystem::Get().Push(ToastType::Info, title, msg);
        }
    }

} // namespace Solar
