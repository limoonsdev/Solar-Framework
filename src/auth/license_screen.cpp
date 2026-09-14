#include "solar/auth/license_screen.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/widgets/button.hpp"
#include "solar/widgets/input_text.hpp"
#include "solar/widgets/checkbox.hpp"
#include "solar/widgets/badge.hpp"
#include "solar/notifications/notification_system.hpp"
#include <imgui.h>
#include <cstring>

namespace Solar::Auth {

    void LicenseScreen::Render(char* licenseBuffer, size_t bufferSize, const char* hwid, bool* rememberMe, bool* loggedIn) {
        const auto& pal = ThemeManager::Get().GetPalette();

        ImGui::TextColored(pal.TextPrimary, "Solar Framework Client Authentication");
        ImGui::TextColored(pal.TextDisabled, "Enter your product key or subscription token below to activate:");
        ImGui::Spacing();

        Widgets::InputText("License Key", licenseBuffer, bufferSize, "SOLAR-XXXX-XXXX-XXXX");
        ImGui::Spacing();

        ImGui::TextColored(pal.TextDisabled, "Hardware Fingerprint:");
        ImGui::TextColored(pal.Accent, "%s", hwid);
        ImGui::SameLine(0, 10.0f);
        if (Widgets::Button("Copy HWID", ImVec2(90, 24), ButtonStyle::Secondary)) {
            ImGui::SetClipboardText(hwid);
            Notify::Info("Clipboard", "HWID copied to clipboard.");
        }

        ImGui::Spacing();
        Widgets::Checkbox("Remember authentication credentials on this device", rememberMe);
        ImGui::Spacing();

        if (Widgets::Button(*loggedIn ? "Disconnect Session" : "Activate & Launch Software", ImVec2(0, 40), *loggedIn ? ButtonStyle::Secondary : ButtonStyle::Primary)) {
            if (*loggedIn) {
                *loggedIn = false;
                Notify::Info("Authentication", "Session terminated.");
            } else {
                if (strlen(licenseBuffer) > 0) {
                    *loggedIn = true;
                    Notify::Success("Authentication", "License validated! Welcome, Lifetime VIP.");
                } else {
                    Notify::Error("Auth Failure", "Please enter a valid license key.");
                }
            }
        }
    }

} // namespace Solar::Auth
