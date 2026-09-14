#pragma once
#include "solar/core/types.hpp"
#include <vector>
#include <string>
#include <utility>

namespace Solar::Game {

    class KeybindList {
    public:
        static void Render(bool* p_open, const std::vector<std::pair<std::string, std::string>>& binds);
    };

} // namespace Solar::Game
