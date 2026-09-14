#pragma once
#include "solar/core/types.hpp"
#include <string>

namespace Solar::Game {

    struct ESPSettings {
        bool enableBox = true;
        int boxType = 1; // 0=Full, 1=Corner, 2=Filled
        ImVec4 boxColor = ImVec4(1.0f, 0.62f, 0.11f, 1.0f);

        bool enableSkeleton = true;
        ImVec4 skeletonColor = ImVec4(1.0f, 1.0f, 1.0f, 0.85f);

        bool enableHealthBar = true;
        bool enableArmorBar = true;
        bool enableName = true;
        bool enableWeapon = true;
        bool enableDistance = true;
        bool enableSnapline = false;
        bool enableGlow = true;

        float health = 85.0f;
        float armor = 60.0f;
        float distance = 28.5f;

        std::string playerName = "Target_Bot01";
        std::string weaponName = "Vandal [25/75]";
    };

    class ESPPreview {
    public:
        static void Render(const char* str_id, const ImVec2& size, const ESPSettings& settings);
    };

} // namespace Solar::Game
