#pragma once
#include "solar/core/types.hpp"
#include "transitions.hpp"
#include "spring.hpp"
#include <unordered_map>

namespace Solar::Anim {

    class AnimationManager {
    public:
        static AnimationManager& Get();

        f32 Transition(ImGuiID id, bool active, f32 speed = 10.0f);
        f32 SpringTransition(ImGuiID id, bool active, f32 stiffness = 200.0f, f32 damping = 20.0f);
        void Reset();

    private:
        AnimationManager() = default;
        std::unordered_map<ImGuiID, TransitionState> m_transitions;
        std::unordered_map<ImGuiID, Spring> m_springs;
    };

} // namespace Solar::Anim
