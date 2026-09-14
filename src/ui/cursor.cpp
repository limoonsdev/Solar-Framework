#include "solar/ui/cursor.hpp"
#include "solar/theme/theme_manager.hpp"
#include <algorithm>
#include <cmath>

namespace Solar::UI {

    CustomCursor& CustomCursor::Get() {
        static CustomCursor instance;
        return instance;
    }

    void CustomCursor::Render() {
        if (!m_enabled) return;

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 mousePos = io.MousePos;
        if (mousePos.x < -20.0f || mousePos.y < -20.0f ||
            mousePos.x > io.DisplaySize.x + 20.0f || mousePos.y > io.DisplaySize.y + 20.0f) {
            return;
        }

        // Hide OS cursor inside framework canvas
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        float dt = io.DeltaTime;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = ImGui::GetForegroundDrawList();

        // 1. Click Ripple Physics
        if (m_enableRipple && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            m_clickRipple = 1.0f;
        }
        if (m_clickRipple > 0.0f) {
            m_clickRipple -= dt * 3.5f;
            if (m_clickRipple < 0.0f) m_clickRipple = 0.0f;
        }

        // 2. Click Micro-Contraction
        bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        m_clickAnim += (mouseDown ? 1.0f : -1.0f) * dt * 14.0f;
        m_clickAnim = std::clamp(m_clickAnim, 0.0f, 1.0f);

        // 3. Hover Reticle Animation
        bool isHovering = ImGui::IsAnyItemHovered();
        m_hoverAnim += (isHovering ? 1.0f : -1.0f) * dt * 12.0f;
        m_hoverAnim = std::clamp(m_hoverAnim, 0.0f, 1.0f);

        // 4. Fluid Ghost Trailing
        if (m_enableTrailing) {
            if (m_smoothPos.x < -50.0f) m_smoothPos = mousePos;
            m_smoothPos.x += (mousePos.x - m_smoothPos.x) * (std::min)(1.0f, dt * 26.0f);
            m_smoothPos.y += (mousePos.y - m_smoothPos.y) * (std::min)(1.0f, dt * 26.0f);

            float trailDist = std::sqrt((mousePos.x - m_smoothPos.x) * (mousePos.x - m_smoothPos.x) +
                                        (mousePos.y - m_smoothPos.y) * (mousePos.y - m_smoothPos.y));
            if (trailDist > 1.2f) {
                draw->AddLine(m_smoothPos, mousePos, pal.Accent.WithAlpha(0.22f).ToU32(), 1.0f);
                draw->AddCircleFilled(m_smoothPos, 1.8f, pal.Accent.WithAlpha(0.35f).ToU32(), 12);
            }
        }

        // 5. Click Ripple Expanding Wave
        if (m_clickRipple > 0.0f) {
            float rippleRadius = 4.0f + (1.0f - m_clickRipple) * 18.0f;
            draw->AddCircle(mousePos, rippleRadius, pal.Accent.WithAlpha(m_clickRipple * 0.70f).ToU32(), 28, 1.2f);
        }

        // 6. Ambient Luminous Glow Halo
        float glowRad = 13.0f + 3.0f * m_hoverAnim - 2.0f * m_clickAnim;
        draw->AddCircleFilled(mousePos, glowRad, pal.Accent.WithAlpha(0.14f).ToU32(), 24);
        draw->AddCircleFilled(mousePos, glowRad * 0.65f, pal.Accent.WithAlpha(0.24f).ToU32(), 20);
        draw->AddCircleFilled(mousePos, glowRad * 0.35f, pal.Accent.WithAlpha(0.40f).ToU32(), 16);

        // 7. Outer Accent Ring
        float ringRadius = 4.0f + 1.2f * m_hoverAnim - 0.7f * m_clickAnim;
        draw->AddCircle(mousePos, ringRadius, pal.Accent.ToU32(), 20, 1.2f);

        // 8. Precision Reticle Crosshair Ticks on Hover
        if (m_hoverAnim > 0.05f) {
            float tickInner = ringRadius + 2.0f;
            float tickOuter = ringRadius + 4.5f * m_hoverAnim;
            u32 tickCol = pal.Accent.WithAlpha(m_hoverAnim * 0.85f).ToU32();

            draw->AddLine(ImVec2(mousePos.x, mousePos.y - tickOuter), ImVec2(mousePos.x, mousePos.y - tickInner), tickCol, 1.0f);
            draw->AddLine(ImVec2(mousePos.x, mousePos.y + tickInner), ImVec2(mousePos.x, mousePos.y + tickOuter), tickCol, 1.0f);
            draw->AddLine(ImVec2(mousePos.x - tickOuter, mousePos.y), ImVec2(mousePos.x - tickInner, mousePos.y), tickCol, 1.0f);
            draw->AddLine(ImVec2(mousePos.x + tickInner, mousePos.y), ImVec2(mousePos.x + tickOuter, mousePos.y), tickCol, 1.0f);
        }

        // 9. Ultra-Sharp Core Star Dot
        draw->AddCircleFilled(mousePos, 1.8f, IM_COL32(255, 255, 255, 255), 16);
    }

} // namespace Solar::UI
