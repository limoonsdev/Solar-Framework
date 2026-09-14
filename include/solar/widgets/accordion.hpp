#pragma once
#include "solar/core/types.hpp"
#include <imgui.h>

namespace Solar::Widgets {

    /**
     * @brief Collapsible Accordion Card Section
     * Renders a modern styled container with an interactive header, indicator chevron,
     * optional subtitle, and smooth expandable content area.
     *
     * @param label Title of the accordion section
     * @param isOpen Pointer to boolean holding expanded/collapsed state
     * @param subtitle Optional muted subtitle description
     * @return true if section is open and body content should be rendered
     */
    bool BeginAccordion(const char* label, bool* isOpen, const char* subtitle = nullptr);

    /**
     * @brief Closes the active accordion section
     */
    void EndAccordion();

} // namespace Solar::Widgets
