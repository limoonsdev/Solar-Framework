#include "solar/tools/profiler.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui.h>

namespace Solar::Tools {

    void Profiler::Render(bool* p_open) {
        if (!p_open || !(*p_open)) return;

        ImGui::SetNextWindowSize(ImVec2(320, 240), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Solar Engine Telemetry", p_open)) {
            ImGuiIO& io = ImGui::GetIO();
            const auto& pal = ThemeManager::Get().GetPalette();

            ImGui::TextColored(pal.Accent, "RENDER PIPELINE METRICS");
            ImGui::Separator();

            ImGui::Text("Framerate:       %.1f FPS", io.Framerate);
            ImGui::Text("Frame Delta:     %.3f ms", io.DeltaTime * 1000.0f);
            ImGui::Text("Draw Calls:      %d", ImGui::GetDrawData() ? ImGui::GetDrawData()->CmdListsCount : 0);
            ImGui::Text("Total Vertices:  %d", ImGui::GetDrawData() ? ImGui::GetDrawData()->TotalVtxCount : 0);
            ImGui::Text("Total Indices:   %d", ImGui::GetDrawData() ? ImGui::GetDrawData()->TotalIdxCount : 0);

            ImGui::Spacing();
            static float fpsValues[60] = { 0 };
            static int offset = 0;
            fpsValues[offset] = io.Framerate;
            offset = (offset + 1) % 60;
            ImGui::PlotLines("##FPSGraph", fpsValues, 60, offset, "FPS Stream", 0.0f, 144.0f, ImVec2(0, 60.0f));
        }
        ImGui::End();
    }

} // namespace Solar::Tools
