#include "solar/widgets/keybind.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include <windows.h>
#include <imgui_internal.h>

namespace Solar::Widgets {

    static const char* KeyToString(int key) {
        switch (key) {
        case VK_LBUTTON: return "M1";
        case VK_RBUTTON: return "M2";
        case VK_MBUTTON: return "M3";
        case VK_XBUTTON1: return "M4";
        case VK_XBUTTON2: return "M5";
        case VK_SHIFT: return "SHIFT";
        case VK_CONTROL: return "CTRL";
        case VK_MENU: return "ALT";
        case VK_SPACE: return "SPACE";
        case 0: return "NONE";
        }
        if (key >= 'A' && key <= 'Z') {
            static char buf[2] = { 0, 0 };
            buf[0] = static_cast<char>(key);
            return buf;
        }
        return "KEY";
    }

    bool Keybind(const char* label, int* key, KeyMode* mode) {
        int m = static_cast<int>(*mode);
        bool res = Keybind(label, key, &m);
        *mode = static_cast<KeyMode>(m);
        return res;
    }

    bool Keybind(const char* label, int* key, int* mode) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(label);
        float availX = ImGui::GetContentRegionAvail().x;
        float height = 24.0f;

        ImVec2 p = window->DC.CursorPos;
        ImRect bb(p, ImVec2(p.x + availX, p.y + height));

        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return false;

        const auto& pal = ThemeManager::Get().GetPalette();
        ImDrawList* draw = window->DrawList;

        draw->AddText(ImVec2(p.x, p.y + 4.0f), pal.TextPrimary.ToU32(), label);

        // Key bind button
        float btnW = 84.0f;
        ImVec2 btnPos(p.x + availX - btnW - 4.0f, p.y);
        ImVec2 btnEnd(btnPos.x + btnW, btnPos.y + height);

        static ImGuiID listeningId = 0;
        bool isListening = (listeningId == id);

        ImRect btnBB(btnPos, btnEnd);
        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(btnBB, id, &hovered, &held);

        if (pressed) {
            listeningId = isListening ? 0 : id;
            Audio::PlayClick();
        }

        if (isListening) {
            for (int k = 1; k < 255; k++) {
                if (GetAsyncKeyState(k) & 0x8000) {
                    if (k == VK_ESCAPE) *key = 0;
                    else *key = k;
                    listeningId = 0;
                    Audio::PlayClick();
                    break;
                }
            }
        }

        draw->AddRectFilled(btnPos, btnEnd, pal.CardHover.ToU32(), 4.0f);
        draw->AddRect(btnPos, btnEnd, isListening ? pal.Accent.ToU32() : pal.Border.ToU32(), 4.0f);

        const char* keyText = isListening ? "..." : KeyToString(*key);
        ImVec2 ts = ImGui::CalcTextSize(keyText);
        draw->AddText(ImVec2(btnPos.x + (btnW - ts.x) * 0.5f, btnPos.y + (height - ts.y) * 0.5f),
                      isListening ? pal.Accent.ToU32() : pal.TextPrimary.ToU32(), keyText);

        return false;
    }

} // namespace Solar::Widgets
