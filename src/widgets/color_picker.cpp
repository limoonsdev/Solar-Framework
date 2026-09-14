#include "solar/widgets/color_picker.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    bool ColorPicker(const char* label, float col[4]) {
        ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar
                                  | ImGuiColorEditFlags_NoInputs
                                  | ImGuiColorEditFlags_AlphaPreview;
        bool changed = ImGui::ColorEdit4(label, col, flags);
        return changed;
    }

} // namespace Solar::Widgets
