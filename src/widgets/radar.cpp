#include "solar/widgets/radar.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/shadow_caster.hpp"
#include <imgui_internal.h>
#include <cmath>
#include <algorithm>

namespace Solar::Widgets {

    void Radar(const char* label, const ImVec2& sizeArg,
               const RadarSettings& settings,
               const std::vector<RadarEntity>& entities)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return;

        ImGuiContext& g = *GImGui;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        float width = sizeArg.x > 0.0f ? sizeArg.x : ImGui::GetContentRegionAvail().x;
        float height = sizeArg.y > 0.0f ? sizeArg.y : 220.0f;

        ImVec2 p = ImGui::GetCursorScreenPos();
        ImRect bb(p, ImVec2(p.x + width, p.y + height));

        ImGuiID id = window->GetID(label);
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return;

        float rounding = 10.0f;

        // Container Card
        Render::ShadowCaster::DrawShadow(draw, bb.Min, bb.Max, 12.0f, rounding, Color(0, 0, 0, 0.40f), ImVec2(0, 3.0f));
        draw->AddRectFilled(bb.Min, bb.Max, pal.Card.ToU32(), rounding);
        Render::ImGuiExt::DrawSpecularEdge(draw, bb.Min, bb.Max, IM_COL32(255, 255, 255, 22), rounding, 1.0f);
        Render::ImGuiExt::AddSmoothBorder(draw, bb.Min, bb.Max, pal.Border.ToU32(), rounding, 1.0f);

        // Header Title
        auto lv = Render::CleanLabel(label);
        draw->AddText(ImVec2(bb.Min.x + 12.0f, bb.Min.y + 10.0f), pal.TextPrimary.ToU32(), lv.textBegin, lv.textEnd);

        // Radar center and radius
        float headerH = 34.0f;
        float contentW = width - 24.0f;
        float contentH = height - headerH - 12.0f;
        float diameter = (std::min)(contentW, contentH);
        float radius = (diameter - 8.0f) * 0.5f;

        ImVec2 center(bb.Min.x + width * 0.5f, bb.Min.y + headerH + contentH * 0.5f);

        if (radius <= 10.0f) return;

        // 1. Radar Circular Body Shadow & Fill
        for (int l = 4; l >= 1; --l) {
            float exp = static_cast<float>(l) * 2.0f;
            draw->AddCircleFilled(center, radius + exp, IM_COL32(0, 0, 0, static_cast<int>(18 * (5 - l))), 48);
        }
        draw->AddCircleFilled(center, radius, pal.Background.WithAlpha(0.92f).ToU32(), 64);

        // 2. Concentric Range Rings
        if (settings.showRings) {
            draw->AddCircle(center, radius * 0.33f, pal.Border.WithAlpha(0.25f).ToU32(), 48, 1.0f);
            draw->AddCircle(center, radius * 0.66f, pal.Border.WithAlpha(0.25f).ToU32(), 48, 1.0f);
            draw->AddCircle(center, radius, pal.Border.WithAlpha(0.40f).ToU32(), 64, 1.0f);

            // Range label
            char rangeBuf[16];
            std::snprintf(rangeBuf, sizeof(rangeBuf), "%.0fm", settings.rangeMeters);
            draw->AddText(ImVec2(center.x + 4.0f, center.y - radius + 4.0f), pal.TextSecondary.WithAlpha(0.50f).ToU32(), rangeBuf);
        }

        // 3. Crosshairs
        draw->AddLine(ImVec2(center.x - radius, center.y), ImVec2(center.x + radius, center.y),
                      pal.Border.WithAlpha(0.30f).ToU32(), 1.0f);
        draw->AddLine(ImVec2(center.x, center.y - radius), ImVec2(center.x, center.y + radius),
                      pal.Border.WithAlpha(0.30f).ToU32(), 1.0f);

        // 4. Cardinal Markers
        if (settings.showCardinal) {
            // North
            ImVec2 nSz = ImGui::CalcTextSize("N");
            draw->AddText(ImVec2(center.x - nSz.x * 0.5f, center.y - radius + 5.0f), pal.Accent.ToU32(), "N");
            // South
            ImVec2 sSz = ImGui::CalcTextSize("S");
            draw->AddText(ImVec2(center.x - sSz.x * 0.5f, center.y + radius - sSz.y - 5.0f), pal.TextSecondary.WithAlpha(0.40f).ToU32(), "S");
            // East
            ImVec2 eSz = ImGui::CalcTextSize("E");
            draw->AddText(ImVec2(center.x + radius - eSz.x - 6.0f, center.y - eSz.y * 0.5f), pal.TextSecondary.WithAlpha(0.40f).ToU32(), "E");
            // West
            ImVec2 wSz = ImGui::CalcTextSize("W");
            draw->AddText(ImVec2(center.x - radius + 6.0f, center.y - wSz.y * 0.5f), pal.TextSecondary.WithAlpha(0.40f).ToU32(), "W");
        }

        // 5. Continuous Phosphorescent Radar Sweep
        if (settings.showSweep) {
            static float s_sweepAngle = 0.0f;
            s_sweepAngle += g.IO.DeltaTime * settings.sweepSpeed;
            if (s_sweepAngle >= 360.0f) s_sweepAngle -= 360.0f;

            float sweepRad = s_sweepAngle * (3.14159265f / 180.0f);
            const int trailSegments = 16;
            const float trailArc = 45.0f * (3.14159265f / 180.0f);

            for (int t = 0; t < trailSegments; ++t) {
                float a0 = sweepRad - trailArc * (static_cast<float>(t + 1) / static_cast<float>(trailSegments));
                float a1 = sweepRad - trailArc * (static_cast<float>(t) / static_cast<float>(trailSegments));

                float alpha = (1.0f - static_cast<float>(t) / static_cast<float>(trailSegments)) * 0.18f;
                u32 wedgeCol = pal.Accent.WithAlpha(alpha).ToU32();

                ImVec2 p0(center.x + std::cos(a0) * radius, center.y + std::sin(a0) * radius);
                ImVec2 p1(center.x + std::cos(a1) * radius, center.y + std::sin(a1) * radius);

                draw->AddTriangleFilled(center, p0, p1, wedgeCol);
            }

            // Crisp sweep beam
            ImVec2 beamEnd(center.x + std::cos(sweepRad) * radius, center.y + std::sin(sweepRad) * radius);
            draw->AddLine(center, beamEnd, pal.Accent.WithAlpha(0.70f).ToU32(), 1.5f);
        }

        // 6. Center Local Player Blip & Forward Heading Cone
        draw->AddCircleFilled(center, 4.0f, IM_COL32(255, 255, 255, 255));
        draw->AddCircle(center, 4.0f, pal.Accent.ToU32(), 16, 1.5f);

        // Forward FOV View Cone (V-shape)
        float fovLen = 14.0f;
        float fovAngle = 35.0f * (3.14159265f / 180.0f);
        ImVec2 fovLeft(center.x - std::sin(fovAngle) * fovLen, center.y - std::cos(fovAngle) * fovLen);
        ImVec2 fovRight(center.x + std::sin(fovAngle) * fovLen, center.y - std::cos(fovAngle) * fovLen);

        draw->AddLine(center, fovLeft, pal.Accent.WithAlpha(0.60f).ToU32(), 1.0f);
        draw->AddLine(center, fovRight, pal.Accent.WithAlpha(0.60f).ToU32(), 1.0f);

        // 7. Tracked Entity Blips
        float meterToPix = radius / (settings.rangeMeters > 0.001f ? settings.rangeMeters : 1.0f);

        for (const auto& ent : entities) {
            float relX = ent.x * meterToPix;
            float relY = -ent.y * meterToPix; // Forward in world is -Y in screen space
            float dist = std::sqrt(relX * relX + relY * relY);

            ImVec2 blipPos;
            bool clamped = false;

            if (dist > radius - 4.0f) {
                float norm = (radius - 4.0f) / dist;
                blipPos = ImVec2(center.x + relX * norm, center.y + relY * norm);
                clamped = true;
            } else {
                blipPos = ImVec2(center.x + relX, center.y + relY);
            }

            Color blipColor = ent.isEnemy ? pal.Danger : Color(0.18f, 0.78f, 0.52f, 1.0f);
            if (ent.isDormant) {
                blipColor = blipColor.WithAlpha(0.40f);
            }

            if (clamped) {
                // Directional peripheral arrow
                float angle = std::atan2(relY, relX);
                ImVec2 arrTip(center.x + std::cos(angle) * (radius - 2.0f), center.y + std::sin(angle) * (radius - 2.0f));
                ImVec2 arrB1(center.x + std::cos(angle + 0.22f) * (radius - 7.0f), center.y + std::sin(angle + 0.22f) * (radius - 7.0f));
                ImVec2 arrB2(center.x + std::cos(angle - 0.22f) * (radius - 7.0f), center.y + std::sin(angle - 0.22f) * (radius - 7.0f));
                draw->AddTriangleFilled(arrTip, arrB1, arrB2, blipColor.ToU32());
            } else {
                // Entity dot
                float blipRadius = 3.5f;
                draw->AddCircleFilled(blipPos, blipRadius, blipColor.ToU32(), 16);
                draw->AddCircle(blipPos, blipRadius, IM_COL32(255, 255, 255, 120), 16, 1.0f);

                // Directional Heading Indicator
                if (settings.showHeadingCones) {
                    float yawRad = (ent.yaw - 90.0f) * (3.14159265f / 180.0f);
                    float dirLen = 8.0f;
                    ImVec2 dirEnd(blipPos.x + std::cos(yawRad) * dirLen, blipPos.y + std::sin(yawRad) * dirLen);
                    draw->AddLine(blipPos, dirEnd, blipColor.ToU32(), 1.5f);
                }

                // Altitude Indicator (^ if above, v if below)
                if (std::abs(ent.z) > 1.8f) {
                    const char* altChar = ent.z > 0.0f ? "^" : "v";
                    draw->AddText(ImVec2(blipPos.x + 4.0f, blipPos.y - 7.0f), blipColor.ToU32(), altChar);
                }
            }
        }

        // 8. Outer Bezel & Specular Sheen
        draw->AddCircle(center, radius, pal.Border.ToU32(), 64, 1.2f);
        // Top specular arc sheen
        draw->PathArcTo(center, radius - 0.5f, -3.14159f * 0.75f, -3.14159f * 0.25f, 32);
        draw->PathStroke(IM_COL32(255, 255, 255, 45), 0, 1.2f);
    }

} // namespace Solar::Widgets
