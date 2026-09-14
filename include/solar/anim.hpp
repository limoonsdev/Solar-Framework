#pragma once

#include <imgui.h>
#include <unordered_map>
#include <cmath>

namespace Solar {

    enum class EasingType {
        Linear,
        InQuad,
        OutQuad,
        InOutQuad,
        OutCubic,
        OutBack
    };

    class AnimationManager {
    public:
        static AnimationManager& Get();

        // Main scalar interpolation
        float Interpolate(ImGuiID id, float target, float speed = 12.0f, EasingType easing = EasingType::OutQuad);

        // Vector4 / Color interpolation
        ImVec4 InterpolateColor(ImGuiID id, const ImVec4& target, float speed = 12.0f);

        // Reset or query
        void Clear(ImGuiID id);
        void ResetAll();
        float GetValue(ImGuiID id, float defaultVal = 0.0f) const;

        // Pure easing helper functions (t in range [0, 1])
        static float Ease(float t, EasingType type);
        static float EaseLinear(float t);
        static float EaseInQuad(float t);
        static float EaseOutQuad(float t);
        static float EaseInOutQuad(float t);
        static float EaseOutCubic(float t);
        static float EaseOutBack(float t, float overshoot = 1.4f);

    private:
        AnimationManager() = default;
        std::unordered_map<ImGuiID, float> m_values;
        std::unordered_map<ImGuiID, ImVec4> m_colorValues;
    };

    // Global helper shortcuts
    inline float AnimLerp(ImGuiID id, float target, float speed = 12.0f, EasingType easing = EasingType::OutQuad) {
        return AnimationManager::Get().Interpolate(id, target, speed, easing);
    }

    inline ImVec4 AnimColorLerp(ImGuiID id, const ImVec4& target, float speed = 12.0f) {
        return AnimationManager::Get().InterpolateColor(id, target, speed);
    }

} // namespace Solar
