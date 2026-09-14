#include "solar/widgets/chip_selector.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/font_awesome.hpp"
#include <imgui_internal.h>
#include <algorithm>

namespace Solar::Widgets {

    bool ChipSelector(const char* label, std::vector<bool>& selections, const std::vector<std::string>& items) {
        if (items.empty()) return false;
        if (selections.size() < items.size()) {
            selections.resize(items.size(), false);
        }

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        auto lv = Render::CleanLabel(label);
        if (lv.textBegin != lv.textEnd) {
            draw->AddText(window->DC.CursorPos, pal.TextPrimary.ToU32(), lv.textBegin, lv.textEnd);
            ImGui::Dummy(ImVec2(0, 18.0f));
        }

        bool anyChanged = false;
        float chipH = 26.0f;
        float spacing = 6.0f;
        float availW = ImGui::GetContentRegionAvail().x;
        float curX = window->DC.CursorPos.x;
        float startX = curX;
        float curY = window->DC.CursorPos.y;

        for (size_t i = 0; i < items.size(); ++i) {
            const auto& item = items[i];
            bool active = selections[i];

            char idBuf[64];
            std::snprintf(idBuf, sizeof(idBuf), "##chip_%s_%zu", label, i);
            ImGuiID id = window->GetID(idBuf);

            ImVec2 txtSz = ImGui::CalcTextSize(item.c_str());
            float chipW = txtSz.x + (active ? 28.0f : 20.0f);

            // Wrap to next line if exceeding width
            if (curX + chipW > startX + availW && curX > startX) {
                curX = startX;
                curY += chipH + spacing;
            }

            ImVec2 chipMin(curX, curY);
            ImVec2 chipMax(curX + chipW, curY + chipH);

            ImRect bb(chipMin, chipMax);
            ImGui::ItemSize(bb);
            if (!ImGui::ItemAdd(bb, id)) continue;

            bool hovered, held;
            if (ImGui::ButtonBehavior(bb, id, &hovered, &held)) {
                selections[i] = !selections[i];
                anyChanged = true;
                ImGui::MarkItemEdited(id);
                Audio::PlayClick();
            }

            u32 bgCol = active ? pal.Accent.WithAlpha(0.25f).ToU32() : (hovered ? pal.CardHover.ToU32() : pal.Header.WithAlpha(0.60f).ToU32());
            u32 borderCol = active ? pal.Accent.ToU32() : (hovered ? pal.Border.ToU32() : pal.Border.WithAlpha(0.40f).ToU32());
            u32 textCol = active ? pal.Accent.ToU32() : (hovered ? pal.TextPrimary.ToU32() : pal.TextSecondary.ToU32());

            draw->AddRectFilled(chipMin, chipMax, bgCol, 13.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, chipMin, chipMax, borderCol, 13.0f, 1.0f);

            if (active) {
                // Checkmark icon
                draw->AddText(ImVec2(chipMin.x + 8.0f, chipMin.y + (chipH - txtSz.y) * 0.5f), textCol, ICON_FA_CHECK);
                draw->AddText(ImVec2(chipMin.x + 22.0f, chipMin.y + (chipH - txtSz.y) * 0.5f), textCol, item.c_str());
            } else {
                draw->AddText(ImVec2(chipMin.x + 10.0f, chipMin.y + (chipH - txtSz.y) * 0.5f), textCol, item.c_str());
            }

            curX += chipW + spacing;
        }

        return anyChanged;
    }

} // namespace Solar::Widgets
