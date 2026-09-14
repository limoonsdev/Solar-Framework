#include "solar/anim/smooth_damp.hpp"
#include <cmath>
#include <algorithm>

namespace Solar::Anim {

    f32 SmoothDamp::Calculate(f32 current, f32 target, f32& currentVelocity,
                             f32 smoothTime, f32 maxSpeed, f32 dt) {
        if (dt <= 0.0f) return current;
        smoothTime = (std::max)(0.0001f, smoothTime);

        f32 omega = 2.0f / smoothTime;
        f32 x = omega * dt;
        f32 exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

        f32 change = current - target;
        f32 originalTo = target;

        // Clamp maximum speed
        f32 maxChange = maxSpeed * smoothTime;
        change = (std::clamp)(change, -maxChange, maxChange);
        target = current - change;

        f32 temp = (currentVelocity + omega * change) * dt;
        currentVelocity = (currentVelocity - omega * temp) * exp;
        f32 output = target + (change + temp) * exp;

        // Prevent overshooting past target
        if ((originalTo - current > 0.0f) == (output > originalTo)) {
            output = originalTo;
            currentVelocity = (output - originalTo) / dt;
        }

        return output;
    }

    ImVec2 SmoothDamp::Calculate2D(const ImVec2& current, const ImVec2& target, ImVec2& currentVelocity,
                                   f32 smoothTime, f32 maxSpeed, f32 dt) {
        return ImVec2(
            Calculate(current.x, target.x, currentVelocity.x, smoothTime, maxSpeed, dt),
            Calculate(current.y, target.y, currentVelocity.y, smoothTime, maxSpeed, dt)
        );
    }

} // namespace Solar::Anim
