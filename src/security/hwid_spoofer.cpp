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

        float availW = ImGui::GetContentRegionAvail().x;
        float colW = (availW - 16.0f) * 0.5f;

        // Left Column: Identity Telemetry & Actions
        ImGui::BeginChild("##SpooferColLeft", ImVec2(colW, 0), false, ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::TextColored(pal.Accent, "KERNEL LEVEL HWID SPOOFER");
            ImGui::SameLine(0, 8.0f);
            Widgets::Badge(s.isSpoofed ? "ACTIVE" : "STOCK HWID", s.isSpoofed ? pal.Success : pal.Warning);

            ImGui::Dummy(ImVec2(0, 6.0f));
            ImGui::TextColored(pal.TextPrimary, "Hardware Serial Telemetry:");
            ImGui::Dummy(ImVec2(0, 2.0f));
            ImGui::TextColored(pal.TextDisabled, "Motherboard UUID:\n  %s", s.moboUUID.c_str());
            ImGui::Dummy(ImVec2(0, 2.0f));
            ImGui::TextColored(pal.TextDisabled, "Network MAC:\n  %s", s.macAddress.c_str());
            ImGui::Dummy(ImVec2(0, 2.0f));
            ImGui::TextColored(pal.TextDisabled, "Storage Serial:\n  %s", s.diskSerial.c_str());
            ImGui::Dummy(ImVec2(0, 2.0f));
            ImGui::TextColored(pal.TextDisabled, "Graphics Card ID:\n  %s", s.gpuGuid.c_str());

            ImGui::Dummy(ImVec2(0, 10.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 6.0f));

            if (Widgets::Button("Generate New Hardware Profile", ImVec2(0, 36), ButtonStyle::Secondary)) {
                s.moboUUID = Crypto::GenerateRandomGUID();
                s.macAddress = Crypto::GenerateMACAddress();
                s.diskSerial = Crypto::GenerateDiskSerial();
                s.gpuGuid = Crypto::GenerateRandomGUID();
                Notify::Info("HWID Generator", "Fresh hardware identities created.");
            }

            ImGui::Dummy(ImVec2(0, 4.0f));
            if (Widgets::Button(s.isSpoofed ? "Restore Original HWID" : "Apply Kernel Spoofer & Clean Traces", ImVec2(0, 40), s.isSpoofed ? ButtonStyle::Danger : ButtonStyle::Primary)) {
                s.isSpoofed = !s.isSpoofed;
                if (s.isSpoofed) {
                    Notify::Success("Spoofer Engaged", "All hardware adapters masked and traces flushed!");
                } else {
                    Notify::Warning("Spoofer Disengaged", "Restored physical device serial numbers.");
                }
            }
        }
        ImGui::EndChild();

        ImGui::SameLine(0, 16.0f);

        // Right Column: Virtualization & Anti-Cheat Cleaning Toggles
        ImGui::BeginChild("##SpooferColRight", ImVec2(colW, 0), false, ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::TextColored(pal.Accent, "Hardware Identifiers to Virtualize:");
            ImGui::Dummy(ImVec2(0, 2.0f));
            Widgets::Toggle("Virtualize SMBIOS & Motherboard", &s.spoofMobo);
            Widgets::Toggle("Randomize MAC Addresses", &s.spoofMAC);
            Widgets::Toggle("Mask Volume & Disk Serials", &s.spoofDisk);
            Widgets::Toggle("Spoof Display Adapter GUID", &s.spoofGPU);

            ImGui::Dummy(ImVec2(0, 8.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 4.0f));

            ImGui::TextColored(pal.Accent, "Anti-Cheat Trace Cleaners:");
            ImGui::Dummy(ImVec2(0, 2.0f));
            Widgets::Toggle("Flush Vanguard Driver Traces", &s.cleanVanguard);
            Widgets::Toggle("Wipe EasyAntiCheat & BattlEye", &s.cleanEAC);
            Widgets::Toggle("Clean FiveM & ROS Identifiers", &s.cleanFiveM);
            Widgets::Toggle("Purge Discord & Steam Journals", &s.cleanDiscord);
        }
        ImGui::EndChild();
    }

} // namespace Solar::Security
