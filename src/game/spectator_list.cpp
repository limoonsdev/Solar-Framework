#include "solar/game/spectator_list.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/shadow_caster.hpp"
#include "solar/font_awesome.hpp"
#include <imgui.h>
#include <imgui_internal.h>

namespace Solar::Game {

    void SpectatorList::Render(bool* p_open, const std::vector<std::string>& spectators) {
        if (!p_open || !(*p_open)) return;

        ImGui::SetNextWindowSize(ImVec2(210.0f, 150.0f), ImGuiCond_FirstUseEver);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

        const auto& pal = ThemeManager::Get().GetPalette();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, pal.Card.WithAlpha(0.96f).ToVec4());
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));

        if (ImGui::Begin("##SpectatorListWindow", p_open, flags)) {
            ImVec2 winPos = ImGui::GetWindowPos();
            ImVec2 winSize = ImGui::GetWindowSize();
            ImDrawList* draw = ImGui::GetWindowDrawList();

            // 1. Soft Ambient Shadow & Specular Top Sheen
            Render::ShadowCaster::DrawShadow(draw, winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
                                            14.0f, 10.0f, Color(0, 0, 0, 0.45f), ImVec2(0, 3.0f));
            Render::ImGuiExt::DrawSpecularEdge(draw, winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
                                              IM_COL32(255, 255, 255, 26), 10.0f, 1.0f);
            Render::ImGuiExt::AddSmoothBorder(draw, winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y),
                                             pal.Border.ToU32(), 10.0f, 1.0f);

            // 2. Custom Window Header (Icon + Title + Count Badge + Close)
            float headerH = 26.0f;
            draw->AddText(ImVec2(winPos.x + 12.0f, winPos.y + 12.0f), pal.Accent.ToU32(), ICON_FA_EYE);
            draw->AddText(ImVec2(winPos.x + 32.0f, winPos.y + 10.0f), pal.TextPrimary.ToU32(), "Spectators");

            // Close button [x]
            ImVec2 closePos(winPos.x + winSize.x - 22.0f, winPos.y + 8.0f);
            ImGui::SetCursorScreenPos(closePos);
            if (ImGui::InvisibleButton("##spectator_close", ImVec2(16.0f, 16.0f))) {
                *p_open = false;
                Audio::PlayClick();
            }
            bool hoverClose = ImGui::IsItemHovered();
            draw->AddText(ImVec2(closePos.x + 2.0f, closePos.y + 2.0f),
                          hoverClose ? pal.Danger.ToU32() : pal.TextDisabled.ToU32(),
                          ICON_FA_XMARK);

            // Divider line
            draw->AddLine(ImVec2(winPos.x + 10.0f, winPos.y + headerH + 6.0f),
                          ImVec2(winPos.x + winSize.x - 10.0f, winPos.y + headerH + 6.0f),
                          pal.Border.WithAlpha(0.35f).ToU32(), 1.0f);

            ImGui::SetCursorPosY(headerH + 12.0f);

            // 3. Observer rows
            if (spectators.empty()) {
                ImGui::SetCursorPosX(14.0f);
                ImGui::TextColored(pal.TextDisabled.ToVec4(), "No active observers");
            } else {
                for (const auto& s : spectators) {
                    ImVec2 rowMin = ImGui::GetCursorScreenPos();
                    ImVec2 rowMax(rowMin.x + winSize.x - 20.0f, rowMin.y + 24.0f);

                    draw->AddRectFilled(rowMin, rowMax, pal.Header.WithAlpha(0.40f).ToU32(), 4.0f);
                    Render::ImGuiExt::AddSmoothBorder(draw, rowMin, rowMax, pal.Border.WithAlpha(0.30f).ToU32(), 4.0f, 1.0f);

                    draw->AddText(ImVec2(rowMin.x + 8.0f, rowMin.y + 5.0f), pal.Accent.ToU32(), ICON_FA_EYE);
                    draw->AddText(ImVec2(rowMin.x + 28.0f, rowMin.y + 4.0f), pal.TextPrimary.ToU32(), s.c_str());

                    ImGui::Dummy(ImVec2(winSize.x - 20.0f, 26.0f));
                }
            }
            Render::ImGuiExt::RenderResizeGrip(ImVec2(180.0f, 90.0f), "##SpectatorsResize");
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);
    }

} // namespace Solar::Game
