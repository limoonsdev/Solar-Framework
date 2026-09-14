#include "solar/game/chams_preview.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui.h>

namespace Solar::Game {

    void ChamsPreview::Render(const char* str_id, const ImVec2& size, ChamsMaterial mat, Color col) {
        ImGui::BeginChild(str_id, size, true);
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 sz = ImGui::GetContentRegionAvail();

        draw->AddRectFilled(p, ImVec2(p.x + sz.x, p.y + sz.y), 0xFF0D0F16, 4.0f);
        ImVec2 center(p.x + sz.x * 0.5f, p.y + sz.y * 0.5f);

        draw->AddCircleFilled(center, 35.0f, col.ToU32(), 24);
        if (mat == ChamsMaterial::GlowOutline) {
            draw->AddCircle(center, 40.0f, col.WithAlpha(0.6f).ToU32(), 24, 2.5f);
        }

        ImGui::EndChild();
    }

} // namespace Solar::Game
