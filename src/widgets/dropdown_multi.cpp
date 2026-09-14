#include "solar/widgets/dropdown_multi.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include <cstdio>

namespace Solar::Widgets {

    bool DropdownMultiSelect(const char* label, std::vector<bool>& selections, const std::vector<std::string>& items) {
        if (selections.size() < items.size()) {
            selections.resize(items.size(), false);
        }

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        float width = ImGui::GetContentRegionAvail().x;
        float boxHeight = 32.0f;
        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImGuiID id = window->GetID(label);
        std::string popupId = std::string("##popup_") + label;

        // Label above without ## hash
        auto lv = Render::CleanLabel(label);
        if (lv.size.x > 0.0f) {
            ImGui::TextColored(pal.TextSecondary, "%.*s", (int)(lv.textEnd - lv.textBegin), lv.textBegin);
            pos = ImGui::GetCursorScreenPos();
        }

        ImRect bb(pos, ImVec2(pos.x + width, pos.y + boxHeight));
        ImGui::ItemSize(bb, g.Style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id)) return false;

        bool hovered = ImGui::ItemHoverable(bb, id, 0);
        if (hovered && ImGui::IsMouseClicked(0)) {
            Audio::PlayClick();
            ImGui::OpenPopup(popupId.c_str());
        }

        // Draw background container
        ImU32 bgCol = hovered ? ThemeManager::ToU32(pal.CardHover) : ThemeManager::ToU32(pal.Card);
        ImU32 borderCol = ThemeManager::ToU32(hovered ? pal.Accent : pal.Border);
        draw->AddRectFilled(bb.Min, bb.Max, bgCol, 6.0f);
        draw->AddRect(bb.Min, bb.Max, borderCol, 6.0f, 0, 1.0f);

        // Count selected
        int selectedCount = 0;
        std::string summary;
        for (size_t i = 0; i < items.size(); ++i) {
            if (selections[i]) {
                if (selectedCount > 0) summary += ", ";
                summary += items[i];
                selectedCount++;
            }
        }

        float textX = bb.Min.x + 10.0f;
        float textY = bb.Min.y + (boxHeight - ImGui::GetTextLineHeight()) * 0.5f;

        if (selectedCount == 0) {
            draw->AddText(ImVec2(textX, textY), ThemeManager::ToU32(pal.TextDisabled), "Select options...");
        } else {
            // Display summary text with ellipsis if too long
            float availTextW = width - 40.0f;
            ImVec2 txtSz = ImGui::CalcTextSize(summary.c_str());
            if (txtSz.x > availTextW) {
                char countBuf[32];
                snprintf(countBuf, sizeof(countBuf), "%d items selected", selectedCount);
                draw->AddText(ImVec2(textX, textY), ThemeManager::ToU32(pal.TextPrimary), countBuf);
            } else {
                draw->AddText(ImVec2(textX, textY), ThemeManager::ToU32(pal.TextPrimary), summary.c_str());
            }
        }

        // Down chevron arrow
        float arrowX = bb.Max.x - 18.0f;
        float arrowY = bb.Min.y + boxHeight * 0.5f;
        draw->AddTriangleFilled(
            ImVec2(arrowX - 4.0f, arrowY - 2.0f),
            ImVec2(arrowX + 4.0f, arrowY - 2.0f),
            ImVec2(arrowX, arrowY + 3.0f),
            ThemeManager::ToU32(pal.TextSecondary)
        );

        bool changed = false;
        ImGui::SetNextWindowPos(ImVec2(bb.Min.x, bb.Max.y + 4.0f));
        ImGui::SetNextWindowSize(ImVec2(width, 0.0f));

        if (ImGui::BeginPopup(popupId.c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
            for (size_t i = 0; i < items.size(); ++i) {
                bool selected = selections[i];
                if (ImGui::Selectable(items[i].c_str(), selected)) {
                    selections[i] = !selected;
                    changed = true;
                    Audio::PlayClick();
                }
            }
            ImGui::EndPopup();
        }

        return changed;
    }

} // namespace Solar::Widgets
