#pragma once
#include "types.hpp"
#include <chrono>

namespace Solar {

    class Timer {
    public:
        Timer();
        void Reset();
        f32 ElapsedSeconds() const;
        f64 ElapsedMilliseconds() const;

    private:
        std::chrono::high_resolution_clock::time_point m_startTime;
    };

    class FrameTimeTracker {
    public:
        static FrameTimeTracker& Get();
        void Update();
        f32 GetDeltaTime() const { return m_deltaTime; }
        f32 GetFPS() const { return m_fps; }

    private:
        FrameTimeTracker();
        std::chrono::high_resolution_clock::time_point m_lastFrame;
        f32 m_deltaTime = 0.016f;
        f32 m_fps = 60.0f;
        f32 m_frameAccumulator = 0.0f;
        i32 m_frameCount = 0;
    };

} // namespace Solar
