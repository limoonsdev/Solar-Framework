#include "solar/core/timer.hpp"

namespace Solar {

    Timer::Timer() {
        Reset();
    }

    void Timer::Reset() {
        m_startTime = std::chrono::high_resolution_clock::now();
    }

    f32 Timer::ElapsedSeconds() const {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<f32> diff = now - m_startTime;
        return diff.count();
    }

    f64 Timer::ElapsedMilliseconds() const {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<f64, std::milli> diff = now - m_startTime;
        return diff.count();
    }

    FrameTimeTracker& FrameTimeTracker::Get() {
        static FrameTimeTracker instance;
        return instance;
    }

    FrameTimeTracker::FrameTimeTracker() {
        m_lastFrame = std::chrono::high_resolution_clock::now();
    }

    void FrameTimeTracker::Update() {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<f32> diff = now - m_lastFrame;
        m_lastFrame = now;
        m_deltaTime = diff.count();
        if (m_deltaTime <= 0.0f || m_deltaTime > 0.5f) m_deltaTime = 0.016f;

        m_frameAccumulator += m_deltaTime;
        m_frameCount++;
        if (m_frameAccumulator >= 0.5f) {
            m_fps = static_cast<f32>(m_frameCount) / m_frameAccumulator;
            m_frameAccumulator = 0.0f;
            m_frameCount = 0;
        }
    }

} // namespace Solar
