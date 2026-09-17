#include "solar/widgets/input_text.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>
#include <cstdio>

namespace Solar::Widgets {

    bool InputText(const char* label, char* buf, size_t buf_size, const char* placeholder) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        const auto& pal = ThemeManager::Get().GetPalette();
        auto lv = Render::CleanLabel(label);

        // If label is provided and doesn't start with ##, render it above the input box with clean spacing
        bool hasVisibleLabel = (lv.size.x > 0.0f && !(label[0] == '#' && label[1] == '#'));
        if (hasVisibleLabel) {
            ImGui::PushStyleColor(ImGuiCol_Text, pal.TextSecondary.ToVec4());
            ImGui::TextUnformatted(lv.textBegin, lv.textEnd);
            ImGui::PopStyleColor();
            ImGui::Spacing();
        }

        float availX = ImGui::GetContentRegionAvail().x;
        ImGui::SetNextItemWidth(availX);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, pal.Background.WithAlpha(0.85f).ToVec4());
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, pal.CardHover.ToVec4());
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, pal.CardHover.ToVec4());
        ImGui::PushStyleColor(ImGuiCol_Text, pal.TextPrimary.ToVec4());
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, pal.TextDisabled.ToVec4());
        ImGui::PushStyleColor(ImGuiCol_Border, pal.Border.WithAlpha(0.70f).ToVec4());

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));

        char hiddenId[128];
        snprintf(hiddenId, sizeof(hiddenId), "##Input_%s", label);

        bool modified = ImGui::InputTextWithHint(hiddenId, placeholder ? placeholder : "", buf, buf_size, 0);

        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 inMin = ImGui::GetItemRectMin();
        ImVec2 inMax = ImGui::GetItemRectMax();
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();

        if (active) {
            Render::ImGuiExt::AddSmoothBorder(draw, inMin, inMax, pal.Accent.ToU32(), 6.0f, 1.2f);
        } else if (hovered) {
            Render::ImGuiExt::AddSmoothBorder(draw, inMin, inMax, pal.Accent.WithAlpha(0.50f).ToU32(), 6.0f, 1.0f);
        }

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(6);

        ImGui::Spacing();
        return modified;
    }

} // namespace Solar::Widgets
