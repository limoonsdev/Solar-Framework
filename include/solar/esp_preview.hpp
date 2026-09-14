#pragma once

#include <imgui.h>
#include <string>

namespace Solar {

    struct ESPPreviewSettings {
        bool enableBox = true;
        int  boxType = 0; // 0 = Full Box, 1 = Corner Box, 2 = Filled Box
        ImVec4 boxColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        bool enableSkeleton = true;
        ImVec4 skeletonColor = ImVec4(0.25f, 0.85f, 1.0f, 1.0f);

        bool enableHealthBar = true;
        float health = 85.0f; // 0 to 100

        bool enableArmorBar = true;
        float armor = 60.0f; // 0 to 100

        bool enableName = true;
        std::string playerName = "Target_Player";

        bool enableWeapon = true;
        std::string weaponName = "Vandal [25/75]";

        bool enableDistance = true;
        float distance = 28.5f;

        bool enableSnapline = false;
        ImVec4 snaplineColor = ImVec4(1.0f, 0.55f, 0.0f, 0.75f);

        bool enableGlow = true;
        ImVec4 glowColor = ImVec4(1.0f, 0.55f, 0.0f, 0.35f);

        bool enableHeadDot = true;
    };

    class ESPPreview {
    public:
        static void Render(const char* id, const ImVec2& size, const ESPPreviewSettings& settings);
    };

} // namespace Solar
