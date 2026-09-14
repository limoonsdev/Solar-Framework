#pragma once
#include "solar/core/types.hpp"
#include <string>

namespace Solar::Auth {

    enum class AuthTier {
        None = 0,
        Standard,
        VIP,
        Lifetime,
        Developer
    };

    class AuthManager {
    public:
        static AuthManager& Get();
        bool Authenticate(const std::string& key);
        AuthTier GetTier() const { return m_tier; }
        bool IsAuthenticated() const { return m_authenticated; }

    private:
        AuthManager() = default;
        bool m_authenticated = false;
        AuthTier m_tier = AuthTier::None;
    };

} // namespace Solar::Auth
