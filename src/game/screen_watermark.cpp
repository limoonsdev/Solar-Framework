#include "solar/game/screen_watermark.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui.h>
#include <cmath>

namespace Solar::Game {

    void ScreenWatermark::Render(const ScreenWatermarkSettings& settings) {
        if (!settings.enabled || settings.text.empty()) return;

        ImGuiIO& io = ImGui::GetIO();
        if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f) return;

        ImFont* font = Render::FontManager::Get().GetGamingFont(settings.fontPreset, true);
        if (!font) {
            font = Render::FontManager::Get().GetTitleFont();
        }
        if (!font) {
            font = ImGui::GetFont();
        }

        // Breathing opacity wave calculation
        float baseAlpha = settings.opacity;
        if (settings.animatedPulse) {
            float t = static_cast<float>(ImGui::GetTime());
            baseAlpha += 0.02f * std::sin(t * 1.8f);
            if (baseAlpha < 0.02f) baseAlpha = 0.02f;
            if (baseAlpha > 0.95f) baseAlpha = 0.95f;
        }

        // Color computation
        ImVec4 textColor;
        if (settings.useThemeColor) {
            const auto& pal = ThemeManager::Get().GetPalette();
            textColor = pal.Accent.ToVec4();
        } else {
            textColor = settings.customColor;
        }
        textColor.w = baseAlpha;
        ImU32 colU32 = ImGui::ColorConvertFloat4ToU32(textColor);

        float fontScale = (settings.scale < 0.3f) ? 0.3f : (settings.scale > 3.0f ? 3.0f : settings.scale);
        float baseFontSize = (font ? font->LegacySize : 48.0f);
        float effectiveFontSize = baseFontSize * fontScale;

        ImVec2 textSize = font ? font->CalcTextSizeA(effectiveFontSize, FLT_MAX, 0.0f, settings.text.c_str())
                               : ImGui::CalcTextSize(settings.text.c_str());

        ImDrawList* draw = ImGui::GetBackgroundDrawList();

        auto drawRotatedTextInstance = [&](ImVec2 center, float angleDeg, ImU32 col) {
            int vtx_start = draw->VtxBuffer.Size;
            ImVec2 textPos(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f);

            if (settings.effect == ScreenWatermarkEffect::OutlinedGlow) {
                // Subtle glow aura
                ImU32 glowCol = (col & 0x00FFFFFF) | (static_cast<ImU32>(((col >> 24) & 0xFF) * 0.28f) << 24);
                float glowDist = 2.5f * fontScale;
                draw->AddText(font, effectiveFontSize, ImVec2(textPos.x - glowDist, textPos.y), glowCol, settings.text.c_str());
                draw->AddText(font, effectiveFontSize, ImVec2(textPos.x + glowDist, textPos.y), glowCol, settings.text.c_str());
                draw->AddText(font, effectiveFontSize, ImVec2(textPos.x, textPos.y - glowDist), glowCol, settings.text.c_str());
                draw->AddText(font, effectiveFontSize, ImVec2(textPos.x, textPos.y + glowDist), glowCol, settings.text.c_str());
            }

            draw->AddText(font, effectiveFontSize, textPos, col, settings.text.c_str());

            // Hardware vertex buffer rotation
            if (std::abs(angleDeg) > 0.01f) {
                float rad = angleDeg * (3.14159265f / 180.0f);
                float cosA = std::cos(rad);
                float sinA = std::sin(rad);

                for (int i = vtx_start; i < draw->VtxBuffer.Size; ++i) {
                    ImDrawVert& v = draw->VtxBuffer.Data[i];
                    float rx = v.pos.x - center.x;
                    float ry = v.pos.y - center.y;
                    v.pos.x = center.x + rx * cosA - ry * sinA;
                    v.pos.y = center.y + rx * sinA + ry * cosA;
                }
            }
        };

        // Layout Render Pass
        switch (settings.layout) {
        case ScreenWatermarkLayout::CenterDiagonal: {
            ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
            drawRotatedTextInstance(center, settings.angleDegrees, colU32);
            break;
        }

        case ScreenWatermarkLayout::CenterHorizontal: {
            ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
            drawRotatedTextInstance(center, 0.0f, colU32);
            break;
        }

        case ScreenWatermarkLayout::BottomBanner: {
            ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y - textSize.y * 0.7f - 30.0f);
            drawRotatedTextInstance(center, 0.0f, colU32);
            break;
        }

        case ScreenWatermarkLayout::TopBanner: {
            ImVec2 center(io.DisplaySize.x * 0.5f, textSize.y * 0.7f + 35.0f);
            drawRotatedTextInstance(center, 0.0f, colU32);
            break;
        }

        case ScreenWatermarkLayout::TiledMatrix: {
            // Anti-leak repeating diagonal matrix across screen
            float stepX = textSize.x * 1.6f + 100.0f;
            float stepY = textSize.y * 2.2f + 80.0f;
            if (stepX < 120.0f) stepX = 120.0f;
            if (stepY < 80.0f) stepY = 80.0f;

            // Reduce opacity for tiled matrix mode so it stays subtle
            ImVec4 tiledCol = textColor;
            tiledCol.w = baseAlpha * 0.65f;
            ImU32 tiledU32 = ImGui::ColorConvertFloat4ToU32(tiledCol);

            int rowIdx = 0;
            for (float y = -stepY * 0.5f; y < io.DisplaySize.y + stepY; y += stepY) {
                float xOffset = (rowIdx % 2 == 1) ? (stepX * 0.5f) : 0.0f;
                for (float x = -stepX * 0.5f + xOffset; x < io.DisplaySize.x + stepX; x += stepX) {
                    drawRotatedTextInstance(ImVec2(x, y), settings.angleDegrees, tiledU32);
                }
                rowIdx++;
            }
            break;
        }
        }
    }

} // namespace Solar::Game
