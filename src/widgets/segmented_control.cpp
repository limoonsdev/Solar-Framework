#include "solar/widgets/segmented_control.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <unordered_map>
#include <algorithm>

namespace Solar::Widgets {

    struct SegmentState {
        float currentX = 0.0f;
        float currentW = 0.0f;
        bool  initialized = false;
    };

    static std::unordered_map<ImGuiID, SegmentState> s_segmentStates;

    bool SegmentedControl(const char* label, int* selectedIndex,
                          const std::vector<std::string>& items,
                          float height) {
        if (!selectedIndex || items.empty()) return false;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        float width = ImGui::GetContentRegionAvail().x;
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImRect bb(pos, ImVec2(pos.x + width, pos.y + height));

        ImGuiID id = window->GetID(label);
        ImGui::ItemSize(bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id)) return false;

        // Background track pill
        draw->AddRectFilled(bb.Min, bb.Max, ThemeManager::ToU32(pal.Card), height * 0.5f);
        draw->AddRect(bb.Min, bb.Max, ThemeManager::ToU32(pal.Border), height * 0.5f, 0, 1.0f);

        int count = static_cast<int>(items.size());
        float segmentW = (width - 4.0f) / static_cast<float>(count);

        SegmentState& state = s_segmentStates[id];
        float targetX = bb.Min.x + 2.0f + (*selectedIndex) * segmentW;
        float targetW = segmentW;

        if (!state.initialized) {
            state.currentX = targetX;
            state.currentW = targetW;
            state.initialized = true;
        } else {
            float dt = g.IO.DeltaTime;
            state.currentX += (targetX - state.currentX) * (std::min)(1.0f, dt * 18.0f);
            state.currentW += (targetW - state.currentW) * (std::min)(1.0f, dt * 18.0f);
        }

        // Active animated indicator pill
        ImVec2 pillMin(state.currentX, bb.Min.y + 2.0f);
        ImVec2 pillMax(state.currentX + state.currentW, bb.Max.y - 2.0f);
        draw->AddRectFilled(pillMin, pillMax, ThemeManager::ToU32(pal.Accent), (height - 4.0f) * 0.5f);

        bool changed = false;

        // Interactive segments
        for (int i = 0; i < count; ++i) {
            ImVec2 segMin(bb.Min.x + 2.0f + i * segmentW, bb.Min.y + 2.0f);
            ImVec2 segMax(segMin.x + segmentW, bb.Max.y - 2.0f);

            bool hovered = ImGui::IsMouseHoveringRect(segMin, segMax);
            if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                if (*selectedIndex != i) {
                    *selectedIndex = i;
                    changed = true;
                    Audio::PlayClick();
                }
            }

            // Segment text
            ImVec2 ts = ImGui::CalcTextSize(items[i].c_str());
            ImVec2 textPos(segMin.x + (segmentW - ts.x) * 0.5f, segMin.y + (height - 4.0f - ts.y) * 0.5f);

            bool isCurrent = (*selectedIndex == i);
            ImU32 textCol = isCurrent ? IM_COL32(255, 255, 255, 255)
                                      : (hovered ? ThemeManager::ToU32(pal.TextPrimary)
                                                 : ThemeManager::ToU32(pal.TextSecondary));

            draw->AddText(textPos, textCol, items[i].c_str());
        }

        return changed;
    }

} // namespace Solar::Widgets
