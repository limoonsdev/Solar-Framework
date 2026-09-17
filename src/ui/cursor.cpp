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

        // Ambient Luminous Glow Halo
        float glowRad = (12.0f + 4.0f * m_hoverAnim - 2.0f * m_clickAnim);
        draw->AddCircleFilled(mousePos, glowRad, pal.Accent.WithAlpha(0.12f).ToU32(), 24);
        draw->AddCircleFilled(mousePos, glowRad * 0.65f, pal.Accent.WithAlpha(0.22f).ToU32(), 20);

        // 6. Style-Specific Cursor Rendering
        switch (m_style) {
            case CursorStyle::CrosshairDot: {
                // Tactical 4-Axis Crosshair
                float armInner = 3.5f + m_hoverAnim * 2.0f;
                float armOuter = 8.5f + m_hoverAnim * 3.0f - m_clickAnim * 1.5f;
                u32 lineCol = pal.Accent.ToU32();
                u32 outlineCol = IM_COL32(0, 0, 0, 180);

                // Shadows/Outlines
                draw->AddLine(ImVec2(mousePos.x, mousePos.y - armOuter), ImVec2(mousePos.x, mousePos.y - armInner), outlineCol, 2.5f);
                draw->AddLine(ImVec2(mousePos.x, mousePos.y + armInner), ImVec2(mousePos.x, mousePos.y + armOuter), outlineCol, 2.5f);
                draw->AddLine(ImVec2(mousePos.x - armOuter, mousePos.y), ImVec2(mousePos.x - armInner, mousePos.y), outlineCol, 2.5f);
                draw->AddLine(ImVec2(mousePos.x + armInner, mousePos.y), ImVec2(mousePos.x + armOuter, mousePos.y), outlineCol, 2.5f);

                // Core lines
                draw->AddLine(ImVec2(mousePos.x, mousePos.y - armOuter), ImVec2(mousePos.x, mousePos.y - armInner), lineCol, 1.4f);
                draw->AddLine(ImVec2(mousePos.x, mousePos.y + armInner), ImVec2(mousePos.x, mousePos.y + armOuter), lineCol, 1.4f);
                draw->AddLine(ImVec2(mousePos.x - armOuter, mousePos.y), ImVec2(mousePos.x - armInner, mousePos.y), lineCol, 1.4f);
                draw->AddLine(ImVec2(mousePos.x + armInner, mousePos.y), ImVec2(mousePos.x + armOuter, mousePos.y), lineCol, 1.4f);

                // Center laser point
                draw->AddCircleFilled(mousePos, 1.5f, IM_COL32(255, 255, 255, 255), 8);
                break;
            }

            case CursorStyle::CyberDot: {
                // Minimalist Glowing Neon Dot
                float dotRad = 3.2f - m_clickAnim * 0.8f + m_hoverAnim * 1.2f;
                draw->AddCircleFilled(mousePos, dotRad + 1.0f, IM_COL32(0, 0, 0, 160), 16);
                draw->AddCircleFilled(mousePos, dotRad, pal.Accent.ToU32(), 16);
                draw->AddCircleFilled(mousePos, dotRad * 0.45f, IM_COL32(255, 255, 255, 240), 12);
                break;
            }

            case CursorStyle::PrecisionTriangle: {
                // Precision Inverted Chevron Pointer
                float s = 1.0f - m_clickAnim * 0.15f;
                ImVec2 pTip = mousePos;
                ImVec2 pL(mousePos.x - 7.0f * s, mousePos.y + 11.0f * s);
                ImVec2 pR(mousePos.x + 7.0f * s, mousePos.y + 11.0f * s);
                ImVec2 pIn(mousePos.x, mousePos.y + 8.0f * s);

                draw->AddTriangleFilled(pTip, pL, pIn, pal.Accent.WithAlpha(0.90f).ToU32());
                draw->AddTriangleFilled(pTip, pIn, pR, IM_COL32(20, 24, 34, 220));

                draw->AddLine(pTip, pL, IM_COL32(255, 255, 255, 200), 1.0f);
                draw->AddLine(pL, pIn, IM_COL32(255, 255, 255, 140), 1.0f);
                draw->AddLine(pIn, pR, IM_COL32(255, 255, 255, 140), 1.0f);
                draw->AddLine(pR, pTip, IM_COL32(255, 255, 255, 200), 1.0f);
                draw->AddCircleFilled(pTip, 1.4f, IM_COL32(255, 255, 255, 255), 8);
                break;
            }

            case CursorStyle::MinimalRing: {
                // Smooth Glowing Ring
                float ringR = 5.0f + m_hoverAnim * 2.0f - m_clickAnim * 1.5f;
                draw->AddCircle(mousePos, ringR + 1.0f, IM_COL32(0, 0, 0, 160), 24, 2.0f);
                draw->AddCircle(mousePos, ringR, pal.Accent.ToU32(), 24, 1.4f);
                draw->AddCircleFilled(mousePos, 1.2f, IM_COL32(255, 255, 255, 240), 8);
                break;
            }

            case CursorStyle::CyberArrow:
            default: {
                // Cyber Pointer Arrow (Morphs into reticle on hover)
                float arrowAlpha = 1.0f - m_hoverAnim;
                if (arrowAlpha > 0.01f) {
                    float s = 1.0f - m_clickAnim * 0.12f;
                    ImVec2 tip = mousePos;
                    ImVec2 left(mousePos.x + 0.5f, mousePos.y + 16.0f * s);
                    ImVec2 right(mousePos.x + 12.0f * s, mousePos.y + 11.5f * s);
                    ImVec2 notch(mousePos.x + 3.8f * s, mousePos.y + 10.0f * s);

                    u32 fillFacetL = pal.Accent.WithAlpha(arrowAlpha * 0.95f).ToU32();
                    u32 fillFacetR = IM_COL32(24, 28, 40, static_cast<int>(arrowAlpha * 230.0f));
                    u32 borderCol = IM_COL32(255, 255, 255, static_cast<int>(arrowAlpha * 200.0f));

                    // Shadow
                    ImVec2 shOff(1.0f, 1.5f);
                    draw->AddTriangleFilled(ImVec2(tip.x + shOff.x, tip.y + shOff.y),
                                            ImVec2(left.x + shOff.x, left.y + shOff.y),
                                            ImVec2(notch.x + shOff.x, notch.y + shOff.y), IM_COL32(0, 0, 0, 100));
                    draw->AddTriangleFilled(ImVec2(tip.x + shOff.x, tip.y + shOff.y),
                                            ImVec2(notch.x + shOff.x, notch.y + shOff.y),
                                            ImVec2(right.x + shOff.x, right.y + shOff.y), IM_COL32(0, 0, 0, 100));

                    // Dual facet fill
                    draw->AddTriangleFilled(tip, left, notch, fillFacetL);
                    draw->AddTriangleFilled(tip, notch, right, fillFacetR);

                    // Specular perimeter border
                    draw->AddLine(tip, left, borderCol, 1.2f);
                    draw->AddLine(left, notch, borderCol, 1.0f);
                    draw->AddLine(notch, right, borderCol, 1.0f);
                    draw->AddLine(right, tip, borderCol, 1.2f);

                    // Razor-sharp tip dot
                    draw->AddCircleFilled(tip, 1.4f, IM_COL32(255, 255, 255, static_cast<int>(arrowAlpha * 255.0f)), 8);
                }

                if (m_hoverAnim > 0.01f) {
                    float ringRadius = 4.2f + 1.2f * m_hoverAnim - 0.8f * m_clickAnim;
                    u32 ringCol = pal.Accent.WithAlpha(m_hoverAnim * 0.90f).ToU32();
                    draw->AddCircle(mousePos, ringRadius, ringCol, 20, 1.2f);

                    // 4-Axis Precision Reticle Ticks
                    float tickInner = ringRadius + 2.0f;
                    float tickOuter = ringRadius + 4.8f * m_hoverAnim;
                    u32 tickCol = pal.Accent.WithAlpha(m_hoverAnim * 0.95f).ToU32();

                    draw->AddLine(ImVec2(mousePos.x, mousePos.y - tickOuter), ImVec2(mousePos.x, mousePos.y - tickInner), tickCol, 1.2f);
                    draw->AddLine(ImVec2(mousePos.x, mousePos.y + tickInner), ImVec2(mousePos.x, mousePos.y + tickOuter), tickCol, 1.2f);
                    draw->AddLine(ImVec2(mousePos.x - tickOuter, mousePos.y), ImVec2(mousePos.x - tickInner, mousePos.y), tickCol, 1.2f);
                    draw->AddLine(ImVec2(mousePos.x + tickInner, mousePos.y), ImVec2(mousePos.x + tickOuter, mousePos.y), tickCol, 1.2f);

                    // Central laser star core
                    draw->AddCircleFilled(mousePos, 1.6f, IM_COL32(255, 255, 255, static_cast<int>(m_hoverAnim * 255.0f)), 12);
                }
                break;
            }
        }
    }

} // namespace Solar::UI
