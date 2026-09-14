#include "solar/widgets/combo.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    bool Combo(const char* label, int* current_item, const char* const items[], int items_count) {
        bool changed = ImGui::Combo(label, current_item, items, items_count);
        if (changed) {
            Audio::PlayClick();
        }
        return changed;
    }

} // namespace Solar::Widgets
