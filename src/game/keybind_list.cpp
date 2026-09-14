#include "solar/game/keybind_list.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/shadow_caster.hpp"
#include "solar/font_awesome.hpp"
#include <imgui.h>
#include <imgui_internal.h>

namespace Solar::Game {

    void KeybindList::Render(bool* p_open, const std::vector<std::pair<std::string, std::string>>& binds) {
        if (!p_open || !(*p_open)) return;

        ImGui::SetNextWindowSize(ImVec2(220.0f, 150.0f), ImGuiCond_FirstUseEver);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

        const auto& pal = ThemeManager::Get().GetPalette();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, pal.Card.WithAlpha(0.96f).ToVec4());
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));

        if (ImGui::Begin("##ActiveKeybindsWindow", p_open, flags)) {
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

            // 2. Custom Window Header (Icon + Title + Close)
            float headerH = 26.0f;
            draw->AddText(ImVec2(winPos.x + 12.0f, winPos.y + 12.0f), pal.Accent.ToU32(), ICON_FA_KEY);
            draw->AddText(ImVec2(winPos.x + 32.0f, winPos.y + 10.0f), pal.TextPrimary.ToU32(), "Active Binds");

            // Close button [x]
            ImVec2 closePos(winPos.x + winSize.x - 22.0f, winPos.y + 8.0f);
            ImGui::SetCursorScreenPos(closePos);
            if (ImGui::InvisibleButton("##keybind_close", ImVec2(16.0f, 16.0f))) {
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

            // 3. Keybind Rows
            if (binds.empty()) {
                ImGui::SetCursorPosX(14.0f);
                ImGui::TextColored(pal.TextDisabled.ToVec4(), "No active binds");
            } else {
                for (const auto& b : binds) {
                    ImVec2 rowMin = ImGui::GetCursorScreenPos();
                    ImVec2 rowMax(rowMin.x + winSize.x - 20.0f, rowMin.y + 24.0f);

                    draw->AddText(ImVec2(rowMin.x + 4.0f, rowMin.y + 4.0f), pal.TextPrimary.ToU32(), b.first.c_str());

                    // Keycap Pill on right
                    ImVec2 keySize = ImGui::CalcTextSize(b.second.c_str());
                    float pillW = keySize.x + 12.0f;
                    ImVec2 pillMin(rowMax.x - pillW, rowMin.y + 2.0f);
                    ImVec2 pillMax(rowMax.x, rowMin.y + 22.0f);

                    draw->AddRectFilled(pillMin, pillMax, pal.Header.WithAlpha(0.80f).ToU32(), 4.0f);
                    Render::ImGuiExt::AddSmoothBorder(draw, pillMin, pillMax, pal.Accent.WithAlpha(0.40f).ToU32(), 4.0f, 1.0f);
                    draw->AddText(ImVec2(pillMin.x + 6.0f, pillMin.y + 3.0f), pal.Accent.ToU32(), b.second.c_str());

                    ImGui::Dummy(ImVec2(winSize.x - 20.0f, 26.0f));
                }
            }
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);
    }

} // namespace Solar::Game
