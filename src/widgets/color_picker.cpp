#include "solar/widgets/color_picker.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include <imgui_internal.h>
#include <cstdio>

namespace Solar::Widgets {

    bool ColorPicker(const char* label, float col[4]) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(label);
        auto lv = Render::CleanLabel(label);

        float availX = ImGui::GetContentRegionAvail().x;
        float height = 24.0f;

        ImVec2 p = window->DC.CursorPos;
        ImRect bb(p, ImVec2(p.x + availX, p.y + height));

        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return false;

        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        // Label on left without ## hash
        draw->AddText(ImVec2(p.x, p.y + (height - lv.size.y) * 0.5f), pal.TextPrimary.ToU32(), lv.textBegin, lv.textEnd);

        // Swatch button on right
        float swatchW = 32.0f;
        float swatchH = 18.0f;
        ImVec2 swatchMin(p.x + availX - swatchW - 2.0f, p.y + (height - swatchH) * 0.5f);
        ImVec2 swatchMax(swatchMin.x + swatchW, swatchMin.y + swatchH);
        ImRect swatchBB(swatchMin, swatchMax);

        char popupId[64];
        snprintf(popupId, sizeof(popupId), "##ColorPopup_%08X", id);

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(swatchBB, id, &hovered, &held);
        if (pressed) {
            ImGui::OpenPopup(popupId);
            Audio::PlayClick();
        }

        float rounding = 4.0f;
        u32 cVal = ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], col[3]));

        // Drop shadow & swatch fill
        draw->AddRectFilled(ImVec2(swatchMin.x, swatchMin.y + 1.0f), ImVec2(swatchMax.x, swatchMax.y + 1.0f),
                            IM_COL32(0, 0, 0, 100), rounding);
        draw->AddRectFilled(swatchMin, swatchMax, cVal, rounding);

        // Top specular highlight & border
        draw->AddLine(ImVec2(swatchMin.x + 2.0f, swatchMin.y + 0.5f), ImVec2(swatchMax.x - 2.0f, swatchMin.y + 0.5f),
                      IM_COL32(255, 255, 255, 50), 1.0f);
        draw->AddRect(swatchMin, swatchMax, hovered ? pal.Accent.ToU32() : IM_COL32(255, 255, 255, 30), rounding, 0, 1.0f);

        // Custom Popup Menu
        bool changed = false;
        ImGui::SetNextWindowPos(ImVec2(swatchMax.x - 240.0f, swatchMax.y + 4.0f), ImGuiCond_Appearing);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, pal.Header.ToU32());
        ImGui::PushStyleColor(ImGuiCol_Border, pal.Border.ToU32());

        if (ImGui::BeginPopup(popupId)) {
            ImGuiColorEditFlags pickerFlags = ImGuiColorEditFlags_AlphaBar
                                            | ImGuiColorEditFlags_NoSidePreview
                                            | ImGuiColorEditFlags_NoSmallPreview;
            if (ImGui::ColorPicker4("##Picker", col, pickerFlags)) {
                changed = true;
            }
            ImGui::EndPopup();
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);

        return changed;
    }

} // namespace Solar::Widgets
