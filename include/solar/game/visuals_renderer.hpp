#pragma once
#include "solar/core/types.hpp"
#include "solar/core/color.hpp"
#include <vector>
#include <string>
#include <utility>

namespace Solar::Game {

    enum class BoxStyle : u8 {
        Full = 0,
        Corner,
        Filled,
        CornerFilled
    };

    enum class BarPosition : u8 {
        Left = 0,
        Right,
        Top,
        Bottom
    };

    enum class SnaplineOrigin : u8 {
        ScreenBottom = 0,
        ScreenCenter,
        ScreenTop
    };

    class VisualsRenderer {
    public:
        // ==============================================================================
        // 1. Aim FOV Circles & Dynamic Target Cones
        // ==============================================================================
        static void DrawFOVCircle(ImDrawList* draw, const ImVec2& screenCenter, float radius,
                                  Color color, float thickness = 1.5f, bool enableGlow = true);

        // ==============================================================================
        // 2. 2D & 3D Bounding Boxes
        // ==============================================================================
        static void DrawBoundingBox2D(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                      BoxStyle style, Color color,
                                      Color outlineColor = Color(0.0f, 0.0f, 0.0f, 0.85f),
                                      float cornerLength = 12.0f, float thickness = 1.5f);

        static void DrawFilledBox(ImDrawList* draw, const ImVec2& min, const ImVec2& max,
                                  Color fillColor, Color borderColor, float rounding = 0.0f);

        // ==============================================================================
        // 3. Status Bars (Health, Armor, Ammo, Heat)
        // ==============================================================================
        static void DrawHealthBar(ImDrawList* draw, const ImVec2& boxMin, const ImVec2& boxMax,
                                  float currentHp, float maxHp = 100.0f,
                                  BarPosition pos = BarPosition::Left,
                                  bool segmented = false, bool showValue = true);

        static void DrawArmorBar(ImDrawList* draw, const ImVec2& boxMin, const ImVec2& boxMax,
                                 float currentArmor, float maxArmor = 100.0f,
                                 BarPosition pos = BarPosition::Left);

        static void DrawAmmoBar(ImDrawList* draw, const ImVec2& boxMin, const ImVec2& boxMax,
                                int currentAmmo, int maxAmmo,
                                BarPosition pos = BarPosition::Bottom);

        // ==============================================================================
        // 4. Skeleton Hierarchy & Bone Linkage
        // ==============================================================================
        static void DrawSkeleton(ImDrawList* draw, const std::vector<std::pair<ImVec2, ImVec2>>& bones,
                                 Color color, float thickness = 1.5f,
                                 bool drawJoints = true, Color jointColor = Color(1.0f, 1.0f, 1.0f, 0.9f));

        static void DrawHeadCircle(ImDrawList* draw, const ImVec2& headCenter, float radius,
                                   Color color, Color outlineColor = Color(0, 0, 0, 0.85f),
                                   bool filled = false);

        // ==============================================================================
        // 5. Targeting Rays & Snaplines
        // ==============================================================================
        static void DrawSnapline(ImDrawList* draw, const ImVec2& targetPos,
                                 SnaplineOrigin originType, Color color,
                                 float thickness = 1.5f, bool dashed = false);

        // ==============================================================================
        // 6. Text Elements & Tag Clouds
        // ==============================================================================
        static void DrawNameTag(ImDrawList* draw, const ImVec2& pos, const std::string& name,
                                Color color = Color(1.0f, 1.0f, 1.0f, 1.0f),
                                bool centered = true, bool dropShadow = true);

        static void DrawWeaponTag(ImDrawList* draw, const ImVec2& pos, const std::string& weaponName,
                                  int currentAmmo = -1, int maxAmmo = -1,
                                  Color color = Color(0.85f, 0.88f, 0.95f, 1.0f),
                                  bool centered = true);

        static void DrawDistanceTag(ImDrawList* draw, const ImVec2& pos, float distanceMeters,
                                    Color color = Color(1.0f, 0.70f, 0.15f, 1.0f));

        static void DrawFlagTags(ImDrawList* draw, const ImVec2& boxMax,
                                 const std::vector<std::pair<std::string, Color>>& flags);

        // ==============================================================================
        // 7. Tactical Combat HUD (Offscreen Indicator & Hitmarkers)
        // ==============================================================================
        static void DrawOffscreenIndicator(ImDrawList* draw, const ImVec2& screenCenter,
                                           float angleRadians, float screenRadius,
                                           Color color, float distanceMeters = 0.0f);

        static void DrawHitmarker(ImDrawList* draw, const ImVec2& screenCenter, float size,
                                  Color color, float progress = 1.0f, float damage = -1.0f);
    };

    // Fast class alias
    using Visuals = VisualsRenderer;

} // namespace Solar::Game
