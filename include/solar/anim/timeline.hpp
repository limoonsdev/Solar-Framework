#pragma once
#include "solar/core/types.hpp"
#include "solar/anim/easing.hpp"
#include <vector>
#include <functional>

namespace Solar::Anim {

    struct Keyframe {
        f32 time;
        f32 value;
        EasingType easing = EasingType::InOutCubic;
    };

    /**
     * @brief Motion Keyframe Timeline & Staggered Animation Sequencer
     */
    class Timeline {
    public:
        Timeline(bool loop = false);

        Timeline& AddKeyframe(f32 timeSeconds, f32 value, EasingType easing = EasingType::InOutCubic);
        void Update(f32 dt);
        void Play();
        void Pause();
        void Reset();

        f32 GetValue() const { return m_currentValue; }
        f32 GetTime() const { return m_currentTime; }
        f32 GetTotalDuration() const { return m_duration; }
        bool IsFinished() const { return m_finished; }

    private:
        std::vector<Keyframe> m_keyframes;
        f32 m_currentTime = 0.0f;
        f32 m_currentValue = 0.0f;
        f32 m_duration = 0.0f;
        bool m_isPlaying = true;
        bool m_isLooping = false;
        bool m_finished = false;
    };

} // namespace Solar::Anim
