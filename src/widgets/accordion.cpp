#include "solar/widgets/accordion.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include <imgui.h>
#include <imgui_internal.h>

namespace Solar::Widgets {

    static bool s_accordionOpen = false;

    bool BeginAccordion(const char* label, bool* isOpen, const char* subtitle) {
        if (!isOpen) return false;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        float width = ImGui::GetContentRegionAvail().x;
        float headerHeight = subtitle ? 44.0f : 34.0f;
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImRect bb(pos, ImVec2(pos.x + width, pos.y + headerHeight));

        ImGuiID id = window->GetID(label);
        ImGui::ItemSize(bb, g.Style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id)) return false;

        bool hovered = ImGui::ItemHoverable(bb, id, 0);
        if (hovered && ImGui::IsMouseClicked(0)) {
            *isOpen = !(*isOpen);
            Audio::PlayClick();
        }

        // Draw header background
        ImU32 bgCol = hovered ? ThemeManager::ToU32(pal.CardHover) : ThemeManager::ToU32(pal.Card);
        ImU32 borderCol = *isOpen ? ThemeManager::ToU32(pal.Accent) : ThemeManager::ToU32(pal.Border);
        float rounding = 6.0f;

        draw->AddRectFilled(bb.Min, bb.Max, bgCol, rounding);
        draw->AddRect(bb.Min, bb.Max, borderCol, rounding, 0, 1.0f);

        // Indicator chevron
        float chevronX = bb.Max.x - 20.0f;
        float chevronY = bb.Min.y + headerHeight * 0.5f;
        ImU32 iconCol = *isOpen ? ThemeManager::ToU32(pal.Accent) : ThemeManager::ToU32(pal.TextSecondary);

        if (*isOpen) {
            // Down arrow
            draw->AddTriangleFilled(
                ImVec2(chevronX - 5.0f, chevronY - 3.0f),
                ImVec2(chevronX + 5.0f, chevronY - 3.0f),
                ImVec2(chevronX, chevronY + 3.0f),
                iconCol
            );
        } else {
            // Right arrow
            draw->AddTriangleFilled(
                ImVec2(chevronX - 3.0f, chevronY - 5.0f),
                ImVec2(chevronX - 3.0f, chevronY + 5.0f),
                ImVec2(chevronX + 3.0f, chevronY),
                iconCol
            );
        }

        // Title and Subtitle text
        float textX = bb.Min.x + 12.0f;
        if (subtitle) {
            draw->AddText(ImVec2(textX, bb.Min.y + 6.0f), ThemeManager::ToU32(pal.TextPrimary), label);
            draw->AddText(ImVec2(textX, bb.Min.y + 24.0f), ThemeManager::ToU32(pal.TextSecondary), subtitle);
        } else {
            float textY = bb.Min.y + (headerHeight - ImGui::GetTextLineHeight()) * 0.5f;
            draw->AddText(ImVec2(textX, textY), ThemeManager::ToU32(pal.TextPrimary), label);
        }

        s_accordionOpen = *isOpen;
        if (*isOpen) {
            ImGui::Indent(12.0f);
            ImGui::Spacing();
            ImGui::BeginGroup();
            return true;
        }

        return false;
    }

    void EndAccordion() {
        if (s_accordionOpen) {
            ImGui::EndGroup();
            ImGui::Spacing();
            ImGui::Unindent(12.0f);
            s_accordionOpen = false;
        }
    }

} // namespace Solar::Widgets
