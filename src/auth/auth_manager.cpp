#include "solar/auth/auth_manager.hpp"

namespace Solar::Auth {

    AuthManager& AuthManager::Get() {
        static AuthManager instance;
        return instance;
    }

    bool AuthManager::Authenticate(const std::string& key) {
        if (!key.empty()) {
            m_authenticated = true;
            m_tier = AuthTier::Lifetime;
            return true;
        }
        return false;
    }

} // namespace Solar::Auth
