#include "solar/widgets/number_stepper.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>
#include <cstdio>
#include <algorithm>

namespace Solar::Widgets {

    bool NumberStepper(const char* label, int* value, int minVal, int maxVal, int step) {
        if (!value) return false;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        float width = ImGui::GetContentRegionAvail().x;
        float height = 30.0f;
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImRect bb(pos, ImVec2(pos.x + width, pos.y + height));

        ImGuiID id = window->GetID(label);
        ImGui::ItemSize(bb, g.Style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id)) return false;

        auto lv = Render::CleanLabel(label);
        draw->AddText(ImVec2(pos.x, pos.y + (height - lv.size.y) * 0.5f),
                      ThemeManager::ToU32(pal.TextPrimary), lv.textBegin, lv.textEnd);

        // Control box on the right
        float controlW = 110.0f;
        float btnW = 26.0f;
        ImVec2 boxMin(pos.x + width - controlW, pos.y);
        ImVec2 boxMax(pos.x + width, pos.y + height);

        draw->AddRectFilled(boxMin, boxMax, ThemeManager::ToU32(pal.Card), 4.0f);
        Render::ImGuiExt::AddSmoothBorder(draw, boxMin, boxMax, ThemeManager::ToU32(pal.Border), 4.0f, 1.0f);

        // Decrement button
        ImRect decBb(boxMin, ImVec2(boxMin.x + btnW, boxMax.y));
        bool decHover = ImGui::IsMouseHoveringRect(decBb.Min, decBb.Max);
        if (decHover) {
            draw->AddRectFilled(decBb.Min, decBb.Max, ThemeManager::ToU32(pal.CardHover), 4.0f);
        }
        draw->AddText(ImVec2(decBb.Min.x + 9, decBb.Min.y + 6), ThemeManager::ToU32(pal.TextPrimary), "-");

        // Increment button
        ImRect incBb(ImVec2(boxMax.x - btnW, boxMin.y), boxMax);
        bool incHover = ImGui::IsMouseHoveringRect(incBb.Min, incBb.Max);
        if (incHover) {
            draw->AddRectFilled(incBb.Min, incBb.Max, ThemeManager::ToU32(pal.CardHover), 4.0f);
        }
        draw->AddText(ImVec2(incBb.Min.x + 8, incBb.Min.y + 6), ThemeManager::ToU32(pal.TextPrimary), "+");

        // Value text centered
        char valBuf[32];
        snprintf(valBuf, sizeof(valBuf), "%d", *value);
        ImVec2 vTs = ImGui::CalcTextSize(valBuf);
        draw->AddText(ImVec2(boxMin.x + btnW + (controlW - btnW * 2.0f - vTs.x) * 0.5f, boxMin.y + (height - vTs.y) * 0.5f),
                      ThemeManager::ToU32(pal.Accent), valBuf);

        bool changed = false;
        if (decHover && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (*value > minVal) {
                *value = (std::max)(minVal, *value - step);
                changed = true;
                Audio::PlayClick();
            }
        }
        if (incHover && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (*value < maxVal) {
                *value = (std::min)(maxVal, *value + step);
                changed = true;
                Audio::PlayClick();
            }
        }

        return changed;
    }

} // namespace Solar::Widgets
