#pragma once
#include "solar/core/types.hpp"
#include <string>

namespace Solar::Game {

    enum class TargetStance : u8 {
        Standing = 0,
        Crouching,
        Scoped,
        Jumping,
        Defusing
    };

    struct ESPSettings {
        bool enableBox = true;
        int boxType = 1; // 0=Full, 1=Corner, 2=Filled, 3=Gradient
        ImVec4 boxColor = ImVec4(1.0f, 0.62f, 0.11f, 1.0f);
        ImVec4 boxGradientBottom = ImVec4(1.0f, 0.22f, 0.11f, 1.0f);

        bool enableSkeleton = true;
        ImVec4 skeletonColor = ImVec4(1.0f, 1.0f, 1.0f, 0.85f);

        bool enableHealthBar = true;
        bool enableArmorBar = true;
        bool enableName = true;
        bool enableWeapon = true;
        bool enableDistance = true;
        bool enableSnapline = false;
        bool enableGlow = true;

        // Visuals 2.0 & 3D Orbital Extensions
        bool enable3DBox = false;
        bool enableGlowOutline = false;
        bool enableBarrelRay = true;
        bool enableAcousticWaves = false;
        bool isFriendly = false;
        int  stance = 0; // 0=Standing, 1=Crouching, 2=Scoped, 3=Jumping, 4=Defusing

        float orbitYaw = 0.0f;
        float orbitPitch = 0.0f;
        float health = 85.0f;
        float armor = 60.0f;
        float distance = 28.5f;

        std::string playerName = "Target_Bot01";
        std::string weaponName = "Vandal [25/75]";
    };

    class ESPPreview {
    public:
        static void Render(const char* str_id, const ImVec2& size, ESPSettings& settings);
    };

} // namespace Solar::Game
