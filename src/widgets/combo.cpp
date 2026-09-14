#include "solar/widgets/combo.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>
#include <cstdio>

namespace Solar::Widgets {

    bool Combo(const char* label, int* current_item, const char* const items[], int items_count) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(label);
        auto lv = Render::CleanLabel(label);

        float availX = ImGui::GetContentRegionAvail().x;
        float height = 30.0f;
        float totalH = (lv.size.x > 0.0f) ? (height + 22.0f) : height;

        ImVec2 p = window->DC.CursorPos;
        ImRect bb(p, ImVec2(p.x + availX, p.y + totalH));

        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return false;

        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        // Label on top without ## hash
        if (lv.size.x > 0.0f) {
            draw->AddText(p, pal.TextPrimary.ToU32(), lv.textBegin, lv.textEnd);
        }

        // Combo Box Frame
        float boxY = (lv.size.x > 0.0f) ? (p.y + 20.0f) : p.y;
        ImVec2 boxMin(p.x, boxY);
        ImVec2 boxMax(p.x + availX - 2.0f, boxY + height);
        ImRect boxBB(boxMin, boxMax);

        char popupId[64];
        snprintf(popupId, sizeof(popupId), "##ComboPopup_%08X", id);

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(boxBB, id, &hovered, &held);
        if (pressed) {
            ImGui::OpenPopup(popupId);
            Audio::PlayClick();
        }

        float rounding = 6.0f;
        u32 bgCol = hovered ? pal.CardHover.ToU32() : pal.Card.ToU32();
        draw->AddRectFilled(boxMin, boxMax, bgCol, rounding);
        Render::ImGuiExt::DrawSpecularEdge(draw, boxMin, boxMax, IM_COL32(255, 255, 255, 18), rounding, 1.0f);
        Render::ImGuiExt::AddSmoothBorder(draw, boxMin, boxMax, (hovered ? pal.Accent : pal.Border).ToU32(), rounding, 1.0f);

        // Current item text (optically centered)
        const char* previewText = (*current_item >= 0 && *current_item < items_count) ? items[*current_item] : "";
        ImVec2 textSize = ImGui::CalcTextSize(previewText);
        float textY = boxMin.y + (height - textSize.y) * 0.5f - 0.5f;
        draw->AddText(ImVec2(boxMin.x + 10.0f, textY), pal.TextPrimary.ToU32(), previewText);

        // Chevron arrow indicator
        ImVec2 chevronCenter(boxMax.x - 14.0f, boxMin.y + height * 0.5f);
        float arrowSize = 4.0f;
        draw->AddTriangleFilled(
            ImVec2(chevronCenter.x - arrowSize, chevronCenter.y - arrowSize * 0.5f),
            ImVec2(chevronCenter.x + arrowSize, chevronCenter.y - arrowSize * 0.5f),
            ImVec2(chevronCenter.x, chevronCenter.y + arrowSize * 0.8f),
            hovered ? pal.Accent.ToU32() : pal.TextSecondary.ToU32()
        );

        // Custom Popup Menu
        bool changed = false;
        ImGui::SetNextWindowPos(ImVec2(boxMin.x, boxMax.y + 3.0f));
        ImGui::SetNextWindowSize(ImVec2(boxMax.x - boxMin.x, 0.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, pal.Header.ToU32());
        ImGui::PushStyleColor(ImGuiCol_Border, pal.Border.ToU32());

        if (ImGui::BeginPopup(popupId, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
            for (int i = 0; i < items_count; ++i) {
                bool isSelected = (*current_item == i);
                ImGui::PushID(i);
                if (ImGui::Selectable(items[i], isSelected, 0, ImVec2(0, 24.0f))) {
                    *current_item = i;
                    changed = true;
                    Audio::PlayClick();
                }
                ImGui::PopID();
            }
            ImGui::EndPopup();
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);

        return changed;
    }

} // namespace Solar::Widgets
