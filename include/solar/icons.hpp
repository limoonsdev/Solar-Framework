#pragma once

#include <imgui.h>

namespace Solar {

    enum class IconType {
        None,
        Crosshair,
        Eye,
        Sliders,
        Palette,
        Folder,
        User,
        Bell,
        Shield,
        Check,
        Close,
        Minimize,
        ChevronDown,
        ChevronRight,
        Search,
        Keyboard,
        Sparkle,
        Sun,
        Lock,
        Floppy,
        Terminal,
        Gear,
        Key,
        Fingerprint,
        Expand,
        Skull,
        Cube
    };

    class IconRenderer {
    public:
        // Render a vector icon within the given bounding box on the specified drawlist
        static void DrawIcon(ImDrawList* drawList, IconType type, const ImVec2& center, float size, ImU32 color, float thickness = 1.6f);
    };

} // namespace Solar
