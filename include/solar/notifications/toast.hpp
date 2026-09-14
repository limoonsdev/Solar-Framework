#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <string>

namespace Solar {

    enum class ToastType {
        Success = 0,
        Warning,
        Error,
        Info
    };

    struct Toast {
        std::string title;
        std::string message;
        ToastType type = ToastType::Info;
        f32 duration = 3.5f;
        f32 timeElapsed = 0.0f;
        f32 animProgress = 0.0f;

        bool IsExpired() const { return timeElapsed >= duration; }
    };

} // namespace Solar
