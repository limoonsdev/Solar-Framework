#include "solar/game/esp_preview.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/core/math.hpp"
#include <imgui.h>
#include <cstdio>

namespace Solar::Game {

    void ESPPreview::Render(const char* str_id, const ImVec2& size, const ESPSettings& s) {
        ImGui::BeginChild(str_id, size, true, ImGuiWindowFlags_NoScrollbar);
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 sz = ImGui::GetWindowSize();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        // Dark checkerboard / grid backdrop
        draw->AddRectFilled(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), pal.Background.ToU32(), 6.0f);
        for (float x = pos.x; x < pos.x + sz.x; x += 24.0f) {
            draw->AddLine(ImVec2(x, pos.y), ImVec2(x, pos.y + sz.y), pal.Border.WithAlpha(0.2f).ToU32(), 1.0f);
        }
        for (float y = pos.y; y < pos.y + sz.y; y += 24.0f) {
            draw->AddLine(ImVec2(pos.x, y), ImVec2(pos.x + sz.x, y), pal.Border.WithAlpha(0.2f).ToU32(), 1.0f);
        }

        // Mannequin Bounding Box
        float boxW = 100.0f;
        float boxH = 200.0f;
        ImVec2 boxMin(pos.x + (sz.x - boxW) * 0.5f, pos.y + (sz.y - boxH) * 0.5f + 4.0f);
        ImVec2 boxMax(boxMin.x + boxW, boxMin.y + boxH);

        u32 boxCol = ImGui::ColorConvertFloat4ToU32(s.boxColor);
        u32 blackOutline = 0xDD000000;

        // Snaplines
        if (s.enableSnapline) {
            ImVec2 screenBottom(pos.x + sz.x * 0.5f, pos.y + sz.y);
            draw->AddLine(screenBottom, ImVec2(boxMin.x + boxW * 0.5f, boxMax.y), boxCol, 1.5f);
        }

        // Skeleton Bones
        if (s.enableSkeleton) {
            u32 skelCol = ImGui::ColorConvertFloat4ToU32(s.skeletonColor);
            ImVec2 head(boxMin.x + boxW * 0.5f, boxMin.y + 24.0f);
            ImVec2 neck(boxMin.x + boxW * 0.5f, boxMin.y + 36.0f);
            ImVec2 spine(boxMin.x + boxW * 0.5f, boxMin.y + 85.0f);
            ImVec2 pelvis(boxMin.x + boxW * 0.5f, boxMin.y + 115.0f);

            ImVec2 lShoulder(boxMin.x + 22.0f, boxMin.y + 44.0f);
            ImVec2 rShoulder(boxMax.x - 22.0f, boxMin.y + 44.0f);
            ImVec2 lElbow(boxMin.x + 14.0f, boxMin.y + 80.0f);
            ImVec2 rElbow(boxMax.x - 14.0f, boxMin.y + 80.0f);
            ImVec2 lHand(boxMin.x + 18.0f, boxMin.y + 110.0f);
            ImVec2 rHand(boxMax.x - 18.0f, boxMin.y + 110.0f);

            ImVec2 lKnee(boxMin.x + 30.0f, boxMin.y + 155.0f);
            ImVec2 rKnee(boxMax.x - 30.0f, boxMin.y + 155.0f);
            ImVec2 lFoot(boxMin.x + 26.0f, boxMax.y - 4.0f);
            ImVec2 rFoot(boxMax.x - 26.0f, boxMax.y - 4.0f);

            // Head circle
            draw->AddCircleFilled(head, 12.0f, skelCol, 20);
            draw->AddCircle(head, 12.0f, blackOutline, 20, 1.5f);

            // Torso
            draw->AddLine(neck, spine, skelCol, 2.0f);
            draw->AddLine(spine, pelvis, skelCol, 2.0f);

            // Arms
            draw->AddLine(neck, lShoulder, skelCol, 2.0f);
            draw->AddLine(neck, rShoulder, skelCol, 2.0f);
            draw->AddLine(lShoulder, lElbow, skelCol, 2.0f);
            draw->AddLine(rShoulder, rElbow, skelCol, 2.0f);
            draw->AddLine(lElbow, lHand, skelCol, 2.0f);
            draw->AddLine(rElbow, rHand, skelCol, 2.0f);

            // Legs
            draw->AddLine(pelvis, lKnee, skelCol, 2.0f);
            draw->AddLine(pelvis, rKnee, skelCol, 2.0f);
            draw->AddLine(lKnee, lFoot, skelCol, 2.0f);
            draw->AddLine(rKnee, rFoot, skelCol, 2.0f);
        }

        // Bounding Box
        if (s.enableBox) {
            if (s.boxType == 0) { // Full Box
                draw->AddRect(ImVec2(boxMin.x - 1, boxMin.y - 1), ImVec2(boxMax.x + 1, boxMax.y + 1), blackOutline, 0, 0, 1.5f);
                draw->AddRect(boxMin, boxMax, boxCol, 0, 0, 1.5f);
                draw->AddRect(ImVec2(boxMin.x + 1, boxMin.y + 1), ImVec2(boxMax.x - 1, boxMax.y - 1), blackOutline, 0, 0, 1.5f);
            } else if (s.boxType == 1) { // Corner Box
                float len = 14.0f;
                // Top-left
                draw->AddLine(boxMin, ImVec2(boxMin.x + len, boxMin.y), boxCol, 2.0f);
                draw->AddLine(boxMin, ImVec2(boxMin.x, boxMin.y + len), boxCol, 2.0f);
                // Top-right
                draw->AddLine(ImVec2(boxMax.x, boxMin.y), ImVec2(boxMax.x - len, boxMin.y), boxCol, 2.0f);
                draw->AddLine(ImVec2(boxMax.x, boxMin.y), ImVec2(boxMax.x, boxMin.y + len), boxCol, 2.0f);
                // Bottom-left
                draw->AddLine(ImVec2(boxMin.x, boxMax.y), ImVec2(boxMin.x + len, boxMax.y), boxCol, 2.0f);
                draw->AddLine(ImVec2(boxMin.x, boxMax.y), ImVec2(boxMin.x, boxMax.y - len), boxCol, 2.0f);
                // Bottom-right
                draw->AddLine(boxMax, ImVec2(boxMax.x - len, boxMax.y), boxCol, 2.0f);
                draw->AddLine(boxMax, ImVec2(boxMax.x, boxMax.y - len), boxCol, 2.0f);
            } else if (s.boxType == 2) { // Filled Box
                draw->AddRectFilled(boxMin, boxMax, ImGui::ColorConvertFloat4ToU32(ImVec4(s.boxColor.x, s.boxColor.y, s.boxColor.z, 0.15f)));
                draw->AddRect(boxMin, boxMax, boxCol, 0, 0, 1.5f);
            }
        }

        // Health Bar (Left)
        if (s.enableHealthBar) {
            float barW = 4.0f;
            float barPad = 5.0f;
            ImVec2 barBgMin(boxMin.x - barPad - barW, boxMin.y);
            ImVec2 barBgMax(boxMin.x - barPad, boxMax.y);
            draw->AddRectFilled(ImVec2(barBgMin.x - 1, barBgMin.y - 1), ImVec2(barBgMax.x + 1, barBgMax.y + 1), blackOutline);

            float hpFrac = Math::Clamp(s.health / 100.0f, 0.0f, 1.0f);
            float fillH = boxH * hpFrac;
            ImVec2 barFillMin(barBgMin.x, boxMax.y - fillH);
            Color hpCol = Color::Lerp(Color(1.0f, 0.2f, 0.2f, 1.0f), Color(0.2f, 1.0f, 0.3f, 1.0f), hpFrac);
            draw->AddRectFilled(barFillMin, barBgMax, hpCol.ToU32());

            if (s.health < 100.0f) {
                char hpBuf[16];
                snprintf(hpBuf, sizeof(hpBuf), "%.0f", s.health);
                draw->AddText(ImVec2(barBgMin.x - 18.0f, barFillMin.y - 6.0f), 0xFFFFFFFF, hpBuf);
            }
        }

        // Armor Bar (Left of Health Bar)
        if (s.enableArmorBar) {
            float barW = 3.0f;
            float barPad = 12.0f;
            ImVec2 barBgMin(boxMin.x - barPad - barW, boxMin.y);
            ImVec2 barBgMax(boxMin.x - barPad, boxMax.y);
            draw->AddRectFilled(ImVec2(barBgMin.x - 1, barBgMin.y - 1), ImVec2(barBgMax.x + 1, barBgMax.y + 1), blackOutline);

            float armFrac = Math::Clamp(s.armor / 100.0f, 0.0f, 1.0f);
            float fillH = boxH * armFrac;
            draw->AddRectFilled(ImVec2(barBgMin.x, boxMax.y - fillH), barBgMax, 0xFF38BDF8);
        }

        // Name Tag (Top)
        if (s.enableName) {
            ImVec2 ns = ImGui::CalcTextSize(s.playerName.c_str());
            ImVec2 np(boxMin.x + (boxW - ns.x) * 0.5f, boxMin.y - ns.y - 4.0f);
            draw->AddText(np, 0xFFFFFFFF, s.playerName.c_str());
        }

        // Weapon (Bottom)
        if (s.enableWeapon) {
            ImVec2 ws = ImGui::CalcTextSize(s.weaponName.c_str());
            ImVec2 wp(boxMin.x + (boxW - ws.x) * 0.5f, boxMax.y + 4.0f);
            draw->AddText(wp, 0xFFD0D6E0, s.weaponName.c_str());
        }

        // Distance (Right tags)
        if (s.enableDistance) {
            char distBuf[32];
            snprintf(distBuf, sizeof(distBuf), "%.1fm", s.distance);
            draw->AddText(ImVec2(boxMax.x + 6.0f, boxMin.y + 2.0f), 0xFFFFAE3D, distBuf);
        }

        ImGui::EndChild();
    }

} // namespace Solar::Game
