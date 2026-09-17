#include "solar_ai.hpp"
#include "studio_app.hpp"
#include <solar/solar.hpp>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <cctype>

namespace Solar::Studio {

    static std::string ToLower(const std::string& str) {
        std::string res = str;
        std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return res;
    }

    SolarAI& SolarAI::Get() {
        static SolarAI instance;
        return instance;
    }

    std::string SolarAI::GetCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S");
        return ss.str();
    }

    void SolarAI::Initialize() {
        m_history.clear();
        AddAIMessage("Bonjour ! Je suis l'IA Copilot intégrée de Solar Studio. Décrivez-moi l'interface, les animations, le thème ou le loader que vous souhaitez concevoir et je l'exécute instantanément.");
    }

    void SolarAI::AddUserMessage(const std::string& text) {
        m_history.push_back({ true, text, {}, GetCurrentTimestamp() });
    }

    void SolarAI::AddAIMessage(const std::string& text, const std::vector<std::string>& actions) {
        m_history.push_back({ false, text, actions, GetCurrentTimestamp() });
    }

    void SolarAI::ClearHistory() {
        m_history.clear();
        AddAIMessage("Historique réinitialisé. Comment puis-je vous aider sur votre interface Solar ?");
    }

    AIResponse SolarAI::ProcessPrompt(const std::string& prompt, StudioApp& studio) {
        std::string lower = ToLower(prompt);
        AIResponse resp;
        resp.confidence = 0.96f;

        // 1. Intent: Reset / Clear
        if (lower.find("efface") != std::string::npos || lower.find("vide") != std::string::npos || lower.find("reset") != std::string::npos || lower.find("recommence") != std::string::npos) {
            studio.ClearComponents();
            resp.actions.push_back("Réinitialisation complète du canevas.");
            resp.text = "J'ai vidé tous les composants du canevas. Votre espace de travail est maintenant vierge et prêt.";
            return resp;
        }

        // 2. Intent: Loader Template creation (Clean Software Loader)
        if (lower.find("loader") != std::string::npos || lower.find("launcher") != std::string::npos || lower.find("connexion") != std::string::npos || lower.find("login") != std::string::npos) {
            studio.SetupLoaderTemplate();
            resp.actions.push_back("Chargement du template Software Loader.");
            resp.actions.push_back("Ajout du champ Clé de Licence & bouton d'activation.");
            resp.actions.push_back("Configuration de la barre de statut de chargement.");
            resp.actions.push_back("Arrondi premium calibré à 14px.");
            resp.text = "J'ai conçu pour vous un **Loader de logiciel moderne et épuré** : champ de clé de licence, bouton d'activation, barre de progression dynamique et statut de connexion sécurisée.";
            return resp;
        }

        // 3. Intent: Theme & Colors
        bool themeChanged = false;
        if (lower.find("cyberpunk") != std::string::npos || lower.find("rose") != std::string::npos || lower.find("neon") != std::string::npos) {
            studio.SetAccentColor(1.00f, 0.20f, 0.55f, 1.0f);
            studio.SetBorderRounding(4.0f);
            resp.actions.push_back("Thème basculé sur Cyberpunk Neon (Accent Rose Vif, Coins 4px).");
            themeChanged = true;
        } else if (lower.find("or") != std::string::npos || lower.find("gold") != std::string::npos || lower.find("luxe") != std::string::npos || lower.find("obsidian") != std::string::npos) {
            studio.SetAccentColor(0.95f, 0.78f, 0.25f, 1.0f);
            studio.SetBorderRounding(12.0f);
            resp.actions.push_back("Thème basculé sur Obsidian Luxury (Accent Or Sombre, Coins 12px).");
            themeChanged = true;
        } else if (lower.find("vert") != std::string::npos || lower.find("green") != std::string::npos || lower.find("emeraude") != std::string::npos || lower.find("streamer") != std::string::npos) {
            studio.SetAccentColor(0.18f, 0.85f, 0.55f, 1.0f);
            studio.SetBorderRounding(16.0f);
            resp.actions.push_back("Thème basculé sur Minimal Streamer (Accent Vert Émeraude, Coins 16px).");
            themeChanged = true;
        } else if (lower.find("bleu") != std::string::npos || lower.find("blue") != std::string::npos || lower.find("tactique") != std::string::npos || lower.find("tactical") != std::string::npos) {
            studio.SetAccentColor(0.20f, 0.65f, 1.00f, 1.0f);
            studio.SetBorderRounding(8.0f);
            resp.actions.push_back("Thème basculé sur Tactical Blue (Accent Bleu Cyan, Coins 8px).");
            themeChanged = true;
        } else if (lower.find("rouge") != std::string::npos || lower.find("red") != std::string::npos || lower.find("orange") != std::string::npos || lower.find("esport") != std::string::npos) {
            studio.SetAccentColor(0.98f, 0.35f, 0.15f, 1.0f);
            studio.SetBorderRounding(6.0f);
            resp.actions.push_back("Thème basculé sur Esports Pro (Accent Orange Racing, Coins 6px).");
            themeChanged = true;
        }

        // 4. Intent: Border Rounding Extraction
        if (lower.find("arrondi") != std::string::npos || lower.find("radius") != std::string::npos || lower.find("coins") != std::string::npos) {
            // Search for digits
            for (size_t i = 0; i < lower.size(); ++i) {
                if (std::isdigit(static_cast<unsigned char>(lower[i]))) {
                    float r = static_cast<float>(std::atoi(lower.c_str() + i));
                    if (r >= 0.0f && r <= 30.0f) {
                        studio.SetBorderRounding(r);
                        resp.actions.push_back("Arrondi des bordures fixé à " + std::to_string(static_cast<int>(r)) + "px.");
                        break;
                    }
                }
            }
        }

        // 5. Intent: Apply Animations
        if (lower.find("anime") != std::string::npos || lower.find("animation") != std::string::npos || lower.find("pulse") != std::string::npos || lower.find("shimmer") != std::string::npos || lower.find("bounce") != std::string::npos) {
            ComponentAnimation anim = ComponentAnimation::GlowPulse;
            std::string animName = "Glow Pulse";

            if (lower.find("shimmer") != std::string::npos || lower.find("brillance") != std::string::npos) {
                anim = ComponentAnimation::ShimmerWave;
                animName = "Shimmer Wave (Balayage Lumineux)";
            } else if (lower.find("bounce") != std::string::npos || lower.find("rebond") != std::string::npos || lower.find("spring") != std::string::npos) {
                anim = ComponentAnimation::SpringBounce;
                animName = "Spring Bounce (Dynamique RK4)";
            } else if (lower.find("fade") != std::string::npos || lower.find("fondu") != std::string::npos) {
                anim = ComponentAnimation::FadeIn;
                animName = "Fade In (Fondu Alpha)";
            } else if (lower.find("breath") != std::string::npos || lower.find("respiration") != std::string::npos) {
                anim = ComponentAnimation::BreathingSine;
                animName = "Breathing Sine (Respiration Tactile)";
            }

            studio.ApplyAnimationToAll(anim);
            resp.actions.push_back("Animation '" + animName + "' appliquée sur les composants.");
        }

        // 6. Intent: Add Components
        if (lower.find("slider") != std::string::npos || lower.find("curseur") != std::string::npos) {
            studio.AddComponent({ "slider", "Paramètre Calibré", "", 65.0f, 0, false });
            resp.actions.push_back("Composant Slider ajouté au canevas.");
        }
        if (lower.find("toggle") != std::string::npos || lower.find("switch") != std::string::npos) {
            studio.AddComponent({ "toggle", "Module d'Accélération", "Traitement en temps réel", 0, 0, true });
            resp.actions.push_back("Composant Toggle Switch ajouté.");
        }
        if (lower.find("bouton") != std::string::npos || lower.find("button") != std::string::npos) {
            studio.AddComponent({ "button", "Exécuter Opération", "", 0, 0, false });
            resp.actions.push_back("Bouton d'Action ajouté.");
        }
        if (lower.find("jauge") != std::string::npos || lower.find("gauge") != std::string::npos || lower.find("radial") != std::string::npos) {
            studio.AddComponent({ "radial_gauge", "Charge Calcul", "", 84.0f, 0, false });
            resp.actions.push_back("Jauge Circulaire (Radial Gauge) ajoutée.");
        }
        if (lower.find("fov") != std::string::npos || lower.find("reticule") != std::string::npos) {
            studio.AddComponent({ "fov_reticle", "Réticule FOV Vectoriel", "", 120.0f, 0, true });
            resp.actions.push_back("Moteur FOV Reticle ajouté.");
        }
        if (lower.find("skin") != std::string::npos || lower.find("cosmetique") != std::string::npos) {
            studio.AddComponent({ "skin_changer", "Inspecteur Cosmétique", "", 0, 0, false });
            resp.actions.push_back("Module Skin Changer Preview ajouté.");
        }

        // 7. Intent: MSBuild Compile
        if (lower.find("compile") != std::string::npos || lower.find("build") != std::string::npos) {
            studio.TriggerMSBuild();
            resp.actions.push_back("Lancement de la compilation MSBuild.");
            resp.text = "J'ai déclenché la compilation complète de votre projet via MSBuild. Les logs s'affichent en temps réel dans l'onglet Compilateur.";
            return resp;
        }

        if (resp.actions.empty()) {
            resp.actions.push_back("Analyse contextuelle effectuée.");
            resp.text = "J'ai bien analysé votre demande. Vous pouvez me demander par exemple : *'Crée un loader moderne'*, *'Mets le thème en or avec coins à 16px'*, *'Anime le bouton en Glow Pulse'*, ou *'Ajoute un slider et compile'*.";
        } else {
            resp.text = "Parfait ! J'ai effectué les modifications demandées sur votre interface dans le canevas en direct.";
        }

        return resp;
    }

} // namespace Solar::Studio
