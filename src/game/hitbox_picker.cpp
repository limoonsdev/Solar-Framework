#include "solar/game/hitbox_picker.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include <imgui_internal.h>

namespace Solar::Game {

    void HitboxPicker::Render(std::vector<bool>& hitboxes, const ImVec2& sizeArg) {
        if (hitboxes.size() < 8) hitboxes.resize(8, true);

        float availX = ImGui::GetContentRegionAvail().x;
        ImVec2 size = sizeArg;
        if (size.x <= 0.0f) size.x = availX;

        ImGui::BeginChild("##HitboxPickerChild", size, true, ImGuiWindowFlags_NoScrollbar);
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 sz = ImGui::GetWindowSize();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        // Dark canvas
        draw->AddRectFilled(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), pal.Background.ToU32(), 4.0f);

        float centerX = pos.x + sz.x * 0.35f;
        float startY = pos.y + 16.0f;

        struct ZoneDef {
            int idx;
            const char* name;
            ImVec2 min;
            ImVec2 max;
        };

        ZoneDef zones[] = {
            { 0, "Head",        ImVec2(centerX - 14.0f, startY),         ImVec2(centerX + 14.0f, startY + 28.0f) },
            { 1, "Neck",        ImVec2(centerX - 8.0f,  startY + 29.0f), ImVec2(centerX + 8.0f,  startY + 38.0f) },
            { 2, "Upper Chest", ImVec2(centerX - 22.0f, startY + 39.0f), ImVec2(centerX + 22.0f, startY + 68.0f) },
            { 3, "Stomach",     ImVec2(centerX - 18.0f, startY + 69.0f), ImVec2(centerX + 18.0f, startY + 95.0f) },
            { 4, "Pelvis",      ImVec2(centerX - 19.0f, startY + 96.0f), ImVec2(centerX + 19.0f, startY + 115.0f) },
            { 5, "Arms",        ImVec2(centerX - 38.0f, startY + 42.0f), ImVec2(centerX - 24.0f, startY + 105.0f) },
            { 6, "Legs",        ImVec2(centerX - 20.0f, startY + 116.0f), ImVec2(centerX - 5.0f, startY + 175.0f) },
            { 7, "Feet",        ImVec2(centerX - 22.0f, startY + 176.0f), ImVec2(centerX - 4.0f, startY + 192.0f) },
        };

        for (const auto& z : zones) {
            ImGuiID id = ImGui::GetID((std::string("##HB_") + z.name).c_str());
            ImRect bb(z.min, z.max);
            bool hovered, held;
            bool clicked = ImGui::ButtonBehavior(bb, id, &hovered, &held);
            if (clicked) {
                hitboxes[z.idx] = !hitboxes[z.idx];
                Audio::PlayToggle(hitboxes[z.idx]);
            }

            bool active = hitboxes[z.idx];
            Color fillCol = active ? (hovered ? pal.AccentHover : pal.Accent)
                                   : (hovered ? pal.CardHover : pal.Card);
            draw->AddRectFilled(z.min, z.max, fillCol.ToU32(), 4.0f);
            draw->AddRect(z.min, z.max, active ? pal.AccentActive.ToU32() : pal.Border.ToU32(), 4.0f);

            // Right arm mirror
            if (z.idx == 5) {
                ImVec2 rMin(centerX + 24.0f, z.min.y);
                ImVec2 rMax(centerX + 38.0f, z.max.y);
                draw->AddRectFilled(rMin, rMax, fillCol.ToU32(), 4.0f);
                draw->AddRect(rMin, rMax, active ? pal.AccentActive.ToU32() : pal.Border.ToU32(), 4.0f);
            }
            // Right leg mirror
            if (z.idx == 6) {
                ImVec2 rMin(centerX + 5.0f, z.min.y);
                ImVec2 rMax(centerX + 20.0f, z.max.y);
                draw->AddRectFilled(rMin, rMax, fillCol.ToU32(), 4.0f);
                draw->AddRect(rMin, rMax, active ? pal.AccentActive.ToU32() : pal.Border.ToU32(), 4.0f);
            }
            // Right foot mirror
            if (z.idx == 7) {
                ImVec2 rMin(centerX + 4.0f, z.min.y);
                ImVec2 rMax(centerX + 22.0f, z.max.y);
                draw->AddRectFilled(rMin, rMax, fillCol.ToU32(), 4.0f);
                draw->AddRect(rMin, rMax, active ? pal.AccentActive.ToU32() : pal.Border.ToU32(), 4.0f);
            }
        }

        // Checklist on the right
        float listX = pos.x + sz.x * 0.65f;
        ImGui::SetCursorScreenPos(ImVec2(listX, pos.y + 12.0f));
        ImGui::BeginGroup();
        {
            const char* names[] = { "Head", "Neck", "Chest", "Stomach", "Pelvis", "Arms", "Legs", "Feet" };
            for (int i = 0; i < 8; i++) {
                bool val = hitboxes[i];
                if (ImGui::Checkbox(names[i], &val)) {
                    hitboxes[i] = val;
                    Audio::PlayToggle(val);
                }
            }
        }
        ImGui::EndGroup();

        ImGui::EndChild();
    }

} // namespace Solar::Game
