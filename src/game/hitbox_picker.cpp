#include "solar/game/hitbox_picker.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/widgets/checkbox.hpp"
#include <imgui_internal.h>
#include <cmath>

namespace Solar::Game {

    void HitboxPicker::Render(std::vector<bool>& hitboxes, const ImVec2& sizeArg) {
        if (hitboxes.size() < 8) hitboxes.resize(8, true);

        float availX = ImGui::GetContentRegionAvail().x;
        ImVec2 size = sizeArg;
        if (size.x <= 0.0f) size.x = availX;
        if (size.y <= 0.0f) size.y = 230.0f;

        ImGui::BeginChild("##HitboxPickerChild", size, false, ImGuiWindowFlags_NoScrollbar);
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 sz = ImGui::GetWindowSize();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        // 1. Dark Tactical Viewport
        draw->AddRectFilled(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), IM_COL32(11, 12, 16, 240), 6.0f);
        draw->AddRect(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), IM_COL32(255, 255, 255, 16), 6.0f, 0, 1.0f);

        float mannequinCenterX = pos.x + sz.x * 0.30f;
        float startY = pos.y + 16.0f;

        struct CapsuleZone {
            int idx;
            const char* name;
            const char* mult;
            ImVec2 min;
            ImVec2 max;
            float rounding;
        };

        CapsuleZone zones[] = {
            { 0, "Head",        "4.0x Lethal",  ImVec2(mannequinCenterX - 11.0f, startY),         ImVec2(mannequinCenterX + 11.0f, startY + 22.0f), 10.0f },
            { 1, "Neck",        "1.5x Vital",   ImVec2(mannequinCenterX - 6.0f,  startY + 23.0f), ImVec2(mannequinCenterX + 6.0f,  startY + 32.0f), 3.0f },
            { 2, "Upper Chest", "1.0x Torso",   ImVec2(mannequinCenterX - 19.0f, startY + 34.0f), ImVec2(mannequinCenterX + 19.0f, startY + 62.0f), 6.0f },
            { 3, "Stomach",     "1.0x Midbody", ImVec2(mannequinCenterX - 16.0f, startY + 64.0f), ImVec2(mannequinCenterX + 16.0f, startY + 88.0f), 5.0f },
            { 4, "Pelvis",      "0.9x Base",    ImVec2(mannequinCenterX - 17.0f, startY + 90.0f), ImVec2(mannequinCenterX + 17.0f, startY + 108.0f), 5.0f },
            { 5, "Left Arm",    "0.8x Limb",    ImVec2(mannequinCenterX - 34.0f, startY + 38.0f), ImVec2(mannequinCenterX - 21.0f, startY + 98.0f), 6.0f },
            { 5, "Right Arm",   "0.8x Limb",    ImVec2(mannequinCenterX + 21.0f, startY + 38.0f), ImVec2(mannequinCenterX + 34.0f, startY + 98.0f), 6.0f },
            { 6, "Left Leg",    "0.75x Limb",   ImVec2(mannequinCenterX - 18.0f, startY + 110.0f), ImVec2(mannequinCenterX - 5.0f, startY + 170.0f), 6.0f },
            { 6, "Right Leg",   "0.75x Limb",   ImVec2(mannequinCenterX + 5.0f,  startY + 110.0f), ImVec2(mannequinCenterX + 18.0f, startY + 170.0f), 6.0f },
            { 7, "Left Foot",   "0.6x Extrem",  ImVec2(mannequinCenterX - 20.0f, startY + 172.0f), ImVec2(mannequinCenterX - 5.0f, startY + 186.0f), 4.0f },
            { 7, "Right Foot",  "0.6x Extrem",  ImVec2(mannequinCenterX + 5.0f,  startY + 172.0f), ImVec2(mannequinCenterX + 20.0f, startY + 186.0f), 4.0f }
        };

        // 2. Bone Connection Linkages (Underlay)
        draw->AddLine(ImVec2(mannequinCenterX, startY + 22.0f), ImVec2(mannequinCenterX, startY + 110.0f), IM_COL32(255, 255, 255, 40), 1.6f);
        draw->AddLine(ImVec2(mannequinCenterX, startY + 45.0f), ImVec2(mannequinCenterX - 27.0f, startY + 45.0f), IM_COL32(255, 255, 255, 40), 1.6f);
        draw->AddLine(ImVec2(mannequinCenterX, startY + 45.0f), ImVec2(mannequinCenterX + 27.0f, startY + 45.0f), IM_COL32(255, 255, 255, 40), 1.6f);
        draw->AddLine(ImVec2(mannequinCenterX, startY + 105.0f), ImVec2(mannequinCenterX - 12.0f, startY + 115.0f), IM_COL32(255, 255, 255, 40), 1.6f);
        draw->AddLine(ImVec2(mannequinCenterX, startY + 105.0f), ImVec2(mannequinCenterX + 12.0f, startY + 115.0f), IM_COL32(255, 255, 255, 40), 1.6f);

        // 3. Render Interactive Capsules
        for (const auto& z : zones) {
            ImGuiID id = ImGui::GetID((std::string("##HBZone_") + z.name).c_str());
            ImRect bb(z.min, z.max);
            bool hovered, held;
            bool clicked = ImGui::ButtonBehavior(bb, id, &hovered, &held);

            if (clicked) {
                hitboxes[z.idx] = !hitboxes[z.idx];
                Audio::PlayToggle(hitboxes[z.idx]);
            }

            bool active = hitboxes[z.idx];

            if (active) {
                // Active glowing capsule
                u32 fillCol = hovered ? pal.AccentHover.ToU32() : pal.Accent.ToU32();
                draw->AddRectFilled(z.min, z.max, fillCol, z.rounding);
                draw->AddRect(z.min, z.max, IM_COL32(255, 255, 255, 200), z.rounding, 0, 1.2f);

                // Capsule soft aura
                draw->AddRect(
                    ImVec2(z.min.x - 2.0f, z.min.y - 2.0f),
                    ImVec2(z.max.x + 2.0f, z.max.y + 2.0f),
                    pal.Accent.WithAlpha(0.35f).ToU32(),
                    z.rounding + 2.0f, 0, 1.5f
                );
            } else {
                // Inactive obsidian capsule
                u32 fillCol = hovered ? IM_COL32(32, 34, 44, 230) : IM_COL32(18, 20, 26, 230);
                draw->AddRectFilled(z.min, z.max, fillCol, z.rounding);
                draw->AddRect(z.min, z.max, IM_COL32(255, 255, 255, 25), z.rounding, 0, 1.0f);
            }

            // Hover Tooltip showing damage multiplier & status
            if (hovered) {
                ImGui::BeginTooltip();
                ImGui::TextColored(pal.Accent, "%s Target Zone", z.name);
                ImGui::Separator();
                ImGui::TextColored(pal.TextSecondary, "Damage Multiplier: %s", z.mult);
                ImGui::TextColored(active ? pal.Success : pal.TextDisabled, "Status: %s", active ? "ENABLED" : "DISABLED");
                ImGui::EndTooltip();
            }
        }

        // 4. Checklist on the right side using Custom Squircle Checkboxes
        float listX = pos.x + sz.x * 0.58f;
        ImGui::SetCursorScreenPos(ImVec2(listX, pos.y + 14.0f));

        ImGui::BeginGroup();
        {
            const char* names[] = { "Headshot Priority", "Neck Target", "Upper Chest", "Stomach Area", "Pelvis / Hips", "Arms & Hands", "Legs", "Feet & Ankles" };
            for (int i = 0; i < 8; i++) {
                bool val = hitboxes[i];
                ImGui::PushID(i);
                if (Widgets::Checkbox(names[i], &val)) {
                    hitboxes[i] = val;
                }
                ImGui::PopID();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
            }
        }
        ImGui::EndGroup();

        ImGui::EndChild();
    }

} // namespace Solar::Game
