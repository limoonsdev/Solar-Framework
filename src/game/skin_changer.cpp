#include "solar/game/skin_changer.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/glow_engine.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/widgets/toggle.hpp"
#include "solar/widgets/slider.hpp"
#include "solar/widgets/combo.hpp"
#include "solar/widgets/button.hpp"
#include "solar/widgets/badge.hpp"
#include "solar/widgets/separator.hpp"
#include <imgui_internal.h>
#include <cmath>
#include <cstdio>
#include <algorithm>

namespace Solar::Game {

    const char* SkinChangerPreview::GetRarityName(SkinRarity rarity) {
        switch (rarity) {
        case SkinRarity::Select:    return "SELECT EDITION";
        case SkinRarity::Deluxe:    return "DELUXE EDITION";
        case SkinRarity::Premium:   return "PREMIUM EDITION";
        case SkinRarity::Ultra:     return "ULTRA EDITION";
        case SkinRarity::Exclusive: return "EXCLUSIVE / COVERT";
        default:                    return "STANDARD";
        }
    }

    ImVec4 SkinChangerPreview::GetRarityColor(SkinRarity rarity) {
        switch (rarity) {
        case SkinRarity::Select:    return ImVec4(0.35f, 0.65f, 1.00f, 1.0f); // Blue
        case SkinRarity::Deluxe:    return ImVec4(0.18f, 0.85f, 0.55f, 1.0f); // Emerald Green
        case SkinRarity::Premium:   return ImVec4(0.72f, 0.35f, 1.00f, 1.0f); // Radiant Purple
        case SkinRarity::Ultra:     return ImVec4(1.00f, 0.28f, 0.55f, 1.0f); // Vivid Pink
        case SkinRarity::Exclusive: return ImVec4(1.00f, 0.78f, 0.15f, 1.0f); // Radiant Gold
        default:                    return ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
        }
    }

    const char* SkinChangerPreview::GetWearName(float wear) {
        if (wear < 0.07f) return "Factory New";
        if (wear < 0.15f) return "Minimal Wear";
        if (wear < 0.38f) return "Field-Tested";
        if (wear < 0.45f) return "Well-Worn";
        return "Battle-Scarred";
    }

    void SkinChangerPreview::Render(const char* str_id, const ImVec2& size, SkinItem& activeItem, std::vector<SkinItem>& inventory) {
        (void)inventory;
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 avail = ImGui::GetContentRegionAvail();
        float w = (size.x > 0.0f) ? size.x : avail.x;
        float h = (size.y > 0.0f) ? size.y : 420.0f;

        ImGui::BeginChild(str_id, ImVec2(w, h), false, ImGuiWindowFlags_NoScrollbar);
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 cMin = ImGui::GetCursorScreenPos();
        ImVec2 cMax = ImVec2(cMin.x + w, cMin.y + h);

        const auto& pal = ThemeManager::Get().GetPalette();
        ImVec4 rarityCol = GetRarityColor(activeItem.rarity);
        u32 rarityColU32 = ImGui::ColorConvertFloat4ToU32(rarityCol);

        // 1. Inspect Showcase Top Box
        float showcaseH = 175.0f;
        ImVec2 showMin = cMin;
        ImVec2 showMax = ImVec2(cMax.x, cMin.y + showcaseH);

        // Dark obsidian backdrop with subtle radial glow
        draw->AddRectFilled(showMin, showMax, IM_COL32(11, 14, 20, 255), 8.0f);
        
        ImVec2 showCenter = ImVec2(showMin.x + w * 0.5f, showMin.y + showcaseH * 0.5f);
        Render::GlowEngine::DrawGlowCircle(draw, showCenter, 45.0f, Color(rarityCol.x, rarityCol.y, rarityCol.z, 0.22f), 30.0f, 0.5f);

        // Subtle holographic shimmer line across weapon box
        float t = static_cast<float>(ImGui::GetTime());
        float shimmerProgress = std::fmod(t * 0.35f, 1.6f) - 0.3f;
        float shimX = showMin.x + w * shimmerProgress;
        draw->AddLine(ImVec2(shimX, showMin.y), ImVec2(shimX + 35.0f, showMax.y), ImGui::ColorConvertFloat4ToU32(ImVec4(rarityCol.x, rarityCol.y, rarityCol.z, 0.18f)), 24.0f);

        // Stylized vector silhouette of the weapon
        float cx = showCenter.x;
        float cy = showCenter.y;
        u32 gunBodyCol = IM_COL32(28, 33, 44, 255);
        u32 gunTrimCol = rarityColU32;

        if (activeItem.weapon.find("Knife") != std::string::npos || activeItem.weapon.find("Karambit") != std::string::npos) {
            // Curved Blade Silhouette (Karambit / Tactical Knife)
            ImVec2 bladePts[] = {
                ImVec2(cx - 70.0f, cy + 20.0f),
                ImVec2(cx - 30.0f, cy + 15.0f),
                ImVec2(cx + 20.0f, cy - 10.0f),
                ImVec2(cx + 75.0f, cy - 5.0f),
                ImVec2(cx + 50.0f, cy + 15.0f),
                ImVec2(cx + 10.0f, cy + 25.0f),
                ImVec2(cx - 40.0f, cy + 30.0f)
            };
            draw->AddConvexPolyFilled(bladePts, 7, gunBodyCol);
            draw->AddPolyline(bladePts, 7, gunTrimCol, ImDrawFlags_Closed, 2.0f);
            draw->AddCircle(ImVec2(cx - 55.0f, cy + 22.0f), 8.0f, gunTrimCol, 16, 2.0f); // Karambit ring
        } else {
            // Rifle / Assault weapon Silhouette (Vandal / Phantom / Operator)
            ImVec2 receiverPts[] = {
                ImVec2(cx - 100.0f, cy + 8.0f),  // Stock
                ImVec2(cx - 85.0f, cy - 6.0f),
                ImVec2(cx - 35.0f, cy - 6.0f),
                ImVec2(cx - 15.0f, cy - 14.0f), // Optic rail
                ImVec2(cx + 45.0f, cy - 14.0f),
                ImVec2(cx + 55.0f, cy - 6.0f),  // Barrel mount
                ImVec2(cx + 115.0f, cy - 4.0f), // Muzzle
                ImVec2(cx + 115.0f, cy + 2.0f),
                ImVec2(cx + 55.0f, cy + 6.0f),  // Handguard
                ImVec2(cx + 20.0f, cy + 12.0f), // Mag top
                ImVec2(cx + 8.0f, cy + 32.0f),  // Mag bottom
                ImVec2(cx - 5.0f, cy + 30.0f),
                ImVec2(cx + 5.0f, cy + 12.0f),
                ImVec2(cx - 20.0f, cy + 12.0f), // Grip top
                ImVec2(cx - 35.0f, cy + 32.0f), // Grip bottom
                ImVec2(cx - 48.0f, cy + 28.0f),
                ImVec2(cx - 35.0f, cy + 10.0f),
                ImVec2(cx - 100.0f, cy + 12.0f) // Lower stock
            };
            draw->AddConvexPolyFilled(receiverPts, 18, gunBodyCol);
            draw->AddPolyline(receiverPts, 18, gunTrimCol, ImDrawFlags_Closed, 2.0f);

            // Barrel accent line
            draw->AddLine(ImVec2(cx + 10.0f, cy - 2.0f), ImVec2(cx + 105.0f, cy - 2.0f), rarityColU32, 1.5f);
        }

        // Rarity Tag Top Left
        const char* rarityTitle = GetRarityName(activeItem.rarity);
        ImVec2 rSize = ImGui::CalcTextSize(rarityTitle);
        ImVec2 rMin(showMin.x + 12.0f, showMin.y + 12.0f);
        ImVec2 rMax(rMin.x + rSize.x + 14.0f, rMin.y + rSize.y + 6.0f);
        draw->AddRectFilled(rMin, rMax, ImGui::ColorConvertFloat4ToU32(ImVec4(rarityCol.x, rarityCol.y, rarityCol.z, 0.20f)), 4.0f);
        draw->AddRect(rMin, rMax, rarityColU32, 4.0f, 0, 1.0f);
        draw->AddText(ImVec2(rMin.x + 7.0f, rMin.y + 3.0f), rarityColU32, rarityTitle);

        // StatTrak Counter Top Right
        if (activeItem.statTrak) {
            char stBuf[48];
            snprintf(stBuf, sizeof(stBuf), "StatTrak™: %d", activeItem.statTrakKills);
            ImVec2 stSize = ImGui::CalcTextSize(stBuf);
            ImVec2 stMin(showMax.x - stSize.x - 22.0f, showMin.y + 12.0f);
            ImVec2 stMax(stMin.x + stSize.x + 14.0f, stMin.y + stSize.y + 6.0f);
            draw->AddRectFilled(stMin, stMax, IM_COL32(230, 110, 20, 45), 4.0f);
            draw->AddRect(stMin, stMax, IM_COL32(255, 130, 30, 220), 4.0f, 0, 1.0f);
            draw->AddText(ImVec2(stMin.x + 7.0f, stMin.y + 3.0f), IM_COL32(255, 140, 35, 255), stBuf);
        }

        // Weapon Name & Skin Title Bottom Left of Showcase
        char titleBuf[128];
        snprintf(titleBuf, sizeof(titleBuf), "%s | %s", activeItem.weapon.c_str(), activeItem.skinName.c_str());
        draw->AddText(ImVec2(showMin.x + 14.0f, showMax.y - 38.0f), IM_COL32(255, 255, 255, 245), titleBuf);

        // Wear and Seed Readout
        char wearBuf[96];
        snprintf(wearBuf, sizeof(wearBuf), "Float: %.4f (%s)  •  Pattern Seed: #%d", activeItem.floatWear, GetWearName(activeItem.floatWear), activeItem.patternSeed);
        draw->AddText(ImVec2(showMin.x + 14.0f, showMax.y - 20.0f), IM_COL32(160, 175, 195, 220), wearBuf);

        // Wear Tier Degradation Bar
        float barY = showMax.y - 5.0f;
        float barW = w - 24.0f;
        ImVec2 bMin(showMin.x + 12.0f, barY);
        ImVec2 bMax(bMin.x + barW, barY + 4.0f);
        draw->AddRectFilled(bMin, bMax, IM_COL32(30, 35, 45, 255), 2.0f);

        // 5 Color Sections of Float Wear: FN (Green), MW (Cyan), FT (Yellow), WW (Orange), BS (Red)
        float splits[5] = { 0.07f, 0.15f, 0.38f, 0.45f, 1.00f };
        u32 splitCols[5] = {
            IM_COL32(60, 220, 120, 200),
            IM_COL32(60, 180, 240, 200),
            IM_COL32(240, 200, 60, 200),
            IM_COL32(240, 120, 40, 200),
            IM_COL32(220, 50, 50, 200)
        };
        float prevSplit = 0.0f;
        for (int i = 0; i < 5; ++i) {
            float sMinX = bMin.x + barW * prevSplit;
            float sMaxX = bMin.x + barW * splits[i];
            draw->AddRectFilled(ImVec2(sMinX, bMin.y), ImVec2(sMaxX, bMax.y), splitCols[i]);
            prevSplit = splits[i];
        }

        // Float needle pin on bar
        float pinX = bMin.x + barW * std::clamp(activeItem.floatWear, 0.0f, 1.0f);
        draw->AddTriangleFilled(ImVec2(pinX, bMin.y - 4.0f), ImVec2(pinX - 3.5f, bMin.y - 8.0f), ImVec2(pinX + 3.5f, bMin.y - 8.0f), IM_COL32(255, 255, 255, 255));

        // Outline on showcase
        Render::ImGuiExt::AddSmoothBorder(draw, showMin, showMax, pal.Border.ToU32(), 8.0f, 1.0f);

        // 2. Interactive Customizer Controls Below Showcase
        ImGui::SetCursorPosY(showcaseH + 12.0f);
        ImGui::SetCursorPosX(14.0f);

        const char* weapons[] = { "Vandal", "Phantom", "Operator", "Karambit Prime", "Butterfly Knife", "Ghost", "Sheriff Reaver" };
        static int selectedWeapon = 0;
        if (Widgets::Combo("Select Weapon", &selectedWeapon, weapons, 7)) {
            activeItem.weapon = weapons[selectedWeapon];
        }

        const char* skins[] = { "Prime 2.0", "Reaver Neon", "Glitchpop Carbon", "Ion Singularity", "Kuronami Water", "Arcane Signature" };
        static int selectedSkin = 0;
        if (Widgets::Combo("Custom Skin Finish", &selectedSkin, skins, 6)) {
            activeItem.skinName = skins[selectedSkin];
        }

        const char* rarities[] = { "Select Edition", "Deluxe Edition", "Premium Edition", "Ultra Edition", "Exclusive / Covert" };
        int currentRarity = static_cast<int>(activeItem.rarity);
        if (Widgets::Combo("Rarity Tier", &currentRarity, rarities, 5)) {
            activeItem.rarity = static_cast<SkinRarity>(currentRarity);
        }

        Widgets::SliderFloat("Float Wear Degradation", &activeItem.floatWear, 0.0001f, 0.9999f, "%.4f");
        Widgets::SliderInt("Pattern Seed Index", &activeItem.patternSeed, 1, 1000, "#%d");

        Widgets::Toggle("StatTrak™ Module", &activeItem.statTrak, "Tracks in-game confirmed eliminations");
        if (activeItem.statTrak) {
            Widgets::SliderInt("StatTrak Counter", &activeItem.statTrakKills, 0, 99999, "%d kills");
        }

        Widgets::Spacing(8.0f);
        if (Widgets::Button(activeItem.equipped ? "Skin Equipped in Loadout" : "Equip Skin to Active Weapon", ImVec2(w - 28.0f, 38.0f), activeItem.equipped ? ButtonStyle::Secondary : ButtonStyle::Primary)) {
            activeItem.equipped = !activeItem.equipped;
            Audio::PlayClick();
            if (activeItem.equipped) {
                Audio::PlayNotification();
            }
        }

        ImGui::EndChild();
    }

} // namespace Solar::Game
