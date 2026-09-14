#include "solar/icons/vector_icons.hpp"
#include "solar/font_awesome.hpp"
#include <cmath>
#include <cstring>

namespace Solar::Icons {

    void VectorIconRenderer::Draw(ImDrawList* draw, IconType icon, const ImVec2& center, float size, Color color) {
        if (!draw || size <= 1.0f) return;
        u32 col = color.ToU32();

        switch (icon) {
            case IconType::Crosshair: DrawCrosshairs(draw, center, size, col); break;
            case IconType::Eye:       DrawEye(draw, center, size, col); break;
            case IconType::Sliders:   DrawSliders(draw, center, size, col); break;
            case IconType::Palette:   DrawPalette(draw, center, size, col); break;
            case IconType::Folder:    DrawFolder(draw, center, size, col); break;
            case IconType::User:      DrawUser(draw, center, size, col); break;
            case IconType::Bell:      DrawBell(draw, center, size, col); break;
            case IconType::Shield:    DrawShield(draw, center, size, col); break;
            case IconType::Lock:      DrawLock(draw, center, size, col); break;
            case IconType::Floppy:    DrawFloppy(draw, center, size, col); break;
            case IconType::Terminal:  DrawTerminal(draw, center, size, col); break;
            case IconType::Sparkle:   DrawWand(draw, center, size, col); break;
            default:                  DrawCube(draw, center, size, col); break;
        }
    }

    bool VectorIconRenderer::DrawByGlyph(ImDrawList* draw, const char* glyphOrName, const ImVec2& center, float size, Color color) {
        if (!draw || !glyphOrName) return false;
        u32 col = color.ToU32();

        if (std::strcmp(glyphOrName, ICON_FA_CROSSHAIRS) == 0 || std::strstr(glyphOrName, "cross")) {
            DrawCrosshairs(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_EYE) == 0 || std::strstr(glyphOrName, "eye")) {
            DrawEye(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_SLIDERS) == 0 || std::strstr(glyphOrName, "slider")) {
            DrawSliders(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_PALETTE) == 0 || std::strstr(glyphOrName, "palette")) {
            DrawPalette(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_FOLDER) == 0 || std::strstr(glyphOrName, "folder")) {
            DrawFolder(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_USER) == 0 || std::strstr(glyphOrName, "user")) {
            DrawUser(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_BELL) == 0 || std::strstr(glyphOrName, "bell")) {
            DrawBell(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_SHIELD) == 0 || std::strstr(glyphOrName, "shield")) {
            DrawShield(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_LOCK) == 0 || std::strstr(glyphOrName, "lock")) {
            DrawLock(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_FLOPPY_DISK) == 0 || std::strstr(glyphOrName, "floppy")) {
            DrawFloppy(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_FINGERPRINT) == 0 || std::strstr(glyphOrName, "finger")) {
            DrawFingerprint(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_EXPAND) == 0 || std::strstr(glyphOrName, "expand")) {
            DrawExpand(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_WAND_MAGIC) == 0 || std::strstr(glyphOrName, "wand")) {
            DrawWand(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_SUN) == 0 || std::strstr(glyphOrName, "sun")) {
            DrawSun(draw, center, size, col); return true;
        }
        if (std::strcmp(glyphOrName, ICON_FA_GEAR) == 0 || std::strstr(glyphOrName, "gear")) {
            DrawGear(draw, center, size, col); return true;
        }

        // Fallback generic cube
        DrawCube(draw, center, size, col);
        return true;
    }

    void VectorIconRenderer::DrawCrosshairs(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float r = s * 0.42f;
        draw->AddCircle(c, r, col, 20, 1.4f);
        draw->AddLine(ImVec2(c.x - s * 0.5f, c.y), ImVec2(c.x - r * 0.3f, c.y), col, 1.4f);
        draw->AddLine(ImVec2(c.x + r * 0.3f, c.y), ImVec2(c.x + s * 0.5f, c.y), col, 1.4f);
        draw->AddLine(ImVec2(c.x, c.y - s * 0.5f), ImVec2(c.x, c.y - r * 0.3f), col, 1.4f);
        draw->AddLine(ImVec2(c.x, c.y + r * 0.3f), ImVec2(c.x, c.y + s * 0.5f), col, 1.4f);
    }

    void VectorIconRenderer::DrawEye(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.48f;
        float hh = s * 0.26f;

        // Top arc and bottom arc
        ImVec2 pL(c.x - hw, c.y);
        ImVec2 pR(c.x + hw, c.y);
        ImVec2 pT(c.x, c.y - hh);
        ImVec2 pB(c.x, c.y + hh);

        draw->AddBezierQuadratic(pL, pT, pR, col, 1.3f);
        draw->AddBezierQuadratic(pL, pB, pR, col, 1.3f);
        draw->AddCircleFilled(c, s * 0.16f, col, 16);
    }

    void VectorIconRenderer::DrawSliders(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.42f;
        float hh = s * 0.38f;

        float y1 = c.y - hh * 0.65f;
        float y2 = c.y;
        float y3 = c.y + hh * 0.65f;

        draw->AddLine(ImVec2(c.x - hw, y1), ImVec2(c.x + hw, y1), col, 1.2f);
        draw->AddLine(ImVec2(c.x - hw, y2), ImVec2(c.x + hw, y2), col, 1.2f);
        draw->AddLine(ImVec2(c.x - hw, y3), ImVec2(c.x + hw, y3), col, 1.2f);

        draw->AddCircleFilled(ImVec2(c.x - hw * 0.2f, y1), 2.2f, col);
        draw->AddCircleFilled(ImVec2(c.x + hw * 0.4f, y2), 2.2f, col);
        draw->AddCircleFilled(ImVec2(c.x - hw * 0.5f, y3), 2.2f, col);
    }

    void VectorIconRenderer::DrawPalette(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float r = s * 0.42f;
        draw->AddCircle(c, r, col, 24, 1.3f);
        draw->AddCircleFilled(ImVec2(c.x - r * 0.45f, c.y - r * 0.2f), 1.8f, col);
        draw->AddCircleFilled(ImVec2(c.x, c.y - r * 0.5f), 1.8f, col);
        draw->AddCircleFilled(ImVec2(c.x + r * 0.45f, c.y - r * 0.2f), 1.8f, col);
        draw->AddCircleFilled(ImVec2(c.x + r * 0.3f, c.y + r * 0.4f), 2.4f, col); // thumb hole
    }

    void VectorIconRenderer::DrawFolder(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.44f;
        float hh = s * 0.34f;
        ImVec2 min(c.x - hw, c.y - hh);
        ImVec2 max(c.x + hw, c.y + hh);

        // Tab
        draw->AddRectFilled(min, ImVec2(c.x - hw * 0.2f, min.y + 4.0f), col, 1.5f);
        // Body
        draw->AddRect(ImVec2(min.x, min.y + 3.0f), max, col, 2.0f, 0, 1.3f);
    }

    void VectorIconRenderer::DrawUser(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float headR = s * 0.18f;
        ImVec2 headC(c.x, c.y - s * 0.18f);
        draw->AddCircle(headC, headR, col, 16, 1.3f);

        ImVec2 shoulderL(c.x - s * 0.42f, c.y + s * 0.42f);
        ImVec2 shoulderR(c.x + s * 0.42f, c.y + s * 0.42f);
        ImVec2 neckC(c.x, c.y + s * 0.08f);
        draw->AddBezierQuadratic(shoulderL, neckC, shoulderR, col, 1.3f);
    }

    void VectorIconRenderer::DrawBell(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.38f;
        float topY = c.y - s * 0.38f;
        float botY = c.y + s * 0.24f;

        draw->AddCircleFilled(ImVec2(c.x, topY), 1.8f, col);
        draw->AddBezierQuadratic(ImVec2(c.x - hw, botY), ImVec2(c.x - hw * 0.4f, topY + 2.0f), ImVec2(c.x, topY + 2.0f), col, 1.3f);
        draw->AddBezierQuadratic(ImVec2(c.x + hw, botY), ImVec2(c.x + hw * 0.4f, topY + 2.0f), ImVec2(c.x, topY + 2.0f), col, 1.3f);
        draw->AddLine(ImVec2(c.x - hw, botY), ImVec2(c.x + hw, botY), col, 1.3f);
        draw->AddCircleFilled(ImVec2(c.x, botY + 3.0f), 2.2f, col);
    }

    void VectorIconRenderer::DrawShield(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.40f;
        float topY = c.y - s * 0.42f;
        float midY = c.y;
        float tipY = c.y + s * 0.45f;

        ImVec2 pT(c.x, topY);
        ImVec2 pTL(c.x - hw, topY + 2.0f);
        ImVec2 pTR(c.x + hw, topY + 2.0f);
        ImVec2 pML(c.x - hw, midY);
        ImVec2 pMR(c.x + hw, midY);
        ImVec2 pB(c.x, tipY);

        draw->AddLine(pTL, pTR, col, 1.3f);
        draw->AddLine(pTL, pML, col, 1.3f);
        draw->AddLine(pTR, pMR, col, 1.3f);
        draw->AddBezierQuadratic(pML, ImVec2(c.x - hw * 0.6f, tipY - 4.0f), pB, col, 1.3f);
        draw->AddBezierQuadratic(pMR, ImVec2(c.x + hw * 0.6f, tipY - 4.0f), pB, col, 1.3f);
    }

    void VectorIconRenderer::DrawGear(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float rInner = s * 0.18f;
        float rOuter = s * 0.38f;
        draw->AddCircle(c, rInner, col, 16, 1.2f);
        draw->AddCircle(c, rOuter * 0.8f, col, 24, 1.2f);

        for (int i = 0; i < 6; ++i) {
            float a = static_cast<float>(i) * (3.14159265f / 3.0f);
            ImVec2 p1(c.x + std::cos(a) * (rOuter * 0.7f), c.y + std::sin(a) * (rOuter * 0.7f));
            ImVec2 p2(c.x + std::cos(a) * rOuter, c.y + std::sin(a) * rOuter);
            draw->AddLine(p1, p2, col, 2.2f);
        }
    }

    void VectorIconRenderer::DrawKey(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float r = s * 0.22f;
        ImVec2 ringC(c.x - s * 0.24f, c.y);
        draw->AddCircle(ringC, r, col, 16, 1.3f);
        draw->AddLine(ImVec2(ringC.x + r, c.y), ImVec2(c.x + s * 0.44f, c.y), col, 1.4f);
        draw->AddLine(ImVec2(c.x + s * 0.26f, c.y), ImVec2(c.x + s * 0.26f, c.y + 4.0f), col, 1.4f);
        draw->AddLine(ImVec2(c.x + s * 0.42f, c.y), ImVec2(c.x + s * 0.42f, c.y + 5.0f), col, 1.4f);
    }

    void VectorIconRenderer::DrawLock(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.38f;
        float hh = s * 0.26f;
        ImVec2 bodyMin(c.x - hw, c.y - hh * 0.4f);
        ImVec2 bodyMax(c.x + hw, c.y + hh * 1.6f);

        // Body
        draw->AddRectFilled(bodyMin, bodyMax, col, 2.5f);

        // Shackle
        float shackleR = hw * 0.65f;
        ImVec2 shackleC(c.x, bodyMin.y);
        draw->AddBezierQuadratic(ImVec2(c.x - shackleR, bodyMin.y),
                                ImVec2(c.x - shackleR, bodyMin.y - shackleR * 1.6f),
                                ImVec2(c.x, bodyMin.y - shackleR * 1.6f), col, 1.4f);
        draw->AddBezierQuadratic(ImVec2(c.x + shackleR, bodyMin.y),
                                ImVec2(c.x + shackleR, bodyMin.y - shackleR * 1.6f),
                                ImVec2(c.x, bodyMin.y - shackleR * 1.6f), col, 1.4f);
    }

    void VectorIconRenderer::DrawFloppy(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.42f;
        float hh = s * 0.42f;
        ImVec2 min(c.x - hw, c.y - hh);
        ImVec2 max(c.x + hw, c.y + hh);

        draw->AddRect(min, max, col, 2.0f, 0, 1.3f);
        // Top slider
        draw->AddRectFilled(ImVec2(min.x + 3.0f, min.y), ImVec2(max.x - 3.0f, min.y + hh * 0.7f), col, 1.0f);
    }

    void VectorIconRenderer::DrawFire(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.35f;
        float topY = c.y - s * 0.48f;
        float botY = c.y + s * 0.42f;

        draw->AddBezierQuadratic(ImVec2(c.x, topY), ImVec2(c.x - hw * 1.4f, c.y), ImVec2(c.x, botY), col, 1.4f);
        draw->AddBezierQuadratic(ImVec2(c.x, topY), ImVec2(c.x + hw * 1.4f, c.y), ImVec2(c.x, botY), col, 1.4f);
        draw->AddCircleFilled(ImVec2(c.x, c.y + s * 0.15f), s * 0.15f, col);
    }

    void VectorIconRenderer::DrawSun(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float r = s * 0.22f;
        draw->AddCircleFilled(c, r, col, 16);
        for (int i = 0; i < 8; ++i) {
            float a = static_cast<float>(i) * (3.14159265f / 4.0f);
            ImVec2 p1(c.x + std::cos(a) * (r + 2.0f), c.y + std::sin(a) * (r + 2.0f));
            ImVec2 p2(c.x + std::cos(a) * (r + 5.0f), c.y + std::sin(a) * (r + 5.0f));
            draw->AddLine(p1, p2, col, 1.4f);
        }
    }

    void VectorIconRenderer::DrawWand(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.40f;
        draw->AddLine(ImVec2(c.x - hw, c.y + hw), ImVec2(c.x + hw * 0.4f, c.y - hw * 0.4f), col, 2.0f);
        // Star tip
        draw->AddCircleFilled(ImVec2(c.x + hw * 0.55f, c.y - hw * 0.55f), 2.2f, col);
    }

    void VectorIconRenderer::DrawTerminal(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.44f;
        float hh = s * 0.35f;
        draw->AddRect(ImVec2(c.x - hw, c.y - hh), ImVec2(c.x + hw, c.y + hh), col, 2.0f, 0, 1.2f);
        // Prompt >
        draw->AddLine(ImVec2(c.x - hw + 3.0f, c.y - 3.0f), ImVec2(c.x - hw + 7.0f, c.y), col, 1.2f);
        draw->AddLine(ImVec2(c.x - hw + 7.0f, c.y), ImVec2(c.x - hw + 3.0f, c.y + 3.0f), col, 1.2f);
        // Underscore _
        draw->AddLine(ImVec2(c.x - hw + 9.0f, c.y + 3.0f), ImVec2(c.x - hw + 14.0f, c.y + 3.0f), col, 1.2f);
    }

    void VectorIconRenderer::DrawCheck(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.40f;
        ImVec2 p1(c.x - hw, c.y);
        ImVec2 p2(c.x - hw * 0.2f, c.y + hw * 0.7f);
        ImVec2 p3(c.x + hw, c.y - hw * 0.7f);
        draw->AddLine(p1, p2, col, 2.0f);
        draw->AddLine(p2, p3, col, 2.0f);
    }

    void VectorIconRenderer::DrawXmark(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.35f;
        draw->AddLine(ImVec2(c.x - hw, c.y - hw), ImVec2(c.x + hw, c.y + hw), col, 1.8f);
        draw->AddLine(ImVec2(c.x - hw, c.y + hw), ImVec2(c.x + hw, c.y - hw), col, 1.8f);
    }

    void VectorIconRenderer::DrawFingerprint(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float r = s * 0.38f;
        draw->AddCircle(c, r, col, 20, 1.2f);
        draw->AddCircle(c, r * 0.65f, col, 16, 1.2f);
        draw->AddCircle(c, r * 0.35f, col, 12, 1.2f);
    }

    void VectorIconRenderer::DrawExpand(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.40f;
        float hh = s * 0.40f;
        draw->AddLine(ImVec2(c.x - hw, c.y - hh + 4.0f), ImVec2(c.x - hw, c.y - hh), col, 1.4f);
        draw->AddLine(ImVec2(c.x - hw, c.y - hh), ImVec2(c.x - hw + 4.0f, c.y - hh), col, 1.4f);
        draw->AddLine(ImVec2(c.x + hw, c.y + hh - 4.0f), ImVec2(c.x + hw, c.y + hh), col, 1.4f);
        draw->AddLine(ImVec2(c.x + hw, c.y + hh), ImVec2(c.x + hw - 4.0f, c.y + hh), col, 1.4f);
    }

    void VectorIconRenderer::DrawSkull(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float r = s * 0.32f;
        draw->AddCircle(ImVec2(c.x, c.y - 2.0f), r, col, 16, 1.3f);
        draw->AddCircleFilled(ImVec2(c.x - r * 0.45f, c.y - 2.0f), 1.8f, col);
        draw->AddCircleFilled(ImVec2(c.x + r * 0.45f, c.y - 2.0f), 1.8f, col);
        draw->AddRect(ImVec2(c.x - r * 0.4f, c.y + 4.0f), ImVec2(c.x + r * 0.4f, c.y + 9.0f), col, 1.0f, 0, 1.2f);
    }

    void VectorIconRenderer::DrawCube(ImDrawList* draw, const ImVec2& c, float s, u32 col) {
        float hw = s * 0.38f;
        draw->AddRect(ImVec2(c.x - hw, c.y - hw), ImVec2(c.x + hw, c.y + hw), col, 2.0f, 0, 1.3f);
    }

} // namespace Solar::Icons
