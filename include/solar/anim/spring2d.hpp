#pragma once
#include "solar/core/types.hpp"
#include "solar/anim/spring.hpp"
#include <imgui.h>

namespace Solar::Anim {

    class Spring2D {
    public:
        Spring x;
        Spring y;

        Spring2D(f32 initX = 0.0f, f32 initY = 0.0f) : x(initX), y(initY) {}
        Spring2D(const ImVec2& init) : x(init.x), y(init.y) {}

        void SetParameters(f32 stiffness, f32 damping, f32 mass = 1.0f) {
            x.stiffness = stiffness;
            x.damping = damping;
            x.mass = mass;
            y.stiffness = stiffness;
            y.damping = damping;
            y.mass = mass;
        }

        void Update(f32 dt) {
            x.Update(dt);
            y.Update(dt);
        }

        void SetTarget(f32 targetX, f32 targetY) {
            x.SetTarget(targetX);
            y.SetTarget(targetY);
        }

        void SetTarget(const ImVec2& target) {
            x.SetTarget(target.x);
            y.SetTarget(target.y);
        }

        void SnapTo(f32 valX, f32 valY) {
            x.SnapTo(valX);
            y.SnapTo(valY);
        }

        void SnapTo(const ImVec2& val) {
            x.SnapTo(val.x);
            y.SnapTo(val.y);
        }

        ImVec2 GetValue() const {
            return ImVec2(x.value, y.value);
        }

        ImVec2 GetVelocity() const {
            return ImVec2(x.velocity, y.velocity);
        }

        bool IsSettled(f32 tolerance = 0.001f) const {
            return x.IsSettled(tolerance) && y.IsSettled(tolerance);
        }
    };

} // namespace Solar::Anim
