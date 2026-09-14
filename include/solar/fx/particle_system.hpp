#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <vector>

namespace Solar::FX {

    enum class ParticleEffectMode {
        Snow = 0,        // Soft falling snowflakes with gentle wind drift
        Embers,          // Rising amber glowing embers
        Constellation,   // Interconnected starry grid nodes
        None
    };

    struct Particle {
        ImVec2 pos;
        ImVec2 vel;
        f32 radius = 1.8f;
        f32 alpha = 0.5f;
        f32 life = 1.0f;
        f32 maxLife = 5.0f;
        f32 swayOffset = 0.0f;
        f32 swaySpeed = 1.0f;
    };

    class ParticleSystem {
    public:
        static ParticleSystem& Get();

        void Initialize(i32 count = 55);
        void SetMode(ParticleEffectMode mode) { m_mode = mode; }
        ParticleEffectMode GetMode() const { return m_mode; }

        void UpdateAndRender(ImDrawList* draw, const ImVec2& min, const ImVec2& max, Color accentColor);

    private:
        ParticleSystem() = default;
        std::vector<Particle> m_particles;
        ParticleEffectMode m_mode = ParticleEffectMode::Embers;
        bool m_initialized = false;
        f32 m_globalTime = 0.0f;
    };

} // namespace Solar::FX
