#include "solar/fx/particle_system.hpp"
#include "solar/core/math.hpp"
#include <random>
#include <cmath>

namespace Solar::FX {

    ParticleSystem& ParticleSystem::Get() {
        static ParticleSystem instance;
        return instance;
    }

    void ParticleSystem::Initialize(i32 count) {
        m_particles.resize(count);
        std::mt19937 rng(42);
        std::uniform_real_distribution<f32> distSpeed(-15.0f, 15.0f);
        std::uniform_real_distribution<f32> distRadius(1.2f, 2.6f);
        std::uniform_real_distribution<f32> distLife(3.0f, 7.0f);
        std::uniform_real_distribution<f32> distSway(0.0f, 6.28f);

        for (auto& p : m_particles) {
            p.pos = ImVec2(0, 0);
            p.vel = ImVec2(distSpeed(rng), distSpeed(rng));
            p.radius = distRadius(rng);
            p.maxLife = distLife(rng);
            p.life = p.maxLife * (distRadius(rng) / 2.6f);
            p.alpha = 0.35f;
            p.swayOffset = distSway(rng);
            p.swaySpeed = 1.0f + (distSpeed(rng) / 30.0f);
        }
        m_initialized = true;
    }

    void ParticleSystem::UpdateAndRender(ImDrawList* draw, const ImVec2& min, const ImVec2& max, Color accentColor) {
        if (m_mode == ParticleEffectMode::None) return;
        if (!m_initialized) Initialize();

        f32 dt = ImGui::GetIO().DeltaTime;
        m_globalTime += dt;
        f32 width = max.x - min.x;
        f32 height = max.y - min.y;
        if (width <= 0.0f || height <= 0.0f) return;

        ImVec2 mouse = ImGui::GetIO().MousePos;
        bool mouseInBounds = mouse.x >= min.x && mouse.x <= max.x && mouse.y >= min.y && mouse.y <= max.y;

        for (auto& p : m_particles) {
            if (p.pos.x < min.x - 10.0f || p.pos.x > max.x + 10.0f || p.pos.y < min.y - 10.0f || p.pos.y > max.y + 10.0f) {
                p.pos.x = min.x + std::fmod(std::abs(p.pos.x + 149.0f), width);
                p.pos.y = min.y + std::fmod(std::abs(p.pos.y + 263.0f), height);
                p.life = p.maxLife;
            }

            p.life -= dt;
            if (p.life <= 0.0f) {
                p.life = p.maxLife;
                p.pos.x = min.x + (static_cast<f32>(rand()) / RAND_MAX) * width;
                p.pos.y = (m_mode == ParticleEffectMode::Snow) ? min.y : ((m_mode == ParticleEffectMode::Embers) ? max.y : min.y + (static_cast<f32>(rand()) / RAND_MAX) * height);
            }

            if (m_mode == ParticleEffectMode::Snow) {
                f32 fallSpeed = 22.0f + p.radius * 12.0f;
                f32 sway = std::sin(m_globalTime * p.swaySpeed + p.swayOffset) * 16.0f;
                p.pos.y += fallSpeed * dt;
                p.pos.x += sway * dt;
            } else if (m_mode == ParticleEffectMode::Embers) {
                f32 riseSpeed = -(26.0f + p.radius * 14.0f);
                f32 drift = std::sin(m_globalTime * 2.0f + p.swayOffset) * 18.0f;
                p.pos.y += riseSpeed * dt;
                p.pos.x += drift * dt;
            } else if (m_mode == ParticleEffectMode::Constellation) {
                p.pos.x += p.vel.x * dt;
                p.pos.y += p.vel.y * dt;
            }

            if (mouseInBounds) {
                f32 dist = Math::Distance(p.pos, mouse);
                if (dist < 90.0f && dist > 1.0f) {
                    ImVec2 push((p.pos.x - mouse.x) / dist, (p.pos.y - mouse.y) / dist);
                    p.pos.x += push.x * 75.0f * dt;
                    p.pos.y += push.y * 75.0f * dt;
                }
            }

            f32 lifeRatio = p.life / p.maxLife;
            f32 currentAlpha = std::sin(lifeRatio * Math::PI);

            if (m_mode == ParticleEffectMode::Snow) {
                u32 snowCol = Color(0.88f, 0.95f, 1.0f, currentAlpha * 0.40f).ToU32();
                draw->AddCircleFilled(p.pos, p.radius, snowCol, 12);
                if (p.radius > 2.0f) {
                    draw->AddCircle(p.pos, p.radius + 1.2f, Color(1, 1, 1, currentAlpha * 0.15f).ToU32(), 12, 1.0f);
                }
            } else if (m_mode == ParticleEffectMode::Embers) {
                u32 emberCol = accentColor.WithAlpha(currentAlpha * 0.45f).ToU32();
                draw->AddCircleFilled(p.pos, p.radius, emberCol, 12);
                draw->AddCircleFilled(p.pos, p.radius * 0.5f, 0xFFFFFFFF, 12);
            } else if (m_mode == ParticleEffectMode::Constellation) {
                u32 nodeCol = accentColor.WithAlpha(currentAlpha * 0.45f).ToU32();
                draw->AddCircleFilled(p.pos, p.radius, nodeCol, 12);
            }
        }

        if (m_mode == ParticleEffectMode::Constellation) {
            const f32 connectDist = 75.0f;
            size_t n = m_particles.size();
            for (size_t i = 0; i < n; i++) {
                for (size_t j = i + 1; j < n; j++) {
                    f32 d = Math::Distance(m_particles[i].pos, m_particles[j].pos);
                    if (d < connectDist) {
                        f32 lineAlpha = (1.0f - (d / connectDist)) * 0.18f;
                        u32 lineCol = accentColor.WithAlpha(lineAlpha).ToU32();
                        draw->AddLine(m_particles[i].pos, m_particles[j].pos, lineCol, 1.0f);
                    }
                }
            }
        }
    }

} // namespace Solar::FX
