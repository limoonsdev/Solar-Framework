#include "solar/security/hwid_spoofer.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/core/crypto.hpp"
#include "solar/widgets/button.hpp"
#include "solar/widgets/toggle.hpp"
#include "solar/widgets/badge.hpp"
#include "solar/notifications/notification_system.hpp"
#include <imgui.h>

namespace Solar::Security {

    void SpooferPanel::Render(SpooferState& s) {
        if (s.moboUUID.empty()) {
            s.moboUUID = Crypto::GenerateRandomGUID();
            s.macAddress = Crypto::GenerateMACAddress();
            s.diskSerial = Crypto::GenerateDiskSerial();
            s.gpuGuid = Crypto::GenerateRandomGUID();
        }

        const auto& pal = ThemeManager::Get().GetPalette();

        ImGui::TextColored(pal.Accent, "KERNEL LEVEL HWID SPOOFER");
        ImGui::SameLine();
        Widgets::Badge(s.isSpoofed ? "ACTIVE" : "STOCK HWID", s.isSpoofed ? pal.Success : pal.Warning);

        ImGui::Spacing();
        ImGui::TextColored(pal.TextDisabled, "Motherboard UUID:  %s", s.moboUUID.c_str());
        ImGui::TextColored(pal.TextDisabled, "Network MAC:       %s", s.macAddress.c_str());
        ImGui::TextColored(pal.TextDisabled, "Storage Serial:    %s", s.diskSerial.c_str());
        ImGui::TextColored(pal.TextDisabled, "Graphics Card ID:  %s", s.gpuGuid.c_str());
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Text("Hardware Identifiers to Virtualize:");
        Widgets::Toggle("Virtualize SMBIOS & Motherboard", &s.spoofMobo);
        Widgets::Toggle("Randomize Physical MAC Addresses", &s.spoofMAC);
        Widgets::Toggle("Mask Drive Volume & Disk Serials", &s.spoofDisk);
        Widgets::Toggle("Spoof Display Adapter GUID", &s.spoofGPU);

        ImGui::Separator();
        ImGui::Text("Anti-Cheat Trace Cleaners:");
        Widgets::Toggle("Flush Vanguard Registry & Driver Traces", &s.cleanVanguard);
        Widgets::Toggle("Wipe EasyAntiCheat & BattlEye Machine GUIDs", &s.cleanEAC);
        Widgets::Toggle("Clean FiveM DigitalEntitlements & ROS Identifiers", &s.cleanFiveM);
        Widgets::Toggle("Purge Discord & Steam USN Journals", &s.cleanDiscord);

        ImGui::Spacing();
        if (Widgets::Button("Generate New Random Hardware Profile", ImVec2(0, 36), ButtonStyle::Secondary)) {
            s.moboUUID = Crypto::GenerateRandomGUID();
            s.macAddress = Crypto::GenerateMACAddress();
            s.diskSerial = Crypto::GenerateDiskSerial();
            s.gpuGuid = Crypto::GenerateRandomGUID();
            Notify::Info("HWID Generator", "Fresh hardware identities created.");
        }

        ImGui::Spacing();
        if (Widgets::Button(s.isSpoofed ? "Restore Original HWID" : "Apply Kernel Spoofer & Clean Traces", ImVec2(0, 42), s.isSpoofed ? ButtonStyle::Danger : ButtonStyle::Primary)) {
            s.isSpoofed = !s.isSpoofed;
            if (s.isSpoofed) {
                Notify::Success("Spoofer Engaged", "All hardware adapters masked and traces flushed!");
            } else {
                Notify::Warning("Spoofer Disengaged", "Restored physical device serial numbers.");
            }
        }
    }

} // namespace Solar::Security
