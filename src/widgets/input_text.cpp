#include "solar/widgets/input_text.hpp"
#include "solar/theme/theme_manager.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    bool InputText(const char* label, char* buf, size_t buf_size, const char* placeholder) {
        ImGuiInputTextFlags flags = 0;
        return ImGui::InputTextWithHint(label, placeholder ? placeholder : "", buf, buf_size, flags);
    }

} // namespace Solar::Widgets
