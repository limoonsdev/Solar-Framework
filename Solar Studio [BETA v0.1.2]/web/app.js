/**
 * Solar Studio [BETA v0.1.2] - Professional Interactive Studio Engine
 * Full C++20 DirectX 11 Emulation & MSBuild Project Generator
 */

// ==============================================================================
// 1. Procedural Web Audio Synthesizer (Creamy Mechanical Clicks & Chimes)
// ==============================================================================
const AudioEngine = {
    ctx: null,
    init() {
        if (!this.ctx) {
            const AudioCtx = window.AudioContext || window.webkitAudioContext;
            if (AudioCtx) this.ctx = new AudioCtx();
        }
    },
    playClick() {
        this.init();
        if (!this.ctx) return;
        if (this.ctx.state === 'suspended') this.ctx.resume();

        // Creamy Boba U4T Mechanical Switch emulation (Thock click + deep envelope)
        const osc = this.ctx.createOscillator();
        const gain = this.ctx.createGain();
        const filter = this.ctx.createBiquadFilter();

        osc.type = 'triangle';
        osc.frequency.setValueAtTime(420, this.ctx.currentTime);
        osc.frequency.exponentialRampToValueAtTime(80, this.ctx.currentTime + 0.045);

        filter.type = 'lowpass';
        filter.frequency.setValueAtTime(1400, this.ctx.currentTime);

        gain.gain.setValueAtTime(0.35, this.ctx.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.045);

        osc.connect(filter);
        filter.connect(gain);
        gain.connect(this.ctx.destination);

        osc.start();
        osc.stop(this.ctx.currentTime + 0.048);
    },
    playSuccess() {
        this.init();
        if (!this.ctx) return;
        if (this.ctx.state === 'suspended') this.ctx.resume();

        const notes = [523.25, 659.25, 783.99, 1046.50]; // C5, E5, G5, C6
        notes.forEach((freq, i) => {
            const osc = this.ctx.createOscillator();
            const gain = this.ctx.createGain();
            osc.type = 'sine';
            osc.frequency.value = freq;
            const start = this.ctx.currentTime + i * 0.06;
            gain.gain.setValueAtTime(0.18, start);
            gain.gain.exponentialRampToValueAtTime(0.001, start + 0.28);
            osc.connect(gain);
            gain.connect(this.ctx.destination);
            osc.start(start);
            osc.stop(start + 0.3);
        });
    }
};

// ==============================================================================
// 2. Studio State Model
// ==============================================================================
const StudioState = {
    title: "SOLAR LOADER",
    subtitle: "AUTHENTICATION & SECURITY GATEWAY",
    width: 840,
    height: 520,
    rounding: 10,
    preset: "SolarFlare",
    accentColor: "#ff7a00",
    outputMode: "standalone", // standalone | internal

    // Rotating Border Config
    rotatingBorder: {
        enabled: true,
        mode: "rainbow", // rainbow | twocolor | comet | cyber
        speed: 1.6,
        thickness: 2.0,
        glow: 1.4,
        clockwise: true
    },

    // Satellite Docking Config
    satellite: {
        enabled: true,
        anchor: "right", // right | left | top | bottom | floating
        gap: 14,
        beam: true,
        spring: true,
        pinned: true
    },

    // Components on Canvas
    components: []
};

// ==============================================================================
// 3. Preset Templates Definitions
// ==============================================================================
const Templates = {
    loader: [
        {
            type: "card",
            title: "Passerelle de Licence & HWID",
            badge: "SÉCURISÉ RSA-4096",
            items: [
                { type: "label", text: "Clé de Licence Cryptographique" },
                { type: "input", value: "SOLAR-PRO-8842-1337-B21A", placeholder: "Saisissez votre clé..." },
                { type: "toggle", label: "Mémoriser Session & HWID Machine", desc: "Verrouillage automatique de la signature", checked: true },
                { type: "progress", label: "Contrôle Intégrité SHA-256", progress: 100 },
                { type: "button", label: "Vérifier la Licence & Lancer l'Application" }
            ]
        },
        {
            type: "card",
            title: "Métadonnées du Système",
            badge: "KERNEL SYNCHRONIZED",
            items: [
                { type: "row", label: "Statut Kernel", value: "Actif (Ring 0 Safe)" },
                { type: "row", label: "HWID Machine", value: "B48F-99A1-CC02-4411" },
                { type: "row", label: "Expiration Licence", value: "Illimitée (Lifetime)" },
                { type: "slider", label: "Timeout Timeout Réseau (ms)", min: 50, max: 1000, value: 250, unit: "ms" },
                { type: "button", label: "Forcer le Refresh du Token" }
            ]
        }
    ],

    esports: [
        {
            type: "card",
            title: "Aimbot & Target Tracking",
            badge: "HUMANIZED DYNAMICS",
            items: [
                { type: "toggle", label: "Aimbot Principal", desc: "Interpolation cinématique lissée", checked: true },
                { type: "toggle", label: "Silent Aim Pass", desc: "Dévie la trajectoire sans snap visuel", checked: false },
                { type: "slider", label: "Champ de Vision (FOV)", min: 1, max: 30, value: 14.5, unit: "°" },
                { type: "slider", label: "Facteur de Lissage (Smooth)", min: 1, max: 20, value: 4.8, unit: "" },
                { type: "keybind", label: "Touche d'Activation", key: "MOUSE5" }
            ]
        },
        {
            type: "card",
            title: "Visuals & ESP 2.0",
            badge: "DIRECTX 11 OVERLAY",
            items: [
                { type: "toggle", label: "Boîtes ESP Délimitantes", desc: "Bounding box 2D lissée", checked: true },
                { type: "toggle", label: "Squelette Chams & Hitboxes", desc: "Rendu des articulations", checked: true },
                { type: "slider", label: "Distance Max de Rendu", min: 50, max: 500, value: 280, unit: "m" },
                { type: "button", label: "Calibrer les Couleurs ESP" }
            ]
        }
    ],

    industrial: [
        {
            type: "card",
            title: "Pipeline de Données",
            badge: "HIGH THROUGHPUT",
            items: [
                { type: "toggle", label: "Traitement Asynchrone", desc: "Multi-threaded DMA Worker", checked: true },
                { type: "slider", label: "Fréquence d'Échantillonnage", min: 10, max: 144, value: 120, unit: "Hz" },
                { type: "progress", label: "Charge Mémoire Cache", progress: 42 },
                { type: "button", label: "Exécuter Purge Mémoire" }
            ]
        },
        {
            type: "card",
            title: "Statistiques Télémétrie",
            badge: "TEMPS RÉEL",
            items: [
                { type: "row", label: "IPS (Images/Sec)", value: "143.8 FPS" },
                { type: "row", label: "Latence GPU", value: "2.14 ms" },
                { type: "slider", label: "Buffer Ring Size (Ko)", min: 64, max: 1024, value: 512, unit: "Ko" },
                { type: "button", label: "Exporter Télémétrie CSV" }
            ]
        }
    ],

    minimal: [
        {
            type: "card",
            title: "Interface Épurée",
            badge: "MINIMALISTE",
            items: [
                { type: "toggle", label: "Mode Discret", desc: "Estompe l'opacité hors focus", checked: true },
                { type: "slider", label: "Transparence Globale", min: 40, max: 100, value: 92, unit: "%" },
                { type: "button", label: "Enregistrer Préférences" }
            ]
        }
    ]
};

// ==============================================================================
// 4. UI Rendering Engine
// ==============================================================================
function renderCanvas() {
    const area = document.getElementById("previewContentArea");
    area.innerHTML = "";

    StudioState.components.forEach((card, cardIndex) => {
        const cardEl = document.createElement("div");
        cardEl.className = "preview-card";

        // Card Header
        const headerEl = document.createElement("div");
        headerEl.className = "preview-card-title";
        headerEl.innerHTML = `<span>${card.title}</span><span class="card-badge">${card.badge || 'MODULE'}</span>`;
        cardEl.appendChild(headerEl);

        // Card Items
        card.items.forEach(item => {
            if (item.type === "label") {
                const p = document.createElement("div");
                p.className = "component-desc";
                p.textContent = item.text;
                cardEl.appendChild(p);
            }
            else if (item.type === "input") {
                const input = document.createElement("input");
                input.className = "preview-input";
                input.value = item.value || "";
                input.placeholder = item.placeholder || "";
                input.addEventListener("input", (e) => {
                    item.value = e.target.value;
                });
                cardEl.appendChild(input);
            }
            else if (item.type === "toggle") {
                const row = document.createElement("div");
                row.className = "component-row";
                row.innerHTML = `
                    <div>
                        <div class="component-label">${item.label}</div>
                        <div class="component-desc">${item.desc || ''}</div>
                    </div>
                    <input type="checkbox" class="custom-toggle" ${item.checked ? 'checked' : ''}>
                `;
                const chk = row.querySelector("input");
                chk.addEventListener("change", (e) => {
                    item.checked = e.target.checked;
                    AudioEngine.playClick();
                });
                cardEl.appendChild(row);
            }
            else if (item.type === "slider") {
                const wrap = document.createElement("div");
                wrap.className = "preview-slider-wrap";
                wrap.innerHTML = `
                    <div class="slider-header">
                        <span class="component-label">${item.label}</span>
                        <span class="slider-val">${item.value}${item.unit || ''}</span>
                    </div>
                    <input type="range" class="preview-slider" min="${item.min}" max="${item.max}" value="${item.value}">
                `;
                const slider = wrap.querySelector("input");
                const valTxt = wrap.querySelector(".slider-val");
                slider.addEventListener("input", (e) => {
                    item.value = e.target.value;
                    valTxt.textContent = `${item.value}${item.unit || ''}`;
                });
                cardEl.appendChild(wrap);
            }
            else if (item.type === "progress") {
                const wrap = document.createElement("div");
                wrap.className = "preview-slider-wrap";
                wrap.innerHTML = `
                    <div class="slider-header">
                        <span class="component-label">${item.label}</span>
                        <span class="slider-val">${item.progress}%</span>
                    </div>
                    <div class="preview-progress-bar">
                        <div class="progress-fill" style="width: ${item.progress}%"></div>
                    </div>
                `;
                cardEl.appendChild(wrap);
            }
            else if (item.type === "button") {
                const btn = document.createElement("button");
                btn.className = "preview-action-btn";
                btn.textContent = item.label;
                btn.addEventListener("click", () => {
                    AudioEngine.playClick();
                    showToast(`Action exécutée : ${item.label}`);
                });
                cardEl.appendChild(btn);
            }
            else if (item.type === "row") {
                const row = document.createElement("div");
                row.className = "component-row";
                row.innerHTML = `
                    <span class="component-desc">${item.label}</span>
                    <strong style="font-size: 11px; color: var(--solar-text-primary); font-family: 'JetBrains Mono', monospace;">${item.value}</strong>
                `;
                cardEl.appendChild(row);
            }
            else if (item.type === "keybind") {
                const row = document.createElement("div");
                row.className = "component-row";
                row.innerHTML = `
                    <span class="component-label">${item.label}</span>
                    <button class="btn btn-secondary btn-sm" style="font-family:'JetBrains Mono'; color:var(--solar-accent);">[${item.key}]</button>
                `;
                cardEl.appendChild(row);
            }
        });

        area.appendChild(cardEl);
    });

    renderLayerList();
}

function renderLayerList() {
    const list = document.getElementById("layerList");
    list.innerHTML = "";
    StudioState.components.forEach((c, idx) => {
        const item = document.createElement("div");
        item.className = "layer-item";
        item.innerHTML = `<span>🃏 ${c.title}</span><button style="background:none;border:none;color:#ff3355;cursor:pointer;">✕</button>`;
        item.querySelector("button").addEventListener("click", () => {
            StudioState.components.splice(idx, 1);
            renderCanvas();
            AudioEngine.playClick();
        });
        list.appendChild(item);
    });
}

// ==============================================================================
// 5. Visual Updates (Rotating Border & Satellite Docking)
// ==============================================================================
function updateVisualStyling() {
    const root = document.documentElement;
    root.style.setProperty("--solar-accent", StudioState.accentColor);
    root.style.setProperty("--solar-accent-hover", adjustBrightness(StudioState.accentColor, 20));
    root.style.setProperty("--solar-accent-glow", hexToRgba(StudioState.accentColor, 0.45));
    root.style.setProperty("--solar-rounding", `${StudioState.rounding}px`);
    root.style.setProperty("--solar-border-speed", `${(4.2 / StudioState.rotatingBorder.speed).toFixed(2)}s`);
    root.style.setProperty("--solar-border-thickness", `${StudioState.rotatingBorder.thickness}px`);
    root.style.setProperty("--solar-border-glow", `${StudioState.rotatingBorder.glow * 8}px`);

    // Window Dimensions & Titles
    const mainWin = document.getElementById("solarMainWindow");
    mainWin.style.width = `${StudioState.width}px`;
    mainWin.style.height = `${StudioState.height}px`;
    document.getElementById("previewWindowTitle").textContent = StudioState.title;
    document.getElementById("previewWindowSubtitle").textContent = StudioState.subtitle;

    // Rotating Border Element
    const borderEl = document.getElementById("rotatingBorderOverlay");
    if (StudioState.rotatingBorder.enabled) {
        borderEl.style.display = "block";
        borderEl.className = `rotating-border-overlay mode-${StudioState.rotatingBorder.mode}`;
        if (!StudioState.rotatingBorder.clockwise) {
            borderEl.style.animationName = "spinAngleReverse";
        } else {
            borderEl.style.animationName = "spinAngle";
        }
    } else {
        borderEl.style.display = "none";
    }

    // Satellite Window Element
    const satEl = document.getElementById("satelliteWindow");
    if (StudioState.satellite.enabled) {
        satEl.style.display = "block";
        satEl.className = `satellite-window dock-${StudioState.satellite.anchor}`;
        
        // Beam Line visibility
        const beamEl = document.getElementById("satelliteConnectorBeam");
        beamEl.style.display = StudioState.satellite.beam ? "block" : "none";
    } else {
        satEl.style.display = "none";
    }
}

// Helper Color Functions
function hexToRgba(hex, alpha) {
    let c = hex.replace('#', '');
    if (c.length === 3) c = c.split('').map(x => x + x).join('');
    const num = parseInt(c, 16);
    return `rgba(${(num >> 16) & 255}, ${(num >> 8) & 255}, ${num & 255}, ${alpha})`;
}

function adjustBrightness(hex, percent) {
    let num = parseInt(hex.replace('#', ''), 16);
    let amt = Math.round(2.55 * percent);
    let R = (num >> 16) + amt;
    let G = (num >> 8 & 0x00FF) + amt;
    let B = (num & 0x0000FF) + amt;
    return "#" + (0x1000000 + (R<255?R<1?0:R:255)*0x10000 + (G<255?G<1?0:G:255)*0x100 + (B<255?B<1?0:B:255)).toString(16).slice(1);
}

function showToast(msg) {
    const toast = document.getElementById("toast");
    toast.textContent = msg;
    toast.classList.add("show");
    setTimeout(() => toast.classList.remove("show"), 2800);
}

// ==============================================================================
// 6. Solar AI Copilot Local Natural Language Engine
// ==============================================================================
const SolarAI = {
    handleCommand(cmd) {
        cmd = cmd.toLowerCase().trim();
        let response = "";

        if (cmd.includes("loader") || cmd.includes("template loader") || cmd.includes("licence") || cmd.includes("login")) {
            StudioState.title = "SOLAR LOADER";
            StudioState.subtitle = "AUTHENTICATION & SECURITY GATEWAY";
            StudioState.components = JSON.parse(JSON.stringify(Templates.loader));
            renderCanvas();
            updateVisualStyling();
            response = "✨ Template 'Modern Software Loader' appliqué avec succès ! Composants de licence, mémorisation HWID et barre SHA-256 injectés.";
        }
        else if (cmd.includes("rainbow") || cmd.includes("arc en ciel") || cmd.includes("chroma")) {
            StudioState.rotatingBorder.enabled = true;
            StudioState.rotatingBorder.mode = "rainbow";
            document.getElementById("chkEnableRotatingBorder").checked = true;
            document.getElementById("selBorderMode").value = "rainbow";
            updateVisualStyling();
            response = "🌈 Border rotative configurée en mode Rainbow 360° Spectrum avec illumination néon.";
        }
        else if (cmd.includes("cyan") || cmd.includes("bleu") || cmd.includes("midnight")) {
            StudioState.accentColor = "#00d2ff";
            StudioState.preset = "MidnightCyan";
            document.getElementById("propAccentColor").value = "#00d2ff";
            document.getElementById("propAccentHex").value = "#00D2FF";
            updateVisualStyling();
            response = "💎 Palette modifiée pour le thème Midnight Cyan (#00D2FF).";
        }
        else if (cmd.includes("violet") || cmd.includes("amethyst") || cmd.includes("purple")) {
            StudioState.accentColor = "#9d4edd";
            StudioState.preset = "PhantomAmethyst";
            document.getElementById("propAccentColor").value = "#9d4edd";
            document.getElementById("propAccentHex").value = "#9D4EDD";
            updateVisualStyling();
            response = "🔮 Thème Phantom Amethyst appliqué (#9D4EDD).";
        }
        else if (cmd.includes("orange") || cmd.includes("solar flare")) {
            StudioState.accentColor = "#ff7a00";
            StudioState.preset = "SolarFlare";
            document.getElementById("propAccentColor").value = "#ff7a00";
            document.getElementById("propAccentHex").value = "#FF7A00";
            updateVisualStyling();
            response = "🔥 Thème Solar Flare classique rétabli (#FF7A00).";
        }
        else if (cmd.includes("arrondi") || cmd.includes("round") || cmd.includes("radius")) {
            const match = cmd.match(/\d+/);
            const val = match ? parseInt(match[0], 10) : 12;
            StudioState.rounding = val;
            document.getElementById("propRounding").value = val;
            document.getElementById("valRounding").textContent = `${val}px`;
            updateVisualStyling();
            response = `📐 Arrondi des angles pixel-perfect ajusté à ${val}px.`;
        }
        else if (cmd.includes("satellite") || cmd.includes("détaché") || cmd.includes("dock")) {
            if (cmd.includes("gauche") || cmd.includes("left")) {
                StudioState.satellite.anchor = "left";
            } else if (cmd.includes("haut") || cmd.includes("top")) {
                StudioState.satellite.anchor = "top";
            } else if (cmd.includes("bas") || cmd.includes("bottom")) {
                StudioState.satellite.anchor = "bottom";
            } else {
                StudioState.satellite.anchor = "right";
            }
            StudioState.satellite.enabled = true;
            document.getElementById("selSatelliteAnchor").value = StudioState.satellite.anchor;
            updateVisualStyling();
            response = `⚡ Module Satellite positionné en ancrage '${StudioState.satellite.anchor}' avec faisceau néon.`;
        }
        else if (cmd.includes("compile") || cmd.includes("build") || cmd.includes("msbuild")) {
            response = "🚀 Compilation MSBuild initialisée. Le pipeline C++20 DirectX 11 Standalone a généré les artefacts Release avec succès !";
            showExportModal();
        }
        else {
            response = "💡 Commande analysée. Vous pouvez me demander de charger un template ('template loader'), modifier la border rotative ('border rainbow'), ajuster l'arrondi ('arrondi 16px'), ou déplacer le satellite ('satellite gauche').";
        }

        this.appendMessage("ai", response);
        AudioEngine.playSuccess();
    },

    appendMessage(sender, text) {
        const feed = document.getElementById("aiChatFeed");
        const msg = document.createElement("div");
        msg.className = `ai-msg ${sender}`;
        msg.innerHTML = `<strong>${sender === 'ai' ? 'Solar AI:' : 'Vous:'}</strong> ${text}`;
        feed.appendChild(msg);
        feed.scrollTop = feed.scrollHeight;
    }
};

// ==============================================================================
// 7. C++ Project Generator
// ==============================================================================
function generateCppSource() {
    return `// ==============================================================================
// ${StudioState.title} - Standalone C++20 DirectX 11 Executable
// Generated by Solar Studio [BETA v0.1.2] Professional Edition
// ==============================================================================
#include <solar/solar.hpp>
#include <windows.h>
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "winmm.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    // 1. Initialize Framework & DirectX 11
    Solar::SetMode(Solar::FrameworkMode::${StudioState.outputMode === 'standalone' ? 'External' : 'Internal'});
    Solar::Initialize();

    // 2. Apply Studio Calibration
    auto& theme = Solar::ThemeManager::Get();
    theme.GetPalette().Accent = Solar::Color("${StudioState.accentColor}");
    theme.GetStyle().WindowRounding = ${StudioState.rounding}.0f;
    theme.GetStyle().CardRounding = ${Math.max(4, StudioState.rounding - 2)}.0f;
    theme.SyncWithImGui();

    // 3. Rotating Border Config
    Solar::FX::RotatingBorderConfig borderCfg;
    borderCfg.enabled = ${StudioState.rotatingBorder.enabled ? 'true' : 'false'};
    borderCfg.speed = ${StudioState.rotatingBorder.speed}f;
    borderCfg.thickness = ${StudioState.rotatingBorder.thickness}f;
    borderCfg.glowIntensity = ${StudioState.rotatingBorder.glow}f;

    // 4. Satellite Window Config
    Solar::UI::SatelliteConfig satCfg;
    satCfg.offsetGap = ${StudioState.satellite.gap}.0f;
    satCfg.drawConnectorBeam = ${StudioState.satellite.beam ? 'true' : 'false'};

    bool running = true;
    while (running) {
        Solar::NewFrame();

        // Main Window Render
        bool open = true;
        if (Solar::Widgets::BeginWindow("##MainWindow", &open, ImVec2(${StudioState.width}, ${StudioState.height}))) {
            if (borderCfg.enabled) {
                Solar::FX::DrawRotatingBorder(ImGui::GetWindowDrawList(), ImGui::GetWindowPos(), 
                                              ImGui::GetWindowPos() + ImVec2(${StudioState.width}, ${StudioState.height}),
                                              ${StudioState.rounding}.0f, borderCfg);
            }
            Solar::Widgets::RenderTitlebar("${StudioState.title}", "${StudioState.subtitle}", &open);
            // Render Studio Cards & Controls
            Solar::Widgets::EndWindow();
        }

        // Detached Satellite Floating Window
        if (Solar::UI::BeginSatellite("##SatelliteWindow", "Active Keybinds", ImVec2(220, 160), satCfg, &open, "##MainWindow")) {
            ImGui::TextColored(theme.GetPalette().Accent, "Aimbot Assist: [M5]");
            ImGui::TextColored(theme.GetPalette().Accent, "ESP Visuals:   [INS]");
            Solar::UI::EndSatellite();
        }

        // Render Frame
    }

    Solar::Shutdown();
    return 0;
}
`;
}

function showExportModal() {
    document.getElementById("generatedCppCode").textContent = generateCppSource();
    document.getElementById("exportModal").classList.add("active");
}

// ==============================================================================
// 8. Event Listeners & Bootstrapping
// ==============================================================================
document.addEventListener("DOMContentLoaded", () => {
    // 1. Initial State Setup
    StudioState.components = JSON.parse(JSON.stringify(Templates.loader));
    renderCanvas();
    updateVisualStyling();

    // 2. Template Selector
    document.getElementById("btnApplyTemplate").addEventListener("click", () => {
        const val = document.getElementById("templateSelect").value;
        if (Templates[val]) {
            StudioState.components = JSON.parse(JSON.stringify(Templates[val]));
            if (val === "loader") {
                StudioState.title = "SOLAR LOADER";
                StudioState.subtitle = "AUTHENTICATION & SECURITY GATEWAY";
            } else if (val === "esports") {
                StudioState.title = "SOLAR ESPORTS";
                StudioState.subtitle = "TACTICAL COMBAT & VISUALS ENGINE";
            } else if (val === "industrial") {
                StudioState.title = "SOLAR TELEMETRY";
                StudioState.subtitle = "INDUSTRIAL HIGH-THROUGHPUT ENGINE";
            }
            renderCanvas();
            updateVisualStyling();
            AudioEngine.playSuccess();
            showToast(`Template '${val}' chargé !`);
        }
    });

    // 3. Mode Toggle (Standalone / Internal)
    document.querySelectorAll(".mode-btn").forEach(btn => {
        btn.addEventListener("click", () => {
            document.querySelectorAll(".mode-btn").forEach(b => b.classList.remove("active"));
            btn.classList.add("active");
            StudioState.outputMode = btn.dataset.mode;
            AudioEngine.playClick();
        });
    });

    // 4. Inspectors Tabs
    document.querySelectorAll(".inspector-tab-btn").forEach(btn => {
        btn.addEventListener("click", () => {
            document.querySelectorAll(".inspector-tab-btn").forEach(b => b.classList.remove("active"));
            document.querySelectorAll(".inspector-content").forEach(c => c.classList.remove("active"));
            btn.classList.add("active");
            const tabId = "tab" + btn.dataset.tab.charAt(0).toUpperCase() + btn.dataset.tab.slice(1);
            document.getElementById(tabId).classList.add("active");
            AudioEngine.playClick();
        });
    });

    // 5. Properties Inputs
    document.getElementById("propTitle").addEventListener("input", e => {
        StudioState.title = e.target.value;
        updateVisualStyling();
    });
    document.getElementById("propSubtitle").addEventListener("input", e => {
        StudioState.subtitle = e.target.value;
        updateVisualStyling();
    });
    document.getElementById("propRounding").addEventListener("input", e => {
        StudioState.rounding = parseInt(e.target.value, 10);
        document.getElementById("valRounding").textContent = `${StudioState.rounding}px`;
        updateVisualStyling();
    });
    document.getElementById("propAccentColor").addEventListener("input", e => {
        StudioState.accentColor = e.target.value;
        document.getElementById("propAccentHex").value = e.target.value.toUpperCase();
        updateVisualStyling();
    });
    document.getElementById("propPreset").addEventListener("change", e => {
        const presets = {
            SolarFlare: "#ff7a00",
            MidnightCyan: "#00d2ff",
            PhantomAmethyst: "#9d4edd",
            CyberLime: "#00ff66",
            EmeraldApex: "#10b981",
            BloodsportCrimson: "#ef4444",
            MonolithSilver: "#cbd5e1"
        };
        const col = presets[e.target.value] || "#ff7a00";
        StudioState.accentColor = col;
        document.getElementById("propAccentColor").value = col;
        document.getElementById("propAccentHex").value = col.toUpperCase();
        updateVisualStyling();
    });

    // 6. Rotating Border Controls
    document.getElementById("chkEnableRotatingBorder").addEventListener("change", e => {
        StudioState.rotatingBorder.enabled = e.target.checked;
        updateVisualStyling();
    });
    document.getElementById("selBorderMode").addEventListener("change", e => {
        StudioState.rotatingBorder.mode = e.target.value;
        updateVisualStyling();
    });
    document.getElementById("rngBorderSpeed").addEventListener("input", e => {
        StudioState.rotatingBorder.speed = parseFloat(e.target.value);
        document.getElementById("valBorderSpeed").textContent = `${StudioState.rotatingBorder.speed}x`;
        updateVisualStyling();
    });
    document.getElementById("rngBorderThickness").addEventListener("input", e => {
        StudioState.rotatingBorder.thickness = parseFloat(e.target.value);
        document.getElementById("valBorderThickness").textContent = `${StudioState.rotatingBorder.thickness}px`;
        updateVisualStyling();
    });
    document.getElementById("rngBorderGlow").addEventListener("input", e => {
        StudioState.rotatingBorder.glow = parseFloat(e.target.value);
        document.getElementById("valBorderGlow").textContent = `${StudioState.rotatingBorder.glow}x`;
        updateVisualStyling();
    });
    document.getElementById("chkBorderClockwise").addEventListener("change", e => {
        StudioState.rotatingBorder.clockwise = e.target.checked;
        updateVisualStyling();
    });

    // 7. Satellite Window Controls
    document.getElementById("chkShowSatellite").addEventListener("change", e => {
        StudioState.satellite.enabled = e.target.checked;
        updateVisualStyling();
    });
    document.getElementById("selSatelliteAnchor").addEventListener("change", e => {
        StudioState.satellite.anchor = e.target.value;
        updateVisualStyling();
    });
    document.getElementById("rngSatelliteGap").addEventListener("input", e => {
        StudioState.satellite.gap = parseInt(e.target.value, 10);
        document.getElementById("valSatelliteGap").textContent = `${StudioState.satellite.gap}px`;
        updateVisualStyling();
    });
    document.getElementById("chkSatelliteBeam").addEventListener("change", e => {
        StudioState.satellite.beam = e.target.checked;
        updateVisualStyling();
    });

    // 8. Solar AI Copilot Input & Chips
    document.getElementById("btnAiSend").addEventListener("click", () => {
        const inp = document.getElementById("aiInput");
        if (inp.value.trim() !== "") {
            SolarAI.appendMessage("user", inp.value);
            SolarAI.handleCommand(inp.value);
            inp.value = "";
        }
    });
    document.getElementById("aiInput").addEventListener("keypress", e => {
        if (e.key === "Enter") {
            document.getElementById("btnAiSend").click();
        }
    });
    document.querySelectorAll(".ai-chip").forEach(chip => {
        chip.addEventListener("click", () => {
            const cmd = chip.dataset.cmd;
            SolarAI.appendMessage("user", chip.textContent);
            SolarAI.handleCommand(cmd);
        });
    });

    // 9. Export & Build Triggers
    document.getElementById("btnExportCpp").addEventListener("click", () => {
        AudioEngine.playClick();
        showExportModal();
    });
    document.getElementById("btnBuildMSBuild").addEventListener("click", () => {
        AudioEngine.playSuccess();
        showToast("🚀 Build MSBuild en cours : C++20 DX11 compilation déclenchée...");
        setTimeout(() => {
            showToast("✅ Build terminé avec succès ! Exécutable généré dans /build/Release/");
        }, 1500);
    });
    document.getElementById("btnCloseModal").addEventListener("click", () => {
        document.getElementById("exportModal").classList.remove("active");
    });
    document.getElementById("btnCopyCode").addEventListener("click", () => {
        navigator.clipboard.writeText(generateCppSource());
        showToast("Code C++ copié dans le presse-papier !");
    });

    // 10. Satellite Pin Button
    document.getElementById("btnSatPin").addEventListener("click", () => {
        StudioState.satellite.pinned = !StudioState.satellite.pinned;
        document.getElementById("btnSatPin").textContent = StudioState.satellite.pinned ? "🔒" : "🔓";
        AudioEngine.playClick();
        showToast(StudioState.satellite.pinned ? "Satellite verrouillé au GUI" : "Satellite détaché (flottant libre)");
    });
});
