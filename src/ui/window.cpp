#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include "solar/ui/window.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/shadow_caster.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/fx/particle_system.hpp"
#include "solar/fx/glow_fx.hpp"
#include <algorithm>

namespace Solar::UI {

    bool Window::Begin(const char* name, bool* p_open, const ImVec2& defaultSize) {
        ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
                               | ImGuiWindowFlags_NoResize
                               | ImGuiWindowFlags_NoCollapse
                               | ImGuiWindowFlags_NoScrollbar
                               | ImGuiWindowFlags_NoScrollWithMouse;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        bool visible = ImGui::Begin(name, p_open, flags);
        if (visible) {
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 size = ImGui::GetWindowSize();
            ImDrawList* draw = ImGui::GetWindowDrawList();
            const auto& pal = ThemeManager::Get().GetPalette();
            const auto& sty = ThemeManager::Get().GetStyle();

            // Velvet drop shadow
            Render::ShadowCaster::DrawShadow(draw, pos, ImVec2(pos.x + size.x, pos.y + size.y),
                                             24.0f, sty.WindowRounding, Color(0, 0, 0, 0.65f), ImVec2(0, 8.0f));

            // Main obsidian background
            draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                                pal.Background.ToU32(), sty.WindowRounding);

            // Ambient background glow
            if (sty.EnableGlow) {
                FX::GlowFX::RenderAmbientBackgroundGlow(draw, pos, ImVec2(pos.x + size.x, pos.y + size.y), pal.Accent);
            }

            // Floating background particles
            if (sty.EnableParticles) {
                FX::ParticleSystem::Get().UpdateAndRender(draw, pos, ImVec2(pos.x + size.x, pos.y + size.y), pal.Accent);
            }

            // Window border (pixel-perfect smooth antialiasing)
            Render::ImGuiExt::AddSmoothBorder(draw, pos, ImVec2(pos.x + size.x, pos.y + size.y),
                                              pal.Border.ToU32(), sty.WindowRounding, sty.BorderSize);
            // Specular top edge sheen on the main window frame
            Render::ImGuiExt::DrawSpecularEdge(draw, pos, ImVec2(pos.x + size.x, pos.y + size.y),
                                               IM_COL32(255, 255, 255, 24), sty.WindowRounding, 1.0f);
        }

        return visible;
    }

    void Window::RenderResizeGrip(const ImVec2& winPos, const ImVec2& winSize) {
        ImGuiIO& io = ImGui::GetIO();
        float gripSize = 18.0f;
        ImVec2 gripPos(winPos.x + winSize.x - gripSize - 4.0f, winPos.y + winSize.y - gripSize - 4.0f);
        ImRect gripRect(gripPos, ImVec2(gripPos.x + gripSize, gripPos.y + gripSize));

        ImGuiID id = ImGui::GetID("##WindowResizeGripHandle");
        bool hovered, held;
        ImGui::ButtonBehavior(gripRect, id, &hovered, &held);

        if (held && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 newSize(winSize.x + io.MouseDelta.x, winSize.y + io.MouseDelta.y);
            newSize.x = (std::max)(newSize.x, 760.0f);
            newSize.y = (std::max)(newSize.y, 480.0f);
            ImGui::SetWindowSize(newSize);
        }

        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = ThemeManager::Get().GetPalette();
        u32 gripCol = (held || hovered) ? pal.Accent.ToU32() : pal.TextDisabled.WithAlpha(0.40f).ToU32();

        // 3 diagonal grip hashes
        for (int i = 0; i < 3; ++i) {
            float offset = static_cast<float>(i) * 5.0f;
            ImVec2 p1(gripPos.x + gripSize - offset, gripPos.y + gripSize);
            ImVec2 p2(gripPos.x + gripSize, gripPos.y + gripSize - offset);
            draw->AddLine(p1, p2, gripCol, 1.5f);
        }

        if (hovered || held) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
        }
    }

    void Window::End() {
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        RenderResizeGrip(pos, size);
        ImGui::End();
        ImGui::PopStyleVar(2);
    }

} // namespace Solar::UI
