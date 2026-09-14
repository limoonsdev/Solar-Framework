#include "solar/anim/timeline.hpp"
#include <algorithm>
#include <cmath>

namespace Solar::Anim {

    Timeline::Timeline(bool loop) : m_isLooping(loop) {}

    Timeline& Timeline::AddKeyframe(f32 timeSeconds, f32 value, EasingType easing) {
        m_keyframes.push_back({ timeSeconds, value, easing });
        std::sort(m_keyframes.begin(), m_keyframes.end(), [](const Keyframe& a, const Keyframe& b) {
            return a.time < b.time;
        });
        if (!m_keyframes.empty()) {
            m_duration = m_keyframes.back().time;
        }
        return *this;
    }

    void Timeline::Update(f32 dt) {
        if (!m_isPlaying || m_keyframes.empty()) return;

        m_currentTime += dt;
        if (m_currentTime >= m_duration) {
            if (m_isLooping && m_duration > 0.0f) {
                m_currentTime = std::fmod(m_currentTime, m_duration);
            } else {
                m_currentTime = m_duration;
                m_finished = true;
            }
        }

        if (m_keyframes.size() == 1) {
            m_currentValue = m_keyframes[0].value;
            return;
        }

        // Locate active span
        size_t idx = 0;
        while (idx + 1 < m_keyframes.size() && m_keyframes[idx + 1].time < m_currentTime) {
            idx++;
        }

        if (idx + 1 >= m_keyframes.size()) {
            m_currentValue = m_keyframes.back().value;
            return;
        }

        const auto& k0 = m_keyframes[idx];
        const auto& k1 = m_keyframes[idx + 1];

        f32 segDuration = k1.time - k0.time;
        if (segDuration <= 0.0001f) {
            m_currentValue = k1.value;
            return;
        }

        f32 t = (m_currentTime - k0.time) / segDuration;
        t = (std::clamp)(t, 0.0f, 1.0f);
        f32 easedT = ApplyEasing(t, k1.easing);
        m_currentValue = k0.value + (k1.value - k0.value) * easedT;
    }

    void Timeline::Play() {
        m_isPlaying = true;
    }

    void Timeline::Pause() {
        m_isPlaying = false;
    }

    void Timeline::Reset() {
        m_currentTime = 0.0f;
        m_finished = false;
        if (!m_keyframes.empty()) {
            m_currentValue = m_keyframes[0].value;
        }
    }

} // namespace Solar::Anim
