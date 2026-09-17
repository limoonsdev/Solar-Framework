#include "solar/ui/command_palette.hpp"
#include "solar/ui/kill_banner.hpp"
#include "solar/theme/theme_manager.hpp"
#include "solar/render/imgui_ext.hpp"
#include "solar/render/shadow_caster.hpp"
#include "solar/audio/audio_engine.hpp"
#include "solar/font_awesome.hpp"
#include "solar/notifications/notification_system.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>

namespace Solar::UI {

    static bool CaseInsensitiveFind(const std::string& str, const std::string& query) {
        if (query.empty()) return true;
        auto it = std::search(
            str.begin(), str.end(),
            query.begin(), query.end(),
            [](char ch1, char ch2) {
                return std::tolower(static_cast<unsigned char>(ch1)) == std::tolower(static_cast<unsigned char>(ch2));
            }
        );
        return it != str.end();
    }

    CommandPalette& CommandPalette::Get() {
        static CommandPalette instance;
        return instance;
    }

    CommandPalette::CommandPalette() {
        PopulateDefaultCommands();
    }

    void CommandPalette::PopulateDefaultCommands() {
        m_commands.clear();

        // 1. All 12 Themes
        RegisterCommand("Theme: Solar Apex (Gold Obsidian)", "Themes", "Alt+1", ICON_FA_SUN, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::SolarApex);
            Notify::Success("Theme Switched", "Activated Solar Apex gold obsidian theme.");
        });
        RegisterCommand("Theme: Neo Tokyo 2077 (Cyberpunk)", "Themes", "Alt+2", ICON_FA_MOON, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::NeoTokyo2077);
            Notify::Success("Theme Switched", "Activated Neo Tokyo 2077 theme.");
        });
        RegisterCommand("Theme: Ares Crimson (Tactical Red)", "Themes", "Alt+3", ICON_FA_FIRE, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::AresCrimson);
            Notify::Success("Theme Switched", "Activated Ares Crimson theme.");
        });
        RegisterCommand("Theme: Viper Matrix (Toxic Green)", "Themes", "Alt+4", ICON_FA_GHOST, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::ViperMatrix);
            Notify::Success("Theme Switched", "Activated Viper Matrix theme.");
        });
        RegisterCommand("Theme: Subzero Frost (Glacial Cyan)", "Themes", "Alt+5", ICON_FA_SNOWFLAKE, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::SubzeroFrost);
            Notify::Success("Theme Switched", "Activated Subzero Frost theme.");
        });
        RegisterCommand("Theme: Astral Amethyst (Imperial Purple)", "Themes", "Alt+6", ICON_FA_GEM, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::AstralAmethyst);
            Notify::Success("Theme Switched", "Activated Astral Amethyst theme.");
        });
        RegisterCommand("Theme: Onyx Chroma (Dynamic Spectrum)", "Themes", "Alt+7", ICON_FA_PALETTE, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::OnyxChroma);
            Notify::Success("Theme Switched", "Activated Onyx Chroma theme.");
        });
        RegisterCommand("Theme: Synthwave Sunset (80s Retrowave)", "Themes", "Alt+8", ICON_FA_SUN, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::SynthwaveSunset);
            Notify::Success("Theme Switched", "Activated Synthwave Sunset theme.");
        });
        RegisterCommand("Theme: Glacier Titanium (Cryo Metallic)", "Themes", "Alt+9", ICON_FA_SNOWFLAKE, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::GlacierTitanium);
            Notify::Success("Theme Switched", "Activated Glacier Titanium theme.");
        });
        RegisterCommand("Theme: Kintsugi Gold (Fractured Ceramic)", "Themes", "Alt+0", ICON_FA_GEM, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::KintsugiGold);
            Notify::Success("Theme Switched", "Activated Kintsugi Gold theme.");
        });
        RegisterCommand("Theme: Nebula Void (Deep Cosmos)", "Themes", "", ICON_FA_MOON, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::NebulaVoid);
            Notify::Success("Theme Switched", "Activated Nebula Void theme.");
        });
        RegisterCommand("Theme: Blood Moon (Gothic Ruby)", "Themes", "", ICON_FA_FIRE, []() {
            ThemeManager::Get().ApplyPreset(ThemePreset::BloodMoon);
            Notify::Success("Theme Switched", "Activated Blood Moon theme.");
        });

        // 2. Engine & Mode Toggles
        RegisterCommand("Mode: Toggle Rainbow Chroma Mode", "Engine", "F9", ICON_FA_WAND_MAGIC, []() {
            bool cur = ThemeManager::Get().IsRainbowMode();
            ThemeManager::Get().SetRainbowMode(!cur);
            Notify::Info("Rainbow Chroma", !cur ? "Rainbow spectrum cycling activated." : "Rainbow mode disabled.");
        });
        RegisterCommand("Action: Trigger Esports Kill Banner", "Testing", "F10", ICON_FA_SKULL, []() {
            TriggerKillBanner("Jett_Carry_01", "VANDAL PRIME", 160, true, 4);
        });

        // 3. System & Utilities
        RegisterCommand("System: Copy HWID to Clipboard", "System", "Ctrl+H", ICON_FA_COPY, []() {
            ImGui::SetClipboardText("HWID-SOLAR-7F9A-4B21-99CE-DEV");
            Notify::Success("HWID Copied", "Hardware ID copied to clipboard.");
        });
        RegisterCommand("System: Flush Audio Synthesizer Stream", "System", "", ICON_FA_VOLUME_HIGH, []() {
            Audio::PlayClick();
            Notify::Info("Audio", "Sound synthesizer buffer flushed.");
        });
    }

    void CommandPalette::RegisterCommand(const std::string& title, const std::string& category,
                                         const std::string& shortcut, const char* icon, std::function<void()> action) {
        m_commands.push_back({ title, category, shortcut, icon, action });
    }

    void CommandPalette::ClearCommands() {
        m_commands.clear();
        PopulateDefaultCommands();
    }

    void CommandPalette::Toggle() {
        if (m_open) {
            Close();
        } else {
            Open();
        }
    }

    void CommandPalette::Open() {
        m_open = true;
        m_filter[0] = '\0';
        m_selectedIndex = 0;
        m_needFocus = true;
        Audio::PlayClick();
    }

    void CommandPalette::Close() {
        m_open = false;
    }

    void CommandPalette::Render() {
        ImGuiIO& io = ImGui::GetIO();
        float dt = io.DeltaTime;

        // Global hotkey: Ctrl + P
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_P, false)) {
            Toggle();
        }

        // Animate smooth fade & slide
        float target = m_open ? 1.0f : 0.0f;
        m_animProgress += (target - m_animProgress) * (std::min)(1.0f, dt * 18.0f);

        if (m_animProgress < 0.01f && !m_open) {
            return;
        }

        const auto& pal = ThemeManager::Get().GetPalette();
        ImVec2 dispSize = io.DisplaySize;

        // Render soft ambient vignette BEHIND windows so palette window is never dimmed
        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(0, 0), dispSize,
            IM_COL32(0, 0, 0, static_cast<int>(55 * m_animProgress))
        );

        // Filter commands matching current query
        std::string query = m_filter;
        std::vector<const CommandItem*> filtered;
        filtered.reserve(m_commands.size());
        for (const auto& cmd : m_commands) {
            if (CaseInsensitiveFind(cmd.title, query) ||
                CaseInsensitiveFind(cmd.category, query) ||
                CaseInsensitiveFind(cmd.shortcut, query)) {
                filtered.push_back(&cmd);
            }
        }

        int filteredCount = static_cast<int>(filtered.size());
        if (filteredCount == 0) {
            m_selectedIndex = 0;
        } else {
            if (m_selectedIndex >= filteredCount) m_selectedIndex = filteredCount - 1;
            if (m_selectedIndex < 0) m_selectedIndex = 0;
        }

        // Palette Modal Window Dimensions & Spring Position
        float palWidth = (std::min)(580.0f, dispSize.x - 40.0f);
        float palHeight = 385.0f;
        float startY = 40.0f;
        float targetY = 90.0f;
        float curY = startY + (targetY - startY) * m_animProgress;
        float posX = (dispSize.x - palWidth) * 0.5f;

        ImGui::SetNextWindowPos(ImVec2(posX, curY), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(palWidth, palHeight), ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, pal.Card.WithAlpha(0.97f * m_animProgress).ToVec4());
        ImGui::PushStyleColor(ImGuiCol_Border, pal.Accent.WithAlpha(0.55f * m_animProgress).ToVec4());
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.2f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 16.0f));

        if (ImGui::Begin("##SolarCommandPalette", nullptr, flags)) {
            ImDrawList* draw = ImGui::GetWindowDrawList();
            ImVec2 winPos = ImGui::GetWindowPos();
            ImVec2 winMax(winPos.x + palWidth, winPos.y + palHeight);

            // Velvet drop shadow
            Render::ShadowCaster::DrawShadow(draw, winPos, winMax, 28.0f, 12.0f,
                                             Color(0, 0, 0, 0.60f * m_animProgress), ImVec2(0, 8.0f));
            Render::ImGuiExt::DrawSpecularEdge(draw, winPos, winMax,
                                               IM_COL32(255, 255, 255, static_cast<int>(30 * m_animProgress)), 12.0f, 1.0f);

            // Top Search Input Box
            ImGui::PushStyleColor(ImGuiCol_FrameBg, pal.Background.WithAlpha(0.85f).ToVec4());
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, pal.Background.ToVec4());
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, pal.Background.ToVec4());
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 10.0f));

            if (m_needFocus) {
                ImGui::SetKeyboardFocusHere();
                m_needFocus = false;
            }

            ImGui::SetNextItemWidth(palWidth - 32.0f);
            bool textChanged = ImGui::InputTextWithHint("##CommandSearch", "Type a command, tab, or action...",
                                                        m_filter, sizeof(m_filter));
            if (textChanged) {
                m_selectedIndex = 0;
            }

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);

            // Search Icon on top of input
            ImVec2 inMin = ImGui::GetItemRectMin();
            ImVec2 inMax = ImGui::GetItemRectMax();
            Render::ImGuiExt::AddSmoothBorder(draw, inMin, inMax,
                                              pal.Accent.WithAlpha(0.60f * m_animProgress).ToU32(), 8.0f, 1.2f);

            // Keyboard navigation
            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                Close();
                Audio::PlayClick();
            }
            if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
                if (filteredCount > 0) {
                    m_selectedIndex = (m_selectedIndex + 1) % filteredCount;
                    Audio::PlayClick();
                }
            }
            if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
                if (filteredCount > 0) {
                    m_selectedIndex = (m_selectedIndex - 1 + filteredCount) % filteredCount;
                    Audio::PlayClick();
                }
            }
            if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                if (filteredCount > 0 && m_selectedIndex >= 0 && m_selectedIndex < filteredCount) {
                    auto action = filtered[m_selectedIndex]->action;
                    Close();
                    Audio::PlayClick();
                    if (action) action();
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Commands List (dynamically sized for exactly 6 items with clean margins)
            float listH = 6 * 38.0f + 5 * 2.0f; // 238.0f
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 2.0f));
            ImGui::BeginChild("##CommandListChild", ImVec2(0, listH), false, ImGuiWindowFlags_NoScrollbar);
            {
                ImDrawList* childDraw = ImGui::GetWindowDrawList();
                if (filteredCount == 0) {
                    ImGui::SetCursorPosY(listH * 0.35f);
                    ImVec2 noResSz = ImGui::CalcTextSize("No matching commands found");
                    ImGui::SetCursorPosX((palWidth - 32.0f - noResSz.x) * 0.5f);
                    ImGui::TextColored(pal.TextDisabled, "No matching commands found");
                } else {
                    for (int i = 0; i < filteredCount; ++i) {
                        const auto* cmd = filtered[i];
                        bool isSelected = (i == m_selectedIndex);

                        if (isSelected) {
                            ImGui::SetScrollHereY(0.5f);
                        }

                        ImGui::PushID(i);
                        ImVec2 itemPos = ImGui::GetCursorScreenPos();
                        float itemW = ImGui::GetContentRegionAvail().x;
                        float itemH = 38.0f;

                        bool clicked = ImGui::InvisibleButton("##ItemBtn", ImVec2(itemW, itemH));
                        bool hovered = ImGui::IsItemHovered();

                        if (hovered && !isSelected) {
                            m_selectedIndex = i;
                        }

                        if (clicked) {
                            auto action = cmd->action;
                            Close();
                            Audio::PlayClick();
                            if (action) action();
                            ImGui::PopID();
                            break;
                        }

                        ImVec2 iMin = itemPos;
                        ImVec2 iMax(itemPos.x + itemW, itemPos.y + itemH);

                        if (isSelected) {
                            childDraw->AddRectFilled(iMin, iMax, pal.Accent.WithAlpha(0.18f).ToU32(), 6.0f);
                            Render::ImGuiExt::AddSmoothBorder(childDraw, iMin, iMax, pal.Accent.WithAlpha(0.70f).ToU32(), 6.0f, 1.0f);
                            // Left accent bar
                            childDraw->AddRectFilled(ImVec2(iMin.x + 3.0f, iMin.y + 6.0f),
                                                     ImVec2(iMin.x + 6.0f, iMax.y - 6.0f), pal.Accent.ToU32(), 2.0f);
                        } else if (hovered) {
                            childDraw->AddRectFilled(iMin, iMax, pal.CardHover.WithAlpha(0.50f).ToU32(), 6.0f);
                        }

                        // Icon
                        float textX = itemPos.x + 16.0f;
                        if (cmd->icon) {
                            u32 iconCol = isSelected ? pal.Accent.ToU32() : pal.TextSecondary.ToU32();
                            childDraw->AddText(ImVec2(textX, itemPos.y + 10.0f), iconCol, cmd->icon);
                            textX += 26.0f;
                        }

                        // Title
                        u32 titleCol = isSelected ? pal.TextPrimary.ToU32() : pal.TextSecondary.ToU32();
                        childDraw->AddText(ImVec2(textX, itemPos.y + 10.0f), titleCol, cmd->title.c_str());

                        // Category Pill
                        if (!cmd->category.empty()) {
                            ImVec2 catSz = ImGui::CalcTextSize(cmd->category.c_str());
                            float catX = itemPos.x + itemW - 12.0f - catSz.x - (cmd->shortcut.empty() ? 0.0f : 70.0f);
                            ImVec2 pMin(catX - 6.0f, itemPos.y + 8.0f);
                            ImVec2 pMax(catX + catSz.x + 6.0f, itemPos.y + 28.0f);
                            childDraw->AddRectFilled(pMin, pMax, pal.Header.WithAlpha(0.85f).ToU32(), 4.0f);
                            childDraw->AddRect(pMin, pMax, pal.Border.WithAlpha(0.50f).ToU32(), 4.0f);
                            childDraw->AddText(ImVec2(catX, itemPos.y + 9.5f), pal.TextDisabled.ToU32(), cmd->category.c_str());
                        }

                        // Shortcut Badge
                        if (!cmd->shortcut.empty()) {
                            ImVec2 scSz = ImGui::CalcTextSize(cmd->shortcut.c_str());
                            float scX = itemPos.x + itemW - 10.0f - scSz.x;
                            childDraw->AddText(ImVec2(scX, itemPos.y + 10.0f), pal.Accent.WithAlpha(0.85f).ToU32(), cmd->shortcut.c_str());
                        }

                        ImGui::PopID();
                    }
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();

            // Footer keyboard hints (natural flow below list with clean separator)
            ImGui::Dummy(ImVec2(0.0f, 6.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 6.0f));

            ImGui::TextColored(pal.TextDisabled, "Navigate:");
            ImGui::SameLine();
            ImGui::TextColored(pal.Accent, "[↑ / ↓]");
            ImGui::SameLine();
            ImGui::TextColored(pal.TextDisabled, "Select:");
            ImGui::SameLine();
            ImGui::TextColored(pal.Accent, "[ENTER]");
            ImGui::SameLine();
            ImGui::TextColored(pal.TextDisabled, "Close:");
            ImGui::SameLine();
            ImGui::TextColored(pal.Accent, "[ESC]");
            ImGui::SameLine();
            ImGui::TextColored(pal.TextDisabled, "Palette:");
            ImGui::SameLine();
            ImGui::TextColored(pal.Accent, "[Ctrl + P]");
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);
    }

} // namespace Solar::UI
