#include "solar/game/recoil_visualizer.hpp"
#include "solar/theme/theme_manager.hpp"
#include <cmath>
#include <cstdio>

namespace Solar::Game {

    const char* RecoilVisualizer::GetWeaponName(RecoilWeapon w) {
        switch (w) {
            case RecoilWeapon::AK47:    return "AK-47 (7.62mm Heavy)";
            case RecoilWeapon::M4A4:    return "M4A4 (5.56mm Tactical)";
            case RecoilWeapon::Vandal:  return "Vandal (7.62mm High-Recoil)";
            case RecoilWeapon::Phantom: return "Phantom (Silenced SpecOps)";
            default:                    return "Unknown";
        }
    }

    static std::vector<ImVec2> GetRecoilOffsets(RecoilWeapon w) {
        // Normalized relative offsets (X, Y)
        switch (w) {
            case RecoilWeapon::AK47:
                return {
                    { 0.0f, 0.0f },    { 0.0f, -0.15f },  { -0.02f, -0.32f }, { -0.04f, -0.48f },
                    { -0.05f, -0.62f }, { -0.08f, -0.74f }, { -0.12f, -0.82f }, { -0.18f, -0.88f },
                    { -0.24f, -0.90f }, { -0.30f, -0.89f }, { -0.26f, -0.88f }, { -0.15f, -0.88f },
                    { 0.02f, -0.87f },  { 0.18f, -0.86f },  { 0.28f, -0.85f },  { 0.32f, -0.84f },
                    { 0.34f, -0.83f },  { 0.28f, -0.84f },  { 0.15f, -0.85f },  { -0.02f, -0.86f }
                };
            case RecoilWeapon::M4A4:
                return {
                    { 0.0f, 0.0f },    { 0.0f, -0.12f },  { 0.02f, -0.25f },  { 0.03f, -0.38f },
                    { 0.04f, -0.50f },  { 0.05f, -0.60f }, { 0.07f, -0.68f },  { 0.12f, -0.74f },
                    { 0.18f, -0.76f },  { 0.22f, -0.75f }, { 0.18f, -0.74f },  { 0.08f, -0.73f },
                    { -0.05f, -0.72f }, { -0.18f, -0.71f }, { -0.24f, -0.70f }, { -0.26f, -0.69f }
                };
            default:
                return {
                    { 0.0f, 0.0f },    { 0.0f, -0.14f },  { -0.01f, -0.30f }, { -0.03f, -0.45f },
                    { -0.06f, -0.58f }, { -0.10f, -0.70f }, { -0.16f, -0.79f }, { -0.22f, -0.84f },
                    { -0.25f, -0.85f }, { -0.18f, -0.84f }, { -0.04f, -0.83f }, { 0.12f, -0.82f },
                    { 0.24f, -0.81f },  { 0.30f, -0.80f },  { 0.26f, -0.79f },  { 0.14f, -0.78f }
                };
        }
    }

    void RecoilVisualizer::Render(const char* str_id, const ImVec2& size, RecoilWeapon weapon,
                                 int activeBullet, bool showCompensation, Color bulletColor) {
        ImGui::BeginChild(str_id, size, false, ImGuiWindowFlags_NoScrollbar);
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 sz = ImGui::GetWindowSize();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        // 1. Dark Backdrop with Crosshair Grid
        draw->AddRectFilled(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), IM_COL32(11, 12, 16, 245), 6.0f);
        draw->AddRect(pos, ImVec2(pos.x + sz.x, pos.y + sz.y), IM_COL32(255, 255, 255, 18), 6.0f);

        ImVec2 origin(pos.x + sz.x * 0.5f, pos.y + sz.y * 0.78f);

        // Grid lines
        draw->AddLine(ImVec2(pos.x, origin.y), ImVec2(pos.x + sz.x, origin.y), IM_COL32(255, 255, 255, 14), 1.0f);
        draw->AddLine(ImVec2(origin.x, pos.y), ImVec2(origin.x, pos.y + sz.y), IM_COL32(255, 255, 255, 14), 1.0f);

        // Center origin cross
        draw->AddCircle(origin, 4.0f, pal.Accent.WithAlpha(0.6f).ToU32(), 16, 1.2f);

        auto offsets = GetRecoilOffsets(weapon);
        float scale = sz.y * 0.75f;

        // 2. Draw Recoil Spray Path
        size_t count = offsets.size();
        for (size_t i = 1; i < count; ++i) {
            ImVec2 p1(origin.x + offsets[i - 1].x * scale, origin.y + offsets[i - 1].y * scale);
            ImVec2 p2(origin.x + offsets[i].x * scale, origin.y + offsets[i].y * scale);

            u32 lineCol = bulletColor.WithAlpha(0.40f).ToU32();
            draw->AddLine(p1, p2, lineCol, 1.6f);
        }

        // 3. Draw Counter-Recoil Compensation Curve
        if (showCompensation) {
            for (size_t i = 1; i < count; ++i) {
                ImVec2 cp1(origin.x - offsets[i - 1].x * scale, origin.y - offsets[i - 1].y * scale * 0.85f);
                ImVec2 cp2(origin.x - offsets[i].x * scale, origin.y - offsets[i].y * scale * 0.85f);

                u32 compCol = IM_COL32(60, 180, 255, 130);
                draw->AddLine(cp1, cp2, compCol, 1.4f);
            }
        }

        // 4. Draw Bullet Nodes
        for (size_t i = 0; i < count; ++i) {
            ImVec2 p(origin.x + offsets[i].x * scale, origin.y + offsets[i].y * scale);
            bool isActive = (activeBullet >= 0 && static_cast<size_t>(activeBullet) == i);

            float r = isActive ? 5.5f : 3.2f;
            u32 nodeCol = isActive ? IM_COL32(255, 255, 255, 255) : bulletColor.ToU32();

            draw->AddCircleFilled(p, r, nodeCol, 16);
            if (isActive) {
                draw->AddCircle(p, r + 2.5f, pal.Accent.ToU32(), 16, 1.5f);
            }

            // Bullet sequence numbers on key bullets
            if (i == 0 || i == 4 || i == 9 || i == 14 || i == count - 1) {
                char buf[8];
                snprintf(buf, sizeof(buf), "#%d", static_cast<int>(i + 1));
                draw->AddText(ImVec2(p.x + 6.0f, p.y - 7.0f), IM_COL32(255, 255, 255, 180), buf);
            }
        }

        // Title and Legend
        draw->AddText(ImVec2(pos.x + 10.0f, pos.y + 8.0f), pal.Accent.ToU32(), GetWeaponName(weapon));
        if (showCompensation) {
            draw->AddText(ImVec2(pos.x + 10.0f, pos.y + 24.0f), IM_COL32(60, 180, 255, 200), "Cyan: Mouse Pull-Down Compensation Path");
        }

        ImGui::EndChild();
    }

} // namespace Solar::Game
