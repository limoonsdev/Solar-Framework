#include "solar/icons.hpp"
#include <cmath>

namespace Solar {

    void IconRenderer::DrawIcon(ImDrawList* drawList, IconType type, const ImVec2& center, float size, ImU32 color, float thickness) {
        if (!drawList || type == IconType::None) return;

        float r = size * 0.5f;

        switch (type) {
            case IconType::Crosshair: {
                // Outer circle
                drawList->AddCircle(center, r * 0.8f, color, 24, thickness);
                // Inner dot
                drawList->AddCircleFilled(center, r * 0.2f, color);
                // 4 Cross ticks
                drawList->AddLine(ImVec2(center.x, center.y - r * 1.15f), ImVec2(center.x, center.y - r * 0.5f), color, thickness);
                drawList->AddLine(ImVec2(center.x, center.y + r * 0.5f), ImVec2(center.x, center.y + r * 1.15f), color, thickness);
                drawList->AddLine(ImVec2(center.x - r * 1.15f, center.y), ImVec2(center.x - r * 0.5f, center.y), color, thickness);
                drawList->AddLine(ImVec2(center.x + r * 0.5f, center.y), ImVec2(center.x + r * 1.15f, center.y), color, thickness);
                break;
            }

            case IconType::Eye: {
                // Eye contour (curved)
                ImVec2 left(center.x - r, center.y);
                ImVec2 right(center.x + r, center.y);
                ImVec2 topCtrl(center.x, center.y - r * 0.75f);
                ImVec2 btmCtrl(center.x, center.y + r * 0.75f);

                drawList->AddBezierQuadratic(left, topCtrl, right, color, thickness);
                drawList->AddBezierQuadratic(left, btmCtrl, right, color, thickness);
                // Pupil
                drawList->AddCircleFilled(center, r * 0.35f, color);
                break;
            }

            case IconType::Sliders: {
                // Three horizontal or vertical adjustment sliders
                float gap = r * 0.65f;
                // Top line
                drawList->AddLine(ImVec2(center.x - r, center.y - gap), ImVec2(center.x + r, center.y - gap), color, thickness);
                drawList->AddCircleFilled(ImVec2(center.x - r * 0.3f, center.y - gap), thickness * 1.6f, color);

                // Mid line
                drawList->AddLine(ImVec2(center.x - r, center.y), ImVec2(center.x + r, center.y), color, thickness);
                drawList->AddCircleFilled(ImVec2(center.x + r * 0.4f, center.y), thickness * 1.6f, color);

                // Bottom line
                drawList->AddLine(ImVec2(center.x - r, center.y + gap), ImVec2(center.x + r, center.y + gap), color, thickness);
                drawList->AddCircleFilled(ImVec2(center.x - r * 0.1f, center.y + gap), thickness * 1.6f, color);
                break;
            }

            case IconType::Palette: {
                // Artist palette outline
                drawList->AddCircle(center, r * 0.85f, color, 20, thickness);
                // Swatch dots inside
                drawList->AddCircleFilled(ImVec2(center.x - r * 0.35f, center.y - r * 0.35f), thickness * 1.2f, color);
                drawList->AddCircleFilled(ImVec2(center.x + r * 0.25f, center.y - r * 0.35f), thickness * 1.2f, color);
                drawList->AddCircleFilled(ImVec2(center.x - r * 0.4f, center.y + r * 0.15f), thickness * 1.2f, color);
                drawList->AddCircleFilled(ImVec2(center.x + r * 0.35f, center.y + r * 0.25f), thickness * 1.2f, color);
                break;
            }

            case IconType::Folder: {
                // Minimalist folder
                ImVec2 tl(center.x - r * 0.9f, center.y - r * 0.6f);
                ImVec2 br(center.x + r * 0.9f, center.y + r * 0.7f);
                // Tab
                drawList->AddLine(tl, ImVec2(center.x - r * 0.2f, tl.y), color, thickness);
                drawList->AddLine(ImVec2(center.x - r * 0.2f, tl.y), ImVec2(center.x, tl.y + r * 0.3f), color, thickness);
                // Main body
                drawList->AddRect(ImVec2(tl.x, tl.y + r * 0.3f), br, color, 2.0f, 0, thickness);
                break;
            }

            case IconType::User: {
                // Head
                drawList->AddCircle(ImVec2(center.x, center.y - r * 0.35f), r * 0.42f, color, 16, thickness);
                // Shoulders arc
                ImVec2 sLeft(center.x - r * 0.85f, center.y + r * 0.85f);
                ImVec2 sCtrl(center.x, center.y + r * 0.15f);
                ImVec2 sRight(center.x + r * 0.85f, center.y + r * 0.85f);
                drawList->AddBezierQuadratic(sLeft, sCtrl, sRight, color, thickness);
                break;
            }

            case IconType::Bell: {
                // Bell dome
                ImVec2 top(center.x, center.y - r * 0.7f);
                ImVec2 left(center.x - r * 0.75f, center.y + r * 0.4f);
                ImVec2 right(center.x + r * 0.75f, center.y + r * 0.4f);
                drawList->AddBezierQuadratic(left, top, right, color, thickness);
                drawList->AddLine(left, right, color, thickness);
                // Clapper
                drawList->AddCircleFilled(ImVec2(center.x, center.y + r * 0.65f), thickness * 1.5f, color);
                break;
            }

            case IconType::Shield: {
                // Crest
                ImVec2 tl(center.x - r * 0.8f, center.y - r * 0.7f);
                ImVec2 tr(center.x + r * 0.8f, center.y - r * 0.7f);
                ImVec2 btm(center.x, center.y + r * 0.9f);
                drawList->AddLine(tl, tr, color, thickness);
                drawList->AddBezierQuadratic(tl, ImVec2(center.x - r * 0.8f, center.y + r * 0.2f), btm, color, thickness);
                drawList->AddBezierQuadratic(tr, ImVec2(center.x + r * 0.8f, center.y + r * 0.2f), btm, color, thickness);
                break;
            }

            case IconType::Check: {
                ImVec2 p1(center.x - r * 0.65f, center.y);
                ImVec2 p2(center.x - r * 0.15f, center.y + r * 0.5f);
                ImVec2 p3(center.x + r * 0.7f, center.y - r * 0.5f);
                drawList->AddLine(p1, p2, color, thickness * 1.3f);
                drawList->AddLine(p2, p3, color, thickness * 1.3f);
                break;
            }

            case IconType::Close: {
                float d = r * 0.65f;
                drawList->AddLine(ImVec2(center.x - d, center.y - d), ImVec2(center.x + d, center.y + d), color, thickness);
                drawList->AddLine(ImVec2(center.x + d, center.y - d), ImVec2(center.x - d, center.y + d), color, thickness);
                break;
            }

            case IconType::Minimize: {
                float d = r * 0.65f;
                drawList->AddLine(ImVec2(center.x - d, center.y + r * 0.3f), ImVec2(center.x + d, center.y + r * 0.3f), color, thickness);
                break;
            }

            case IconType::ChevronDown: {
                float w = r * 0.6f;
                float h = r * 0.35f;
                drawList->AddLine(ImVec2(center.x - w, center.y - h), ImVec2(center.x, center.y + h), color, thickness);
                drawList->AddLine(ImVec2(center.x, center.y + h), ImVec2(center.x + w, center.y - h), color, thickness);
                break;
            }

            case IconType::ChevronRight: {
                float w = r * 0.35f;
                float h = r * 0.6f;
                drawList->AddLine(ImVec2(center.x - w, center.y - h), ImVec2(center.x + w, center.y), color, thickness);
                drawList->AddLine(ImVec2(center.x + w, center.y), ImVec2(center.x - w, center.y + h), color, thickness);
                break;
            }

            case IconType::Search: {
                float cr = r * 0.55f;
                ImVec2 circleCenter(center.x - r * 0.2f, center.y - r * 0.2f);
                drawList->AddCircle(circleCenter, cr, color, 18, thickness);
                drawList->AddLine(ImVec2(circleCenter.x + cr * 0.707f, circleCenter.y + cr * 0.707f),
                                  ImVec2(center.x + r * 0.8f, center.y + r * 0.8f), color, thickness * 1.2f);
                break;
            }

            case IconType::Keyboard: {
                ImVec2 tl(center.x - r * 0.9f, center.y - r * 0.5f);
                ImVec2 br(center.x + r * 0.9f, center.y + r * 0.5f);
                drawList->AddRect(tl, br, color, 3.0f, 0, thickness);
                // Key lines
                drawList->AddLine(ImVec2(center.x - r * 0.5f, center.y - r * 0.1f), ImVec2(center.x - r * 0.3f, center.y - r * 0.1f), color, thickness);
                drawList->AddLine(ImVec2(center.x - r * 0.1f, center.y - r * 0.1f), ImVec2(center.x + r * 0.1f, center.y - r * 0.1f), color, thickness);
                drawList->AddLine(ImVec2(center.x + r * 0.3f, center.y - r * 0.1f), ImVec2(center.x + r * 0.5f, center.y - r * 0.1f), color, thickness);
                drawList->AddLine(ImVec2(center.x - r * 0.4f, center.y + r * 0.2f), ImVec2(center.x + r * 0.4f, center.y + r * 0.2f), color, thickness);
                break;
            }

            case IconType::Sparkle:
            case IconType::Sun: {
                // Glowing star / Sun rays
                drawList->AddCircleFilled(center, r * 0.35f, color);
                for (int i = 0; i < 8; i++) {
                    float angle = i * (3.14159265f / 4.0f);
                    ImVec2 p1(center.x + std::cos(angle) * (r * 0.55f), center.y + std::sin(angle) * (r * 0.55f));
                    ImVec2 p2(center.x + std::cos(angle) * (r * 0.95f), center.y + std::sin(angle) * (r * 0.95f));
                    drawList->AddLine(p1, p2, color, thickness);
                }
                break;
            }
        }
    }

} // namespace Solar
