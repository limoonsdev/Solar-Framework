#pragma once
#include "solar/core/types.hpp"

namespace Solar::Widgets {

    void ProgressBar(float fraction, const ImVec2& size = ImVec2(0, 10.0f), const char* overlay = nullptr);

} // namespace Solar::Widgets
