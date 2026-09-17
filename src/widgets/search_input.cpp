#include "solar/widgets/search_input.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/anim/animation_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/shadow_caster.hpp"
#include "solar/icons.hpp"
#include "solar/font_awesome.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <algorithm>

namespace Solar::Widgets {

    bool SearchInput(const char* label, char* buffer, size_t bufferSize, const char* hint) {
        if (!buffer || bufferSize == 0) return false;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        float width = ImGui::GetContentRegionAvail().x;
        float height = 34.0f;
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImRect bb(pos, ImVec2(pos.x + width, pos.y + height));

        ImGuiID id = window->GetID(label);

        // 1. Draw custom luxury obsidian glass background
        float rounding = 6.0f;
        draw->AddRectFilled(bb.Min, bb.Max, pal.Card.ToU32(), rounding);
        Render::ImGuiExt::DrawSpecularEdge(draw, bb.Min, bb.Max, IM_COL32(255, 255, 255, 20), rounding, 1.0f);

        // 2. Optical vertical padding calculation
        float textLineH = ImGui::GetTextLineHeight();
        float padY = (height - textLineH) * 0.5f;

        // Custom styling for transparent inner InputText
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(34.0f, padY));

        ImGui::SetNextItemWidth(width);
        bool changed = ImGui::InputTextWithHint(label, hint ? hint : "Search...", buffer, bufferSize);

        bool active = ImGui::IsItemActive();
        bool hovered = ImGui::IsItemHovered();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);

        // 3. Smooth focus / hover animation & border
        f32 focusAnim = Anim::AnimationManager::Get().Transition(id, active, 16.0f);
        f32 hoverAnim = Anim::AnimationManager::Get().Transition(id + 1, hovered, 14.0f);

        if (focusAnim > 0.01f) {
            // Radiant glow halo when focused
            draw->AddRect(ImVec2(bb.Min.x - 1.0f, bb.Min.y - 1.0f), ImVec2(bb.Max.x + 1.0f, bb.Max.y + 1.0f),
                          pal.Accent.WithAlpha(0.24f * focusAnim).ToU32(), rounding + 1.0f, 0, 1.5f);
            Render::ImGuiExt::AddSmoothBorder(draw, bb.Min, bb.Max, pal.Accent.WithAlpha(0.85f * focusAnim).ToU32(), rounding, 1.2f);
        } else {
            u32 borderCol = hovered ? pal.Accent.WithAlpha(0.45f * hoverAnim).ToU32() : pal.Border.ToU32();
            Render::ImGuiExt::AddSmoothBorder(draw, bb.Min, bb.Max, borderCol, rounding, 1.0f);
        }

        // 4. Centered Search Icon on left
        ImVec2 iconCenter(pos.x + 18.0f, pos.y + height * 0.5f);
        u32 iconCol = active ? pal.Accent.ToU32() : (hovered ? pal.TextPrimary.ToU32() : pal.TextDisabled.ToU32());
        IconRenderer::DrawIcon(draw, IconType::Search, iconCenter, 10.0f, iconCol, 1.4f);

        // 5. Right-hand accessory: [Ctrl K] keycap or [x] clear button
        if (buffer[0] != '\0') {
            // Interactive Clear Button
            ImVec2 clearPos(pos.x + width - 18.0f, pos.y + height * 0.5f);
            bool hoverX = ImGui::IsMouseHoveringRect(ImVec2(clearPos.x - 9, clearPos.y - 9), ImVec2(clearPos.x + 9, clearPos.y + 9));

            if (hoverX) {
                draw->AddCircleFilled(clearPos, 8.0f, pal.CardHover.ToU32(), 16);
                Render::ImGuiExt::AddSmoothBorder(draw, ImVec2(clearPos.x - 8, clearPos.y - 8), ImVec2(clearPos.x + 8, clearPos.y + 8),
                                                  pal.Accent.ToU32(), 8.0f, 1.0f);
            }

            ImVec2 xSz = ImGui::CalcTextSize(ICON_FA_XMARK);
            if (xSz.x > 2.0f) {
                draw->AddText(ImVec2(clearPos.x - xSz.x * 0.5f, clearPos.y - xSz.y * 0.5f),
                              hoverX ? pal.TextPrimary.ToU32() : pal.TextDisabled.ToU32(), ICON_FA_XMARK);
            } else {
                draw->AddText(ImVec2(clearPos.x - 3.5f, clearPos.y - 6.5f),
                              hoverX ? pal.TextPrimary.ToU32() : pal.TextDisabled.ToU32(), "x");
            }

            if (hoverX && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                buffer[0] = '\0';
                changed = true;
                Audio::PlayClick();
            }
        } else {
            // Sleek Spotlight [Ctrl K] badge
            const char* shortcutText = "Ctrl K";
            ImVec2 scSize = ImGui::CalcTextSize(shortcutText);
            float badgeW = scSize.x + 10.0f;
            float badgeH = 18.0f;
            ImVec2 bMin(pos.x + width - badgeW - 10.0f, pos.y + (height - badgeH) * 0.5f);
            ImVec2 bMax(bMin.x + badgeW, bMin.y + badgeH);

            draw->AddRectFilled(bMin, bMax, pal.Header.WithAlpha(0.85f).ToU32(), 4.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, bMin, bMax, pal.Border.WithAlpha(0.60f).ToU32(), 4.0f, 1.0f);
            draw->AddText(ImVec2(bMin.x + 5.0f, bMin.y + (badgeH - scSize.y) * 0.5f - 0.5f),
                          pal.TextDisabled.WithAlpha(0.80f).ToU32(), shortcutText);
        }

        return changed;
    }

} // namespace Solar::Widgets
