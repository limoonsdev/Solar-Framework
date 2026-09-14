#include "solar/ui/card.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/shadow_caster.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/icons/vector_icons.hpp"
#include <imgui_internal.h>

namespace Solar::UI {

    bool Card::Begin(const char* str_id, const char* title, const ImVec2& size, const char* icon) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 avail = ImGui::GetContentRegionAvail();
        float w = (size.x > 0.0f) ? size.x : avail.x;
        float h = (size.y > 0.0f) ? size.y : 490.0f;

        float rounding = 10.0f;
        float headerH = 42.0f;

        ImDrawList* draw = ImGui::GetWindowDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();

        ImVec2 cardMin = p;
        ImVec2 cardMax = ImVec2(p.x + w, p.y + h);

        // 1. Layered Ambient Drop Shadow
        Render::ShadowCaster::DrawShadow(draw, cardMin, cardMax, 16.0f, rounding, Color(0, 0, 0, 0.45f), ImVec2(0, 4.0f));

        // 2. Obsidian Glass Body
        draw->AddRectFilled(cardMin, cardMax, pal.Card.ToU32(), rounding);

        // 3. Header Background Strip
        ImVec2 headerMax = ImVec2(cardMax.x, cardMin.y + headerH);
        draw->AddRectFilled(cardMin, headerMax, pal.Header.WithAlpha(0.60f).ToU32(), rounding, ImDrawFlags_RoundCornersTop);
        draw->AddLine(ImVec2(cardMin.x, cardMin.y + headerH), ImVec2(cardMax.x, cardMin.y + headerH), pal.Border.ToU32(), 1.0f);

        // 4. Top Specular Glass Reflection Line
        draw->AddLine(ImVec2(cardMin.x + rounding, cardMin.y + 0.5f),
                      ImVec2(cardMax.x - rounding, cardMin.y + 0.5f),
                      IM_COL32(255, 255, 255, 30), 1.0f);

        // 5. Outer Beveled Border
        draw->AddRect(cardMin, cardMax, pal.Border.ToU32(), rounding, 0, 1.0f);

        // 6. Header Icon Box & Clean Title
        float curX = cardMin.x + 14.0f;
        if (icon) {
            ImVec2 iconBoxMin(curX, cardMin.y + 8.0f);
            ImVec2 iconBoxMax(curX + 26.0f, cardMin.y + 34.0f);

            draw->AddRectFilled(iconBoxMin, iconBoxMax, pal.Accent.WithAlpha(0.14f).ToU32(), 5.0f);
            draw->AddRect(iconBoxMin, iconBoxMax, pal.Accent.WithAlpha(0.35f).ToU32(), 5.0f, 0, 1.0f);

            ImVec2 iconSize = ImGui::CalcTextSize(icon);
            if (iconSize.x > 2.0f) {
                draw->AddText(ImVec2(iconBoxMin.x + (26.0f - iconSize.x) * 0.5f, iconBoxMin.y + (26.0f - iconSize.y) * 0.5f),
                              pal.Accent.ToU32(), icon);
            } else {
                Icons::VectorIconRenderer::DrawByGlyph(draw, icon, ImVec2(iconBoxMin.x + 13.0f, iconBoxMin.y + 13.0f), 14.0f, pal.Accent);
            }
            curX += 34.0f;
        }

        auto lv = Render::CleanLabel(title);
        float titleY = cardMin.y + (headerH - lv.size.y) * 0.5f;
        draw->AddText(ImVec2(curX, titleY), pal.TextPrimary.ToU32(), lv.textBegin, lv.textEnd);

        // 7. Child container for inner elements (border = false to eliminate ugly ImGui grey borders)
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, rounding);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 14.0f));

        bool visible = ImGui::BeginChild(str_id, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground);
        if (visible) {
            ImGui::SetCursorPosY(headerH + 12.0f);
            ImGui::SetCursorPosX(14.0f);
        }

        return visible;
    }

    void Card::End() {
        ImGui::EndChild();
        ImGui::PopStyleVar(3);
    }

} // namespace Solar::UI
