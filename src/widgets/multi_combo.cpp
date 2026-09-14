#include "solar/widgets/multi_combo.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/audio/audio_engine.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    bool MultiCombo(const char* label, std::vector<bool>& values, const std::vector<std::string>& items) {
        std::string preview;
        int count = 0;
        for (size_t i = 0; i < values.size() && i < items.size(); i++) {
            if (values[i]) {
                if (!preview.empty()) preview += ", ";
                preview += items[i];
                count++;
            }
        }
        if (count == 0) preview = "None selected";

        bool changed = false;
        if (ImGui::BeginCombo(label, preview.c_str())) {
            for (size_t i = 0; i < items.size() && i < values.size(); i++) {
                bool selected = values[i];
                if (ImGui::Selectable(items[i].c_str(), selected, ImGuiSelectableFlags_DontClosePopups)) {
                    values[i] = !values[i];
                    changed = true;
                    Audio::PlayClick();
                }
            }
            ImGui::EndCombo();
        }
        return changed;
    }

} // namespace Solar::Widgets
