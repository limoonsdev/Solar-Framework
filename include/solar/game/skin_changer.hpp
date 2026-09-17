#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>
#include <string>
#include <vector>

namespace Solar::Game {

    enum class SkinRarity : u8 {
        Select = 0,    // Standard Blue
        Deluxe = 1,    // Green
        Premium = 2,   // Deep Purple
        Ultra = 3,     // Vivid Pink / Crimson
        Exclusive = 4  // Radiant Gold
    };

    struct SkinItem {
        std::string weapon;
        std::string skinName;
        SkinRarity rarity;
        float floatWear = 0.0350f;      // 0.00 to 1.00
        int patternSeed = 661;          // 1 to 1000
        bool statTrak = true;
        int statTrakKills = 1337;
        ImVec4 customTint = ImVec4(0.20f, 0.65f, 1.0f, 1.0f);
        bool equipped = false;
    };

    class SkinChangerPreview {
    public:
        // Returns the rarity title string
        static const char* GetRarityName(SkinRarity rarity);

        // Returns the color associated with the rarity tier
        static ImVec4 GetRarityColor(SkinRarity rarity);

        // Returns wear condition name based on float value
        static const char* GetWearName(float wear);

        // Main interactive skin preview & customizer card
        static void Render(const char* str_id, const ImVec2& size, SkinItem& activeItem, std::vector<SkinItem>& inventory);
    };

} // namespace Solar::Game
