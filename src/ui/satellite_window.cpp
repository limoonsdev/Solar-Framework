#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include "solar/ui/satellite_window.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/shadow_caster.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/font_awesome.hpp"
#include <cmath>
#include <algorithm>

namespace Solar::UI {

    static void DrawSatelliteConnector(ImDrawList* draw, const ImVec2& parentEdge, const ImVec2& satelliteEdge,
                                      Color color, float timeSec) {
        if (!draw) return;

        ImVec2 mid((parentEdge.x + satelliteEdge.x) * 0.5f, (parentEdge.y + satelliteEdge.y) * 0.5f);

        // Core line
        u32 beamCol = color.ToU32();
        u32 glowCol = color.WithAlpha(color.a * 0.28f).ToU32();

        // Multi-pass neon glow
        draw->AddLine(parentEdge, satelliteEdge, glowCol, 4.0f);
        draw->AddLine(parentEdge, satelliteEdge, beamCol, 1.5f);

        // Terminal joint nodes
        draw->AddCircleFilled(parentEdge, 3.5f, beamCol, 12);
        draw->AddCircle(parentEdge, 5.0f, glowCol, 12, 1.5f);

        draw->AddCircleFilled(satelliteEdge, 3.5f, beamCol, 12);
        draw->AddCircle(satelliteEdge, 5.0f, glowCol, 12, 1.5f);

        // Animated travelling energy pulse
        float phase = std::fmod(timeSec * 2.0f, 1.0f);
        ImVec2 pulsePos(parentEdge.x + (satelliteEdge.x - parentEdge.x) * phase,
                        parentEdge.y + (satelliteEdge.y - parentEdge.y) * phase);
        draw->AddCircleFilled(pulsePos, 2.5f, IM_COL32(255, 255, 255, 220), 8);
    }

    bool SatelliteWindow::Begin(const char* name, const char* title, const ImVec2& size,
                               SatelliteConfig& config, bool* p_open,
                               const char* parentWindowName) {
        if (p_open && !*p_open) return false;

        // 1. Locate parent window to anchor
        ImGuiWindow* parentWin = ImGui::FindWindowByName(parentWindowName);
        ImVec2 parentPos = parentWin ? parentWin->Pos : ImGui::GetIO().DisplaySize * 0.5f;
        ImVec2 parentSize = parentWin ? parentWin->Size : ImVec2(800, 500);

        // 2. Compute dock position
        ImVec2 targetPos = config.currentPos;
        ImVec2 parentEdge(0, 0);
        ImVec2 satelliteEdge(0, 0);

        if (config.anchor != SatelliteAnchor::FreeFloating && config.isPinned) {
            switch (config.anchor) {
                case SatelliteAnchor::DockRight:
                    targetPos.x = parentPos.x + parentSize.x + config.offsetGap;
                    targetPos.y = parentPos.y + 40.0f;
                    parentEdge = ImVec2(parentPos.x + parentSize.x, targetPos.y + size.y * 0.5f);
                    satelliteEdge = ImVec2(targetPos.x, targetPos.y + size.y * 0.5f);
                    break;
                case SatelliteAnchor::DockLeft:
                    targetPos.x = parentPos.x - size.x - config.offsetGap;
                    targetPos.y = parentPos.y + 40.0f;
                    parentEdge = ImVec2(parentPos.x, targetPos.y + size.y * 0.5f);
                    satelliteEdge = ImVec2(targetPos.x + size.x, targetPos.y + size.y * 0.5f);
                    break;
                case SatelliteAnchor::DockTop:
                    targetPos.x = parentPos.x + (parentSize.x - size.x) * 0.5f;
                    targetPos.y = parentPos.y - size.y - config.offsetGap;
                    parentEdge = ImVec2(targetPos.x + size.x * 0.5f, parentPos.y);
                    satelliteEdge = ImVec2(targetPos.x + size.x * 0.5f, targetPos.y + size.y);
                    break;
                case SatelliteAnchor::DockBottom:
                    targetPos.x = parentPos.x + (parentSize.x - size.x) * 0.5f;
                    targetPos.y = parentPos.y + parentSize.y + config.offsetGap;
                    parentEdge = ImVec2(targetPos.x + size.x * 0.5f, parentPos.y + parentSize.y);
                    satelliteEdge = ImVec2(targetPos.x + size.x * 0.5f, targetPos.y);
                    break;
                case SatelliteAnchor::DockTopRight:
                    targetPos.x = parentPos.x + parentSize.x + config.offsetGap;
                    targetPos.y = parentPos.y;
                    parentEdge = ImVec2(parentPos.x + parentSize.x, targetPos.y + 20.0f);
                    satelliteEdge = ImVec2(targetPos.x, targetPos.y + 20.0f);
                    break;
                case SatelliteAnchor::DockBottomRight:
                    targetPos.x = parentPos.x + parentSize.x + config.offsetGap;
                    targetPos.y = parentPos.y + parentSize.y - size.y;
                    parentEdge = ImVec2(parentPos.x + parentSize.x, targetPos.y + size.y - 20.0f);
                    satelliteEdge = ImVec2(targetPos.x, targetPos.y + size.y - 20.0f);
                    break;
                default:
                    break;
            }

            // Spring / magnetic follow smoothing
            if (config.currentPos.x < 0.0f) {
                config.currentPos = targetPos;
            } else if (config.smoothSpring) {
                float dt = ImGui::GetIO().DeltaTime;
                float blend = (std::clamp)(dt * 14.0f, 0.05f, 1.0f);
                config.currentPos.x += (targetPos.x - config.currentPos.x) * blend;
                config.currentPos.y += (targetPos.y - config.currentPos.y) * blend;
            } else {
                config.currentPos = targetPos;
            }

            ImGui::SetNextWindowPos(config.currentPos, ImGuiCond_Always);
        } else {
            // Free floating window
            if (config.currentPos.x > 0.0f) {
                ImGui::SetNextWindowPos(config.currentPos, ImGuiCond_FirstUseEver);
            }
        }

        ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
                               | ImGuiWindowFlags_NoCollapse
                               | ImGuiWindowFlags_NoScrollbar;

        if (config.isPinned) {
            flags |= ImGuiWindowFlags_NoMove;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        bool visible = ImGui::Begin(name, p_open, flags);
        if (visible) {
            ImVec2 curPos = ImGui::GetWindowPos();
            if (!config.isPinned) {
                config.currentPos = curPos;

                // Magnetic Snap Detection
                if (config.magneticSnap && parentWin) {
                    float distRight = std::abs(curPos.x - (parentPos.x + parentSize.x + config.offsetGap));
                    float distLeft = std::abs((curPos.x + size.x) - (parentPos.x - config.offsetGap));
                    if (distRight < config.snapThreshold) {
                        config.anchor = SatelliteAnchor::DockRight;
                        config.isPinned = true;
                        Audio::PlayNotification();
                    } else if (distLeft < config.snapThreshold) {
                        config.anchor = SatelliteAnchor::DockLeft;
                        config.isPinned = true;
                        Audio::PlayNotification();
                    }
                }
            }

            ImDrawList* draw = ImGui::GetWindowDrawList();
            const auto& pal = ThemeManager::Get().GetPalette();
            const auto& sty = ThemeManager::Get().GetStyle();

            // Velvet drop shadow
            Render::ShadowCaster::DrawShadow(draw, curPos, ImVec2(curPos.x + size.x, curPos.y + size.y),
                                             18.0f, sty.CardRounding + 2.0f, Color(0, 0, 0, 0.55f), ImVec2(0, 4.0f));

            // Glassmorphic detached background
            draw->AddRectFilled(curPos, ImVec2(curPos.x + size.x, curPos.y + size.y),
                                pal.Card.WithAlpha(0.92f).ToU32(), sty.CardRounding + 2.0f);

            // Specular top edge sheen
            Render::ImGuiExt::DrawSpecularEdge(draw, curPos, ImVec2(curPos.x + size.x, curPos.y + size.y),
                                               IM_COL32(255, 255, 255, 28), sty.CardRounding + 2.0f, 1.0f);

            // Rotating border or smooth border
            if (config.enableRotatingBorder) {
                FX::RotatingBorder::Draw(draw, curPos, ImVec2(curPos.x + size.x, curPos.y + size.y),
                                         sty.CardRounding + 2.0f, config.borderConfig);
            } else {
                Render::ImGuiExt::AddSmoothBorder(draw, curPos, ImVec2(curPos.x + size.x, curPos.y + size.y),
                                                  pal.Border.WithAlpha(0.70f).ToU32(), sty.CardRounding + 2.0f, 1.0f);
            }

            // Draw luxury connector beam to parent window
            if (config.drawConnectorBeam && config.isPinned && parentWin) {
                ImDrawList* fgDraw = ImGui::GetForegroundDrawList();
                float timeSec = static_cast<float>(ImGui::GetTime());
                DrawSatelliteConnector(fgDraw, parentEdge, satelliteEdge, config.connectorColor, timeSec);
            }

            // Satellite Header Bar
            ImGui::BeginGroup();
            {
                // Status indicator orb
                ImVec2 orbPos = ImGui::GetCursorScreenPos() + ImVec2(4.0f, 6.0f);
                draw->AddCircleFilled(orbPos, 3.5f, pal.Accent.ToU32(), 12);
                draw->AddCircle(orbPos, 5.5f, pal.Accent.WithAlpha(0.35f).ToU32(), 12, 1.0f);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 14.0f);

                ImGui::TextColored(pal.TextPrimary, "%s", title);
                ImGui::SameLine();

                // Pin / Unpin button at top right
                float pinX = size.x - 48.0f;
                ImGui::SetCursorPosX(pinX);

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.1f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.2f));

                const char* pinIcon = config.isPinned ? ICON_FA_LOCK : ICON_FA_UNLOCK;
                if (ImGui::SmallButton(pinIcon)) {
                    config.isPinned = !config.isPinned;
                    if (!config.isPinned) {
                        config.anchor = SatelliteAnchor::FreeFloating;
                    }
                    Audio::PlayClick();
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", config.isPinned ? "Locked to Parent (Click to Undock/Float)" : "Free Floating (Click to Dock)");
                }

                if (p_open) {
                    ImGui::SameLine();
                    if (ImGui::SmallButton(ICON_FA_XMARK)) {
                        *p_open = false;
                        Audio::PlayClick();
                    }
                }
                ImGui::PopStyleColor(3);
            }
            ImGui::EndGroup();

            ImGui::Spacing();
            draw->AddLine(curPos + ImVec2(10.0f, 28.0f), curPos + ImVec2(size.x - 10.0f, 28.0f),
                          pal.Border.WithAlpha(0.40f).ToU32(), 1.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
        }

        return visible;
    }

    void SatelliteWindow::End() {
        Render::ImGuiExt::RenderResizeGrip(ImVec2(160.0f, 90.0f), "##SatelliteResizeGrip");
        ImGui::End();
        ImGui::PopStyleVar(2);
    }

} // namespace Solar::UI
