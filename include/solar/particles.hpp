#pragma once

#include <imgui.h>
#include <vector>

namespace Solar {

    struct Particle {
        ImVec2 pos;
        ImVec2 velocity;
        float  radius;
        float  alpha;
        float  pulseSpeed;
        float  pulsePhase;
    };

    class ParticleSystem {
    public:
        static ParticleSystem& Get();

        void Initialize(int count = 50);
        void UpdateAndRender(ImDrawList* drawList, const ImVec2& minBounds, const ImVec2& maxBounds, const ImVec4& accentColor);
        void SetCount(int count);

    private:
        ParticleSystem();
        std::vector<Particle> m_particles;
        ImVec2 m_lastBoundsSize = ImVec2(0, 0);
        bool m_initialized = false;
    };

} // namespace Solar
