#include "solar/anim/animation_manager.hpp"

namespace Solar::Anim {

    AnimationManager& AnimationManager::Get() {
        static AnimationManager instance;
        return instance;
    }

    f32 AnimationManager::Transition(ImGuiID id, bool active, f32 speed) {
        f32 dt = ImGui::GetIO().DeltaTime;
        auto& state = m_transitions[id];
        state.Update(dt, active ? 1.0f : 0.0f, speed);
        return state.Get();
    }

    f32 AnimationManager::SpringTransition(ImGuiID id, bool active, f32 stiffness, f32 damping) {
        f32 dt = ImGui::GetIO().DeltaTime;
        auto it = m_springs.find(id);
        if (it == m_springs.end()) {
            m_springs[id] = Spring(active ? 1.0f : 0.0f);
        }
        auto& spring = m_springs[id];
        spring.stiffness = stiffness;
        spring.damping = damping;
        spring.SetTarget(active ? 1.0f : 0.0f);
        spring.Update(dt);
        return spring.value;
    }

    void AnimationManager::Reset() {
        m_transitions.clear();
        m_springs.clear();
    }

} // namespace Solar::Anim
