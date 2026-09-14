#pragma once
#include "solar/core/types.hpp"
#include <vector>
#include <string>

namespace Solar::Game {

    class SpectatorList {
    public:
        static void Render(bool* p_open, const std::vector<std::string>& spectators);
    };

} // namespace Solar::Game
