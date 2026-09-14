#include "solar/game/radar.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/core/math.hpp"
#include <imgui.h>

namespace Solar::Game {

    void Radar::Render(const char* str_id, const ImVec2& size, const std::vector<RadarBlip>& blips) {
        ImGui::BeginChild(str_id, size, true, ImGuiWindowFlags_NoScrollbar);
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 sz = ImGui::GetWindowSize();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        // Radar background
        draw->AddRectFilled(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), pal.Background.ToU32(), 6.0f);

        ImVec2 center(pos.x + sz.x * 0.5f, pos.y + sz.y * 0.5f);
        float radius = std::min(sz.x, sz.y) * 0.45f;

        // Concentric range circles
        draw->AddCircle(center, radius * 0.33f, pal.Border.WithAlpha(0.25f).ToU32(), 32);
        draw->AddCircle(center, radius * 0.66f, pal.Border.WithAlpha(0.25f).ToU32(), 32);
        draw->AddCircle(center, radius, pal.Border.WithAlpha(0.5f).ToU32(), 48, 1.5f);

        // Crosshairs
        draw->AddLine(ImVec2(center.x - radius, center.y), ImVec2(center.x + radius, center.y), pal.Border.WithAlpha(0.3f).ToU32());
        draw->AddLine(ImVec2(center.x, center.y - radius), ImVec2(center.x, center.y + radius), pal.Border.WithAlpha(0.3f).ToU32());

        // Local Player Dot & FOV Cone
        draw->AddCircleFilled(center, 4.0f, pal.Accent.ToU32(), 16);
        float fovLen = 28.0f;
        draw->AddLine(center, ImVec2(center.x - 12.0f, center.y - fovLen), pal.Accent.WithAlpha(0.5f).ToU32(), 1.5f);
        draw->AddLine(center, ImVec2(center.x + 12.0f, center.y - fovLen), pal.Accent.WithAlpha(0.5f).ToU32(), 1.5f);

        // Render Blips
        for (const auto& b : blips) {
            ImVec2 blipPos(center.x + b.pos.x, center.y + b.pos.y);
            if (Math::Distance(center, blipPos) > radius) continue;

            u32 col = b.isEnemy ? pal.Danger.ToU32() : pal.Success.ToU32();
            draw->AddCircleFilled(blipPos, 4.5f, col, 16);
            draw->AddCircle(blipPos, 5.5f, 0xFF000000, 16, 1.0f);

            // Elevation triangle
            if (b.altitude > 0) {
                draw->AddTriangleFilled(
                    ImVec2(blipPos.x, blipPos.y - 7.0f),
                    ImVec2(blipPos.x - 3.5f, blipPos.y - 3.0f),
                    ImVec2(blipPos.x + 3.5f, blipPos.y - 3.0f),
                    0xFFFFFFFF
                );
            } else if (b.altitude < 0) {
                draw->AddTriangleFilled(
                    ImVec2(blipPos.x, blipPos.y + 7.0f),
                    ImVec2(blipPos.x - 3.5f, blipPos.y + 3.0f),
                    ImVec2(blipPos.x + 3.5f, blipPos.y + 3.0f),
                    0xFFFFFFFF
                );
            }
        }

        ImGui::EndChild();
    }

} // namespace Solar::Game
