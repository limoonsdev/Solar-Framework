#include <windows.h>
#include <timeapi.h>
#include <d3d11.h>

#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <sstream>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "solar/solar.hpp"
#include "solar/font_awesome.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "winmm.lib")

using namespace DirectX;

// ==============================================================================
// 1. Unreal Engine 5 Internal Emulation Data Structures
// ==============================================================================
#pragma pack(push, 8)

struct FVector {
    float X, Y, Z;
    FVector() : X(0), Y(0), Z(0) {}
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
};

struct FRotator {
    float Pitch, Yaw, Roll;
    FRotator() : Pitch(0), Yaw(0), Roll(0) {}
    FRotator(float p, float y, float r) : Pitch(p), Yaw(y), Roll(r) {}
};

struct UObject {
    void* VTable;
    int32_t ObjectFlags;
    int32_t InternalIndex;
    void* ClassPrivate;
    const char* NamePrivate;
};

struct AActor : UObject {
    FVector RootLocation;
    FRotator RootRotation;
    float Health;
    float MaxHealth;
    bool bIsEnemy;
    bool bIsDormant;
    const char* CharacterName;
    FVector HeadBone;
    FVector ChestBone;
    FVector PelvisBone;
};

struct ULevel : UObject {
    std::vector<AActor*> Actors;
};

struct APlayerCameraManager : UObject {
    FVector CameraLocation;
    FRotator CameraRotation;
    float FieldOfView;
};

struct APlayerController : UObject {
    APlayerCameraManager* PlayerCameraManager;
    AActor* AcknowledgedPawn;
};

struct ULocalPlayer : UObject {
    APlayerController* PlayerController;
};

struct UGameInstance : UObject {
    std::vector<ULocalPlayer*> LocalPlayers;
};

struct UWorld : UObject {
    ULevel* PersistentLevel;
    UGameInstance* OwningGameInstance;
    float TimeSeconds;
};

#pragma pack(pop)

// Exported Unreal Engine Globals for Internal DLL Injection Testing
extern "C" {
    __declspec(dllexport) UWorld* GWorld = nullptr;
    __declspec(dllexport) void* GObjects = (void*)0x7FF700100000;
    __declspec(dllexport) void* GNames = (void*)0x7FF700200000;
    __declspec(dllexport) UWorld* GetUWorld() { return GWorld; }
    __declspec(dllexport) IDXGISwapChain* g_ExportedSwapChain = nullptr;
}

// Memory signature pattern: 48 8B 05 ? ? ? ? 48 85 C0
__declspec(noinline) void* SigScanTargetGWorld() {
    volatile void* p = GWorld;
    if (p != nullptr) return (void*)p;
    return nullptr;
}

// ==============================================================================
// 2. Direct3D 11 Render Pipeline & 3D Shaders
// ==============================================================================
struct SceneConstantBuffer {
    XMMATRIX mWorldViewProj;
    XMMATRIX mWorld;
    XMFLOAT4 vColor;
    XMFLOAT4 vLightDir;
    XMFLOAT4 vEyePos;
    XMFLOAT4 vParams; // x: ambient, y: specular, z: unlit flag, w: gamma
};

struct SkyConstantBuffer {
    XMMATRIX mInvViewProj;
    XMFLOAT4 vEyePos;
    XMFLOAT4 vSunDir;
    XMFLOAT4 vSkyZenith;
    XMFLOAT4 vSkyHorizon;
};

struct Vertex {
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
};

static const char* g_SceneShaderSource = R"(
cbuffer SceneConstantBuffer : register(b0) {
    matrix WorldViewProj;
    matrix World;
    float4 Color;
    float4 LightDir;
    float4 EyePos;
    float4 Params; // x: ambient, y: specular, z: unlit flag, w: gamma
};

struct VS_INPUT {
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 Normal : TEXCOORD0;
    float4 Color : COLOR0;
    float3 WorldPos : TEXCOORD1;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), WorldViewProj);
    output.Normal = mul(float4(input.Normal, 0.0f), World).xyz;
    output.Color = Color;
    output.WorldPos = mul(float4(input.Pos, 1.0f), World).xyz;
    return output;
}

float4 PS(PS_INPUT input) : SV_Target {
    if (Params.z > 0.5f) {
        return input.Color;
    }
    float3 N = normalize(input.Normal);
    float3 L = normalize(-LightDir.xyz);
    float3 V = normalize(EyePos.xyz - input.WorldPos);
    float diff = max(dot(N, L), 0.0f);
    float3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0f), 24.0f) * Params.y;
    float3 lit = input.Color.rgb * (Params.x + diff * (1.0f - Params.x)) + spec * float3(1.0f, 0.95f, 0.85f);
    return float4(lit, input.Color.a);
}
)";

static const char* g_SkyShaderSource = R"(
cbuffer SkyConstantBuffer : register(b0) {
    matrix InvViewProj;
    float4 EyePos;
    float4 SunDir;
    float4 SkyZenith;
    float4 SkyHorizon;
};

struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 UV  : TEXCOORD0;
};

VS_OUTPUT SkyVS(uint id : SV_VertexID) {
    VS_OUTPUT o;
    o.UV = float2((id == 2) ? 2.0f : 0.0f, (id == 1) ? 2.0f : 0.0f);
    o.Pos = float4(o.UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 1.0f, 1.0f);
    return o;
}

float4 SkyPS(VS_OUTPUT input) : SV_Target {
    float4 ndc = float4(input.Pos.x, input.Pos.y, 1.0f, 1.0f);
    float4 worldPos = mul(ndc, InvViewProj);
    worldPos /= worldPos.w;
    float3 viewDir = normalize(worldPos.xyz - EyePos.xyz);

    float t = clamp(viewDir.y * 0.5f + 0.5f, 0.0f, 1.0f);
    float3 sky = lerp(SkyHorizon.rgb, SkyZenith.rgb, pow(t, 0.65f));

    // Sun disc highlight
    float3 sun = normalize(-SunDir.xyz);
    float sunDot = max(dot(viewDir, sun), 0.0f);
    float sunDisc = pow(sunDot, 256.0f) * 1.8f;
    float sunHalo = pow(sunDot, 16.0f) * 0.25f;
    sky += (sunDisc + sunHalo) * float3(1.0f, 0.95f, 0.80f);

    return float4(sky, 1.0f);
}
)";

// Global D3D11 Objects
static HWND                     g_hWnd = nullptr;
static ID3D11Device*            g_pDevice = nullptr;
static ID3D11DeviceContext*     g_pContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static ID3D11RenderTargetView*  g_pRenderTargetView = nullptr;
static ID3D11Texture2D*         g_pDepthStencilBuffer = nullptr;
static ID3D11DepthStencilView*  g_pDepthStencilView = nullptr;
static ID3D11VertexShader*      g_pSceneVS = nullptr;
static ID3D11PixelShader*       g_pScenePS = nullptr;
static ID3D11InputLayout*       g_pInputLayout = nullptr;
static ID3D11Buffer*            g_pCubeVertexBuffer = nullptr;
static ID3D11Buffer*            g_pCubeIndexBuffer = nullptr;
static ID3D11Buffer*            g_pSceneConstantBuffer = nullptr;
static ID3D11Buffer*            g_pSkyConstantBuffer = nullptr;
static ID3D11VertexShader*      g_pSkyVS = nullptr;
static ID3D11PixelShader*       g_pSkyPS = nullptr;
static ID3D11RasterizerState*   g_pRasterizerSolid = nullptr;
static ID3D11RasterizerState*   g_pRasterizerWireframe = nullptr;
static ID3D11DepthStencilState* g_pDepthStateDefault = nullptr;
static ID3D11DepthStencilState* g_pDepthStateSky = nullptr;

// 3D FPS Camera State & Viewport
enum class ViewMode { Lit = 0, Wireframe, Unlit };

static XMFLOAT3 g_CamPos(0.0f, 2.0f, -8.0f);
static float    g_CamPitch = 0.0f;
static float    g_CamYaw = 0.0f;
static float    g_FieldOfView = 85.0f;
static float    g_MouseSensitivity = 0.0022f;
static float    g_TimeDilation = 1.0f;
static float    g_Gamma = 1.0f;
static bool     g_MouseCaptured = true;
static bool     g_ShowSkyAtmosphere = true;
static bool     g_GodMode = false;
static ViewMode g_ViewMode = ViewMode::Lit;
static int      g_FpsCapMode = 0; // 0: VSync, 1: 60, 2: 120, 3: 144, 4: 240, 5: Uncapped

// Interactive UI State (Console, Menu, HUD)
static bool g_ShowDevConsole = false;
static bool g_ShowPauseMenu = false;
static bool g_ShowTacticalHUD = true;
static bool g_ShowFpsStat = true;
static bool g_ShowUnitStat = false;

static char g_ConsoleInput[256] = "";
static std::vector<std::string> g_ConsoleLogs;
static std::vector<std::string> g_ConsoleHistory;
static int g_ConsoleHistoryIdx = -1;

// Injected DLL Tracker
struct InjectedModuleInfo {
    std::string name;
    uintptr_t baseAddress;
    float detectionTime;
};
static std::vector<std::string> g_BaselineModules;
static std::vector<InjectedModuleInfo> g_InjectedModules;
static char g_LastInjectedNotification[256] = "";
static float g_NotificationTimer = 0.0f;

// World & Actors
static UWorld s_SimulatedWorld;
static ULevel s_SimulatedLevel;
static UGameInstance s_SimulatedGameInstance;
static ULocalPlayer s_SimulatedLocalPlayer;
static APlayerController s_SimulatedPlayerController;
static APlayerCameraManager s_SimulatedCameraManager;
static std::vector<AActor> s_DummyBots;

// Forward Declarations
static void AddConsoleLog(const std::string& text);
static void ExecuteConsoleCommand(const std::string& cmd);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void InitializeSimulatedWorld() {
    s_DummyBots.resize(5);
    const char* names[] = { "TargetDummy_Alpha", "TargetDummy_Bravo", "TargetDummy_Charlie", "TargetDummy_Delta", "TargetDummy_Sniper" };
    FVector positions[] = {
        { -4.0f, 1.2f, 8.0f },
        { 0.0f, 1.2f, 12.0f },
        { 4.5f, 1.2f, 9.5f },
        { -8.0f, 1.2f, 15.0f },
        { 7.0f, 1.2f, 16.0f }
    };

    s_SimulatedLevel.Actors.clear();
    for (size_t i = 0; i < s_DummyBots.size(); ++i) {
        s_DummyBots[i].CharacterName = names[i];
        s_DummyBots[i].RootLocation = positions[i];
        s_DummyBots[i].RootRotation = FRotator(0.0f, 180.0f, 0.0f);
        s_DummyBots[i].Health = 100.0f;
        s_DummyBots[i].MaxHealth = 100.0f;
        s_DummyBots[i].bIsEnemy = (i != 0);
        s_DummyBots[i].bIsDormant = false;
        s_DummyBots[i].HeadBone = FVector(positions[i].X, positions[i].Y + 1.65f, positions[i].Z);
        s_DummyBots[i].ChestBone = FVector(positions[i].X, positions[i].Y + 1.15f, positions[i].Z);
        s_DummyBots[i].PelvisBone = FVector(positions[i].X, positions[i].Y + 0.65f, positions[i].Z);
        s_SimulatedLevel.Actors.push_back(&s_DummyBots[i]);
    }

    s_SimulatedCameraManager.CameraLocation = FVector(g_CamPos.x, g_CamPos.y, g_CamPos.z);
    s_SimulatedCameraManager.CameraRotation = FRotator(g_CamPitch, g_CamYaw, 0.0f);
    s_SimulatedCameraManager.FieldOfView = g_FieldOfView;

    s_SimulatedPlayerController.PlayerCameraManager = &s_SimulatedCameraManager;
    s_SimulatedPlayerController.AcknowledgedPawn = nullptr;

    s_SimulatedLocalPlayer.PlayerController = &s_SimulatedPlayerController;
    s_SimulatedGameInstance.LocalPlayers.push_back(&s_SimulatedLocalPlayer);

    s_SimulatedWorld.PersistentLevel = &s_SimulatedLevel;
    s_SimulatedWorld.OwningGameInstance = &s_SimulatedGameInstance;
    s_SimulatedWorld.TimeSeconds = 0.0f;

    GWorld = &s_SimulatedWorld;

    AddConsoleLog("[LogInit] Unreal Engine 5.4.2 Simulated Testbed Initialized.");
    AddConsoleLog("[LogWorld] Level loaded: /Game/Maps/Solar_Testbed_P");
    AddConsoleLog("[LogWorld] 5 Target Dummy actors spawned and registered.");
}

static void SnapshotBaselineModules() {
    g_BaselineModules.clear();
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 me = { sizeof(me) };
        if (Module32First(hSnap, &me)) {
            do {
                g_BaselineModules.push_back(me.szModule);
            } while (Module32Next(hSnap, &me));
        }
        CloseHandle(hSnap);
    }
}

static void CheckForInjectedModules(float dt) {
    g_NotificationTimer = (std::max)(0.0f, g_NotificationTimer - dt);

    static float s_checkTimer = 0.0f;
    s_checkTimer += dt;
    if (s_checkTimer < 0.5f) return;
    s_checkTimer = 0.0f;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 me = { sizeof(me) };
        if (Module32First(hSnap, &me)) {
            do {
                std::string modName = me.szModule;
                bool isBaseline = false;
                for (const auto& b : g_BaselineModules) {
                    if (_stricmp(b.c_str(), modName.c_str()) == 0) {
                        isBaseline = true;
                        break;
                    }
                }
                if (!isBaseline) {
                    bool alreadyLogged = false;
                    for (const auto& inj : g_InjectedModules) {
                        if (_stricmp(inj.name.c_str(), modName.c_str()) == 0) {
                            alreadyLogged = true;
                            break;
                        }
                    }
                    if (!alreadyLogged) {
                        g_InjectedModules.push_back({ modName, (uintptr_t)me.modBaseAddr, 0.0f });
                        snprintf(g_LastInjectedNotification, sizeof(g_LastInjectedNotification),
                                 "[INJECTION DETECTED] %s loaded at 0x%p", modName.c_str(), me.modBaseAddr);
                        g_NotificationTimer = 6.0f;
                        AddConsoleLog(g_LastInjectedNotification);
                        printf("\n>>> %s\n", g_LastInjectedNotification);
                    }
                }
            } while (Module32Next(hSnap, &me));
        }
        CloseHandle(hSnap);
    }
}

static bool InitD3D(HWND hWnd) {
    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        featureLevels, 1, D3D11_SDK_VERSION, &sd,
        &g_pSwapChain, &g_pDevice, &featureLevel, &g_pContext
    );
    if (FAILED(hr)) return false;

    g_ExportedSwapChain = g_pSwapChain;

    // Render Target View
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
    g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();

    // Depth Stencil Buffer & View
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    g_pDevice->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencilBuffer);
    g_pDevice->CreateDepthStencilView(g_pDepthStencilBuffer, nullptr, &g_pDepthStencilView);

    // Rasterizer States (Solid and Wireframe)
    D3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.DepthClipEnable = TRUE;
    g_pDevice->CreateRasterizerState(&rastDesc, &g_pRasterizerSolid);

    rastDesc.FillMode = D3D11_FILL_WIREFRAME;
    g_pDevice->CreateRasterizerState(&rastDesc, &g_pRasterizerWireframe);

    // Depth Stencil States
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    g_pDevice->CreateDepthStencilState(&dsDesc, &g_pDepthStateDefault);

    dsDesc.DepthEnable = FALSE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    g_pDevice->CreateDepthStencilState(&dsDesc, &g_pDepthStateSky);

    // Compile Scene Shaders
    ID3DBlob* pVSBlob = nullptr;
    ID3DBlob* pPSBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    hr = D3DCompile(g_SceneShaderSource, strlen(g_SceneShaderSource), "VS", nullptr, nullptr, "VS", "vs_4_0", 0, 0, &pVSBlob, &pErrorBlob);
    if (FAILED(hr)) return false;
    hr = D3DCompile(g_SceneShaderSource, strlen(g_SceneShaderSource), "PS", nullptr, nullptr, "PS", "ps_4_0", 0, 0, &pPSBlob, &pErrorBlob);
    if (FAILED(hr)) return false;

    g_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pSceneVS);
    g_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pScenePS);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    g_pDevice->CreateInputLayout(layout, 2, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pInputLayout);
    pVSBlob->Release();
    pPSBlob->Release();

    // Compile Sky Shaders
    hr = D3DCompile(g_SkyShaderSource, strlen(g_SkyShaderSource), "SkyVS", nullptr, nullptr, "SkyVS", "vs_4_0", 0, 0, &pVSBlob, &pErrorBlob);
    if (SUCCEEDED(hr)) {
        g_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pSkyVS);
        pVSBlob->Release();
    }
    hr = D3DCompile(g_SkyShaderSource, strlen(g_SkyShaderSource), "SkyPS", nullptr, nullptr, "SkyPS", "ps_4_0", 0, 0, &pPSBlob, &pErrorBlob);
    if (SUCCEEDED(hr)) {
        g_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pSkyPS);
        pPSBlob->Release();
    }

    // Cube Mesh Vertices
    Vertex cubeVertices[] = {
        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0, 0, -1) },
        { XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(0, 0, -1) },
        { XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(0, 0, -1) },
        { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0, 0, -1) },

        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0, 0, 1) },
        { XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(0, 0, 1) },
        { XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(0, 0, 1) },
        { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0, 0, 1) },

        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1, 0, 0) },
        { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(-1, 0, 0) },
        { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(-1, 0, 0) },
        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(-1, 0, 0) },

        { XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1, 0, 0) },
        { XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1, 0, 0) },
        { XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1, 0, 0) },
        { XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1, 0, 0) },

        { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0, 1, 0) },
        { XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(0, 1, 0) },
        { XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(0, 1, 0) },
        { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0, 1, 0) },

        { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0, -1, 0) },
        { XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(0, -1, 0) },
        { XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(0, -1, 0) },
        { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0, -1, 0) },
    };

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(cubeVertices);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vData = { cubeVertices };
    g_pDevice->CreateBuffer(&vbd, &vData, &g_pCubeVertexBuffer);

    WORD cubeIndices[] = {
        0, 2, 1, 0, 3, 2,
        4, 5, 6, 4, 6, 7,
        8, 10, 9, 8, 11, 10,
        12, 13, 14, 12, 14, 15,
        16, 18, 17, 16, 19, 18,
        20, 21, 22, 20, 22, 23
    };

    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(cubeIndices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA iData = { cubeIndices };
    g_pDevice->CreateBuffer(&ibd, &iData, &g_pCubeIndexBuffer);

    // Constant Buffers
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(SceneConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    g_pDevice->CreateBuffer(&cbd, nullptr, &g_pSceneConstantBuffer);

    cbd.ByteWidth = sizeof(SkyConstantBuffer);
    g_pDevice->CreateBuffer(&cbd, nullptr, &g_pSkyConstantBuffer);

    return true;
}

static void CleanD3D() {
    if (g_pSkyConstantBuffer) g_pSkyConstantBuffer->Release();
    if (g_pSceneConstantBuffer) g_pSceneConstantBuffer->Release();
    if (g_pCubeIndexBuffer) g_pCubeIndexBuffer->Release();
    if (g_pCubeVertexBuffer) g_pCubeVertexBuffer->Release();
    if (g_pInputLayout) g_pInputLayout->Release();
    if (g_pSkyPS) g_pSkyPS->Release();
    if (g_pSkyVS) g_pSkyVS->Release();
    if (g_pScenePS) g_pScenePS->Release();
    if (g_pSceneVS) g_pSceneVS->Release();
    if (g_pDepthStateSky) g_pDepthStateSky->Release();
    if (g_pDepthStateDefault) g_pDepthStateDefault->Release();
    if (g_pRasterizerWireframe) g_pRasterizerWireframe->Release();
    if (g_pRasterizerSolid) g_pRasterizerSolid->Release();
    if (g_pDepthStencilView) g_pDepthStencilView->Release();
    if (g_pDepthStencilBuffer) g_pDepthStencilBuffer->Release();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pContext) g_pContext->Release();
    if (g_pDevice) g_pDevice->Release();
}

static void AddConsoleLog(const std::string& text) {
    g_ConsoleLogs.push_back(text);
    if (g_ConsoleLogs.size() > 200) {
        g_ConsoleLogs.erase(g_ConsoleLogs.begin());
    }
}

static void ExecuteConsoleCommand(const std::string& rawCmd) {
    if (rawCmd.empty()) return;
    g_ConsoleHistory.push_back(rawCmd);
    g_ConsoleHistoryIdx = -1;
    AddConsoleLog("> " + rawCmd);

    std::string cmd = rawCmd;
    for (auto& c : cmd) c = (char)tolower((unsigned char)c);

    std::stringstream ss(cmd);
    std::string action;
    ss >> action;

    if (action == "help" || action == "?") {
        AddConsoleLog("=== Unreal Engine Simulated Console Commands ===");
        AddConsoleLog("stat fps                 - Toggle framerate display");
        AddConsoleLog("stat unit                - Toggle CPU/GPU frame time stats");
        AddConsoleLog("stat hud                 - Toggle tactical combat HUD");
        AddConsoleLog("viewmode wireframe       - Render geometry in wireframe");
        AddConsoleLog("viewmode lit             - Render geometry fully lit");
        AddConsoleLog("viewmode unlit           - Render geometry unlit");
        AddConsoleLog("fov <degrees>            - Set camera field of view (e.g. fov 90)");
        AddConsoleLog("gamma <value>            - Set scene exposure (e.g. gamma 1.2)");
        AddConsoleLog("slomo <multiplier>       - Time dilation (e.g. slomo 0.5)");
        AddConsoleLog("god                      - Toggle actor godmode");
        AddConsoleLog("teleport                 - Reset player location to spawn");
        AddConsoleLog("spawn                    - Spawn additional target dummy actor");
        AddConsoleLog("modules                  - Inspect injected DLL modules");
        AddConsoleLog("inject <dll_name>        - Load module via LoadLibraryA");
        AddConsoleLog("cls / clear              - Clear console buffer");
        AddConsoleLog("quit / exit              - Exit simulation application");
    } else if (action == "stat") {
        std::string sub;
        ss >> sub;
        if (sub == "fps") {
            g_ShowFpsStat = !g_ShowFpsStat;
            AddConsoleLog(std::string("[LogStat] FPS overlay: ") + (g_ShowFpsStat ? "Enabled" : "Disabled"));
        } else if (sub == "unit") {
            g_ShowUnitStat = !g_ShowUnitStat;
            AddConsoleLog(std::string("[LogStat] Unit stats: ") + (g_ShowUnitStat ? "Enabled" : "Disabled"));
        } else if (sub == "hud") {
            g_ShowTacticalHUD = !g_ShowTacticalHUD;
            AddConsoleLog(std::string("[LogStat] Tactical HUD: ") + (g_ShowTacticalHUD ? "Enabled" : "Disabled"));
        } else {
            AddConsoleLog("[LogStat] Unknown stat command. Available: fps, unit, hud");
        }
    } else if (action == "viewmode") {
        std::string mode;
        ss >> mode;
        if (mode == "wireframe") {
            g_ViewMode = ViewMode::Wireframe;
            AddConsoleLog("[LogRHI] ViewMode set to Wireframe.");
        } else if (mode == "lit") {
            g_ViewMode = ViewMode::Lit;
            AddConsoleLog("[LogRHI] ViewMode set to Lit (Default).");
        } else if (mode == "unlit") {
            g_ViewMode = ViewMode::Unlit;
            AddConsoleLog("[LogRHI] ViewMode set to Unlit.");
        } else {
            AddConsoleLog("[LogRHI] Unknown viewmode. Available: lit, wireframe, unlit");
        }
    } else if (action == "fov") {
        float val = 0.0f;
        if (ss >> val && val >= 30.0f && val <= 140.0f) {
            g_FieldOfView = val;
            s_SimulatedCameraManager.FieldOfView = val;
            AddConsoleLog("[LogCamera] Field of View updated to " + std::to_string((int)val) + " deg.");
        } else {
            AddConsoleLog("[LogCamera] Invalid FOV. Valid range: 30 to 140.");
        }
    } else if (action == "gamma") {
        float val = 1.0f;
        if (ss >> val && val >= 0.2f && val <= 3.0f) {
            g_Gamma = val;
            AddConsoleLog("[LogRHI] Gamma updated to " + std::to_string(val));
        } else {
            AddConsoleLog("[LogRHI] Invalid Gamma value. Valid range: 0.2 to 3.0.");
        }
    } else if (action == "slomo") {
        float val = 1.0f;
        if (ss >> val && val >= 0.05f && val <= 5.0f) {
            g_TimeDilation = val;
            AddConsoleLog("[LogWorld] Time dilation factor set to " + std::to_string(val) + "x.");
        } else {
            AddConsoleLog("[LogWorld] Invalid slomo factor. Range: 0.05 to 5.0.");
        }
    } else if (action == "god") {
        g_GodMode = !g_GodMode;
        AddConsoleLog(std::string("[LogPlayer] GodMode: ") + (g_GodMode ? "ACTIVE (Infinite Health)" : "OFF"));
    } else if (action == "teleport") {
        g_CamPos = XMFLOAT3(0.0f, 2.0f, -8.0f);
        g_CamPitch = 0.0f;
        g_CamYaw = 0.0f;
        AddConsoleLog("[LogPlayer] Teleported camera to spawn origin (0.0, 2.0, -8.0).");
    } else if (action == "spawn") {
        AActor extraBot;
        static int s_spawnCounter = 1;
        std::string name = "TargetDummy_Spawned_" + std::to_string(s_spawnCounter++);
        extraBot.CharacterName = _strdup(name.c_str());
        extraBot.RootLocation = FVector(g_CamPos.x + std::sin(g_CamYaw) * 6.0f, 1.2f, g_CamPos.z + std::cos(g_CamYaw) * 6.0f);
        extraBot.RootRotation = FRotator(0.0f, 180.0f, 0.0f);
        extraBot.Health = 100.0f;
        extraBot.MaxHealth = 100.0f;
        extraBot.bIsEnemy = true;
        extraBot.bIsDormant = false;
        extraBot.HeadBone = FVector(extraBot.RootLocation.X, extraBot.RootLocation.Y + 1.65f, extraBot.RootLocation.Z);
        extraBot.ChestBone = FVector(extraBot.RootLocation.X, extraBot.RootLocation.Y + 1.15f, extraBot.RootLocation.Z);
        extraBot.PelvisBone = FVector(extraBot.RootLocation.X, extraBot.RootLocation.Y + 0.65f, extraBot.RootLocation.Z);
        s_DummyBots.push_back(extraBot);
        s_SimulatedLevel.Actors.push_back(&s_DummyBots.back());
        AddConsoleLog("[LogWorld] Spawned actor: " + name);
    } else if (action == "modules") {
        AddConsoleLog("--- Loaded / Injected Modules ---");
        for (const auto& m : g_InjectedModules) {
            char buf[128];
            snprintf(buf, sizeof(buf), "  %s at 0x%p", m.name.c_str(), (void*)m.baseAddress);
            AddConsoleLog(buf);
        }
        if (g_InjectedModules.empty()) {
            AddConsoleLog("  No external injected modules detected.");
        }
    } else if (action == "inject") {
        std::string targetDll;
        ss >> targetDll;
        if (!targetDll.empty()) {
            HMODULE h = LoadLibraryA(targetDll.c_str());
            if (h) {
                AddConsoleLog("[LogModule] Successfully loaded: " + targetDll);
            } else {
                AddConsoleLog("[LogModule] Failed to load: " + targetDll);
            }
        } else {
            AddConsoleLog("[LogModule] Usage: inject <filename.dll>");
        }
    } else if (action == "cls" || action == "clear") {
        g_ConsoleLogs.clear();
    } else if (action == "quit" || action == "exit") {
        PostQuitMessage(0);
    } else {
        AddConsoleLog("[LogConsole] Unknown command: '" + rawCmd + "'. Type 'help' for command list.");
    }
}

static void RenderCube(const XMMATRIX& world, const XMMATRIX& viewProj, const XMFLOAT4& color, float spec = 0.4f) {
    SceneConstantBuffer cb;
    cb.mWorldViewProj = XMMatrixTranspose(world * viewProj);
    cb.mWorld = XMMatrixTranspose(world);
    cb.vColor = color;
    cb.vLightDir = XMFLOAT4(0.4f, -0.8f, 0.45f, 0.0f);
    cb.vEyePos = XMFLOAT4(g_CamPos.x, g_CamPos.y, g_CamPos.z, 1.0f);
    cb.vParams = XMFLOAT4(0.35f, spec, (g_ViewMode == ViewMode::Unlit) ? 1.0f : 0.0f, g_Gamma);
    g_pContext->UpdateSubresource(g_pSceneConstantBuffer, 0, nullptr, &cb, 0, 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    g_pContext->IASetVertexBuffers(0, 1, &g_pCubeVertexBuffer, &stride, &offset);
    g_pContext->IASetIndexBuffer(g_pCubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    g_pContext->DrawIndexed(36, 0, 0);
}

// 3D-to-2D World to Screen projection
static bool WorldToScreen(const FVector& world, const XMMATRIX& viewProj, float screenW, float screenH, XMFLOAT2& outScreen, float& outDist) {
    XMVECTOR wp = XMVectorSet(world.X, world.Y, world.Z, 1.0f);
    XMVECTOR clip = XMVector4Transform(wp, viewProj);
    float w = XMVectorGetW(clip);
    if (w <= 0.1f) return false;

    float x = XMVectorGetX(clip) / w;
    float y = XMVectorGetY(clip) / w;

    outScreen.x = (x + 1.0f) * 0.5f * screenW;
    outScreen.y = (1.0f - y) * 0.5f * screenH;

    float dx = world.X - g_CamPos.x;
    float dy = world.Y - g_CamPos.y;
    float dz = world.Z - g_CamPos.z;
    outDist = std::sqrt(dx * dx + dy * dy + dz * dz);
    return true;
}

// ==============================================================================
// 3. ImGui User Interface (UE5 Developer Console, Pause Menu, In-Game HUD)
// ==============================================================================
static void RenderTacticalHUD(float screenW, float screenH) {
    if (!g_ShowTacticalHUD) return;
    ImDrawList* draw = ImGui::GetForegroundDrawList();

    // 1. Top Horizon Tactical Compass Tape
    float compassW = 420.0f;
    float compassH = 34.0f;
    float compassX = (screenW - compassW) * 0.5f;
    float compassY = 16.0f;

    draw->AddRectFilled(ImVec2(compassX, compassY), ImVec2(compassX + compassW, compassY + compassH),
                        IM_COL32(12, 16, 24, 185), 6.0f);
    draw->AddRect(ImVec2(compassX, compassY), ImVec2(compassX + compassW, compassY + compassH),
                  IM_COL32(50, 70, 95, 120), 6.0f, 0, 1.0f);

    float currentDeg = fmodf(XMConvertToDegrees(g_CamYaw) + 3600.0f, 360.0f);
    float centerTapeX = compassX + compassW * 0.5f;

    draw->PushClipRect(ImVec2(compassX + 4.0f, compassY), ImVec2(compassX + compassW - 4.0f, compassY + compassH), true);
    for (int deg = -180; deg <= 540; deg += 15) {
        float diff = (float)deg - currentDeg;
        while (diff < -180.0f) diff += 360.0f;
        while (diff > 180.0f) diff -= 360.0f;

        float x = centerTapeX + diff * 3.5f;
        if (x < compassX || x > compassX + compassW) continue;

        int normDeg = (deg % 360 + 360) % 360;
        const char* card = nullptr;
        if (normDeg == 0) card = "N";
        else if (normDeg == 45) card = "NE";
        else if (normDeg == 90) card = "E";
        else if (normDeg == 135) card = "SE";
        else if (normDeg == 180) card = "S";
        else if (normDeg == 225) card = "SW";
        else if (normDeg == 270) card = "W";
        else if (normDeg == 315) card = "NW";

        if (card) {
            ImVec2 ts = ImGui::CalcTextSize(card);
            draw->AddText(ImVec2(x - ts.x * 0.5f, compassY + 4.0f),
                          (normDeg == 0) ? IM_COL32(245, 80, 80, 255) : IM_COL32(230, 240, 255, 240), card);
            draw->AddLine(ImVec2(x, compassY + 22.0f), ImVec2(x, compassY + 28.0f), IM_COL32(200, 210, 230, 200), 1.5f);
        } else {
            draw->AddLine(ImVec2(x, compassY + 20.0f), ImVec2(x, compassY + 26.0f), IM_COL32(140, 160, 185, 140), 1.0f);
        }
    }
    draw->PopClipRect();

    // Compass Center Notch & Digital Bearing
    draw->AddTriangleFilled(ImVec2(centerTapeX - 4.0f, compassY + compassH),
                            ImVec2(centerTapeX + 4.0f, compassY + compassH),
                            ImVec2(centerTapeX, compassY + compassH - 6.0f),
                            IM_COL32(0, 210, 255, 255));
    char degBuf[32];
    snprintf(degBuf, sizeof(degBuf), "%.0f°", currentDeg);
    ImVec2 dts = ImGui::CalcTextSize(degBuf);
    draw->AddText(ImVec2(compassX + compassW + 12.0f, compassY + (compassH - dts.y) * 0.5f),
                  IM_COL32(0, 210, 255, 230), degBuf);

    // 2. High-Precision Center Tactical Crosshair
    float cx = screenW * 0.5f;
    float cy = screenH * 0.5f;
    float gap = 5.0f;
    float length = 11.0f;

    // Crosshair ticks
    draw->AddLine(ImVec2(cx - gap - length, cy), ImVec2(cx - gap, cy), IM_COL32(0, 240, 255, 240), 1.5f);
    draw->AddLine(ImVec2(cx + gap, cy), ImVec2(cx + gap + length, cy), IM_COL32(0, 240, 255, 240), 1.5f);
    draw->AddLine(ImVec2(cx, cy - gap - length), ImVec2(cx, cy - gap), IM_COL32(0, 240, 255, 240), 1.5f);
    draw->AddLine(ImVec2(cx, cy + gap), ImVec2(cx, cy + gap + length), IM_COL32(0, 240, 255, 240), 1.5f);
    draw->AddCircleFilled(ImVec2(cx, cy), 1.5f, IM_COL32(255, 255, 255, 255), 6);

    // 3. Bottom-Left Operator Vitality & Armor HUD
    float vitalX = 24.0f;
    float vitalY = screenH - 85.0f;
    float vitalW = 210.0f;

    draw->AddRectFilled(ImVec2(vitalX, vitalY), ImVec2(vitalX + vitalW, vitalY + 62.0f),
                        IM_COL32(10, 14, 20, 200), 6.0f);
    draw->AddRect(ImVec2(vitalX, vitalY), ImVec2(vitalX + vitalW, vitalY + 62.0f),
                  IM_COL32(40, 55, 75, 140), 6.0f);

    draw->AddText(ImVec2(vitalX + 12.0f, vitalY + 8.0f), IM_COL32(180, 200, 220, 220), "HP: 100 / 100");
    draw->AddRectFilled(ImVec2(vitalX + 12.0f, vitalY + 26.0f), ImVec2(vitalX + vitalW - 12.0f, vitalY + 34.0f),
                        IM_COL32(25, 40, 30, 255), 3.0f);
    draw->AddRectFilled(ImVec2(vitalX + 12.0f, vitalY + 26.0f), ImVec2(vitalX + vitalW - 12.0f, vitalY + 34.0f),
                        IM_COL32(45, 215, 95, 255), 3.0f);

    draw->AddText(ImVec2(vitalX + 12.0f, vitalY + 40.0f), IM_COL32(0, 210, 255, 220), "ARMOR: 100");
    draw->AddText(ImVec2(vitalX + 115.0f, vitalY + 40.0f), IM_COL32(255, 185, 40, 220), "STAMINA: 100%");

    // 4. Bottom-Right Weapon Ammo HUD
    float ammoX = screenW - 235.0f;
    float ammoY = screenH - 85.0f;
    float ammoW = 210.0f;

    draw->AddRectFilled(ImVec2(ammoX, ammoY), ImVec2(ammoX + ammoW, ammoY + 62.0f),
                        IM_COL32(10, 14, 20, 200), 6.0f);
    draw->AddRect(ImVec2(ammoX, ammoY), ImVec2(ammoX + ammoW, ammoY + 62.0f),
                  IM_COL32(40, 55, 75, 140), 6.0f);

    draw->AddText(ImVec2(ammoX + 12.0f, ammoY + 8.0f), IM_COL32(245, 195, 65, 255), "VANDAL PRIME");
    draw->AddText(ImVec2(ammoX + 120.0f, ammoY + 8.0f), IM_COL32(160, 180, 200, 200), "[AUTO]");
    draw->AddText(ImVec2(ammoX + 12.0f, ammoY + 28.0f), IM_COL32(255, 255, 255, 255), "25 / 75");
    draw->AddText(ImVec2(ammoX + 115.0f, ammoY + 36.0f), IM_COL32(0, 210, 255, 200), "7.62x39mm");

    // 5. Stat FPS & Unit Engine Telemetry
    if (g_ShowFpsStat) {
        char statBuf[64];
        snprintf(statBuf, sizeof(statBuf), "%.0f FPS (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / (std::max)(1.0f, ImGui::GetIO().Framerate));
        draw->AddText(ImVec2(24.0f, 20.0f), IM_COL32(45, 225, 105, 240), statBuf);
    }
    if (g_ShowUnitStat) {
        draw->AddText(ImVec2(24.0f, 38.0f), IM_COL32(0, 210, 255, 220), "Frame: 1.8ms | Game: 0.9ms | Render: 0.9ms | D3D11 RHI");
    }

    // Injected Notification Toast
    if (g_NotificationTimer > 0.0f && g_LastInjectedNotification[0] != '\0') {
        float toastW = 480.0f;
        float toastH = 38.0f;
        float tx = (screenW - toastW) * 0.5f;
        float ty = 60.0f;
        draw->AddRectFilled(ImVec2(tx, ty), ImVec2(tx + toastW, ty + toastH), IM_COL32(25, 15, 30, 240), 6.0f);
        draw->AddRect(ImVec2(tx, ty), ImVec2(tx + toastW, ty + toastH), IM_COL32(220, 60, 80, 255), 6.0f, 0, 1.5f);
        draw->AddText(ImVec2(tx + 16.0f, ty + 10.0f), IM_COL32(255, 90, 110, 255), g_LastInjectedNotification);
    }
}

// 3D Target Nameplates & Hitboxes HUD
static void RenderTargetNameplates(const XMMATRIX& viewProj, float screenW, float screenH) {
    if (!g_ShowTacticalHUD) return;
    ImDrawList* draw = ImGui::GetForegroundDrawList();

    for (const auto& bot : s_DummyBots) {
        XMFLOAT2 sHead, sPelvis;
        float distHead, distPelvis;
        if (WorldToScreen(bot.HeadBone, viewProj, screenW, screenH, sHead, distHead) &&
            WorldToScreen(bot.PelvisBone, viewProj, screenW, screenH, sPelvis, distPelvis)) {

            if (distHead > 75.0f) continue;

            ImU32 teamColor = bot.bIsEnemy ? IM_COL32(245, 60, 60, 230) : IM_COL32(0, 210, 255, 230);

            // Head diamond marker
            draw->AddQuadFilled(
                ImVec2(sHead.x, sHead.y - 4.0f),
                ImVec2(sHead.x + 4.0f, sHead.y),
                ImVec2(sHead.x, sHead.y + 4.0f),
                ImVec2(sHead.x - 4.0f, sHead.y),
                teamColor
            );

            // Nameplate Tag
            char tag[64];
            snprintf(tag, sizeof(tag), "%s [%.0fm]", bot.CharacterName, distHead);
            ImVec2 ts = ImGui::CalcTextSize(tag);
            float tagX = sHead.x - ts.x * 0.5f;
            float tagY = sHead.y - 22.0f;

            draw->AddRectFilled(ImVec2(tagX - 4.0f, tagY - 2.0f), ImVec2(tagX + ts.x + 4.0f, tagY + ts.y + 2.0f),
                                IM_COL32(8, 12, 16, 200), 3.0f);
            draw->AddText(ImVec2(tagX, tagY), teamColor, tag);

            // Small Health Bar
            float barW = 34.0f;
            float barH = 3.0f;
            float barX = sHead.x - barW * 0.5f;
            float barY = tagY - 6.0f;
            draw->AddRectFilled(ImVec2(barX, barY), ImVec2(barX + barW, barY + barH), IM_COL32(30, 30, 30, 220), 1.0f);
            draw->AddRectFilled(ImVec2(barX, barY), ImVec2(barX + barW * (bot.Health / bot.MaxHealth), barY + barH),
                                IM_COL32(45, 215, 95, 240), 1.0f);
        }
    }
}

// Unreal Engine Developer Console (Triggered by Delete / Suppr or ~ / Tilde)
static void RenderDevConsole(float screenW, float screenH) {
    if (!g_ShowDevConsole) return;

    ImGui::SetNextWindowPos(ImVec2(0.0f, screenH - 260.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(screenW, 260.0f), ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.07f, 0.10f, 0.94f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.20f, 0.30f, 0.45f, 0.70f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 8.0f));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("##UnrealEngineConsole", &g_ShowDevConsole, flags)) {
        ImGui::TextColored(ImVec4(0.95f, 0.75f, 0.20f, 1.0f), "UNREAL ENGINE 5.4.2 DEVELOPER CONSOLE  [PRESS DELETE / SUPPR TO CLOSE]");
        ImGui::Separator();

        // Output Log Area
        float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("##ConsoleLogScroll", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
        for (const auto& log : g_ConsoleLogs) {
            ImVec4 col = ImVec4(0.85f, 0.90f, 0.95f, 1.0f);
            if (log.rfind("[INJECTION", 0) == 0 || log.rfind("[LogModule] Failed", 0) == 0) {
                col = ImVec4(1.0f, 0.35f, 0.35f, 1.0f);
            } else if (log.rfind(">", 0) == 0) {
                col = ImVec4(0.0f, 0.85f, 1.0f, 1.0f);
            } else if (log.rfind("[LogWorld]", 0) == 0) {
                col = ImVec4(0.40f, 0.85f, 0.50f, 1.0f);
            }
            ImGui::TextColored(col, "%s", log.c_str());
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

        // Input Line
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.0f, 0.85f, 1.0f, 1.0f), "CMD >");
        ImGui::SameLine();
        ImGui::PushItemWidth(screenW - 80.0f);

        ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;
        auto callback = [](ImGuiInputTextCallbackData* data) -> int {
            if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
                if (g_ConsoleHistory.empty()) return 0;
                if (data->EventKey == ImGuiKey_UpArrow) {
                    if (g_ConsoleHistoryIdx == -1) g_ConsoleHistoryIdx = (int)g_ConsoleHistory.size() - 1;
                    else if (g_ConsoleHistoryIdx > 0) g_ConsoleHistoryIdx--;
                } else if (data->EventKey == ImGuiKey_DownArrow) {
                    if (g_ConsoleHistoryIdx != -1) {
                        g_ConsoleHistoryIdx++;
                        if (g_ConsoleHistoryIdx >= (int)g_ConsoleHistory.size()) g_ConsoleHistoryIdx = -1;
                    }
                }
                data->DeleteChars(0, data->BufTextLen);
                if (g_ConsoleHistoryIdx != -1) {
                    data->InsertChars(0, g_ConsoleHistory[g_ConsoleHistoryIdx].c_str());
                }
            }
            return 0;
        };

        if (ImGui::InputText("##ConsoleInput", g_ConsoleInput, sizeof(g_ConsoleInput), inputFlags, callback)) {
            ExecuteConsoleCommand(g_ConsoleInput);
            g_ConsoleInput[0] = '\0';
            ImGui::SetKeyboardFocusHere(-1);
        }
        ImGui::PopItemWidth();

        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere(-1);
        }
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

// Unreal Engine Pause / Escape Menu (Triggered by Escape)
static void RenderPauseMenu(float screenW, float screenH) {
    if (!g_ShowPauseMenu) return;

    // Dark Frosted Backdrop
    ImDrawList* bg = ImGui::GetForegroundDrawList();
    bg->AddRectFilled(ImVec2(0, 0), ImVec2(screenW, screenH), IM_COL32(5, 7, 10, 180));

    float menuW = 540.0f;
    float menuH = 460.0f;
    ImGui::SetNextWindowPos(ImVec2((screenW - menuW) * 0.5f, (screenH - menuH) * 0.5f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(menuW, menuH), ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.10f, 0.14f, 0.98f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.75f, 1.0f, 0.65f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 18.0f));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("##UnrealPauseMenu", &g_ShowPauseMenu, flags)) {
        ImGui::TextColored(ImVec4(0.0f, 0.85f, 1.0f, 1.0f), "UNREAL ENGINE 5.4.2 // SIMULATION PAUSED");
        ImGui::TextColored(ImVec4(0.55f, 0.65f, 0.75f, 1.0f), "DirectX 11 Internal Hookable Testbed Sandbox");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::BeginTabBar("##PauseMenuTabs")) {
            if (ImGui::BeginTabItem("Session & Controls")) {
                ImGui::Spacing();
                if (ImGui::Button("Resume Game (ESC)", ImVec2(-1, 36))) {
                    g_ShowPauseMenu = false;
                    g_MouseCaptured = true;
                    ShowCursor(FALSE);
                }
                ImGui::Spacing();

                if (ImGui::Button("Spawn Additional Target Dummy (F2)", ImVec2(-1, 32))) {
                    ExecuteConsoleCommand("spawn");
                }
                ImGui::Spacing();

                if (ImGui::Button("Teleport Camera to Spawn Origin", ImVec2(-1, 32))) {
                    ExecuteConsoleCommand("teleport");
                }
                ImGui::Spacing();

                if (ImGui::Button("Open Developer Console (Delete / ~)", ImVec2(-1, 32))) {
                    g_ShowDevConsole = true;
                    g_ShowPauseMenu = false;
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                if (ImGui::Button("Quit Simulation to Desktop", ImVec2(-1, 34))) {
                    PostQuitMessage(0);
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Graphics & Viewport")) {
                ImGui::Spacing();
                ImGui::SliderFloat("Field of View (deg)", &g_FieldOfView, 50.0f, 120.0f, "%.0f");
                s_SimulatedCameraManager.FieldOfView = g_FieldOfView;

                ImGui::SliderFloat("Mouse Sensitivity", &g_MouseSensitivity, 0.0005f, 0.0080f, "%.4f");

                const char* viewModes[] = { "Lit (Standard Shaded)", "Wireframe Mesh", "Unlit (Flat Color)" };
                int curVm = static_cast<int>(g_ViewMode);
                if (ImGui::Combo("Render Mode", &curVm, viewModes, 3)) {
                    g_ViewMode = static_cast<ViewMode>(curVm);
                }

                ImGui::Checkbox("Render Atmospheric Sky & Sun", &g_ShowSkyAtmosphere);
                ImGui::Checkbox("Display Tactical Combat HUD", &g_ShowTacticalHUD);
                ImGui::Checkbox("Display FPS Counter", &g_ShowFpsStat);
                ImGui::Checkbox("Display Unit Telemetry", &g_ShowUnitStat);

                ImGui::SliderFloat("Scene Gamma", &g_Gamma, 0.5f, 2.0f, "%.2f");
                ImGui::SliderFloat("Simulation Slomo", &g_TimeDilation, 0.1f, 2.0f, "%.2fx");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Injected Modules")) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.95f, 0.75f, 0.20f, 1.0f), "Loaded Non-Baseline Modules (%zu detected):", g_InjectedModules.size());
                ImGui::Spacing();

                ImGui::BeginChild("##ModulesList", ImVec2(0, 180), true);
                if (g_InjectedModules.empty()) {
                    ImGui::TextColored(ImVec4(0.5f, 0.6f, 0.7f, 1.0f), "No external DLLs detected yet.");
                    ImGui::TextColored(ImVec4(0.5f, 0.6f, 0.7f, 1.0f), "Inject any cheat or test DLL with your injector.");
                } else {
                    for (const auto& mod : g_InjectedModules) {
                        ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.4f, 1.0f), "%s", mod.name.c_str());
                        ImGui::SameLine(220.0f);
                        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Base: 0x%p", (void*)mod.baseAddress);
                    }
                }
                ImGui::EndChild();

                ImGui::Spacing();
                if (ImGui::Button("Quick Inject: solar.dll / solar_lib.dll (F5)", ImVec2(-1, 32))) {
                    ExecuteConsoleCommand("inject solar.dll");
                }
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

// ==============================================================================
// 4. 3D Scene Rendering (Atmospheric Sky, Decors, Props, Actors)
// ==============================================================================
static void RenderScene3D(float dt, float width, float height) {
    D3D11_VIEWPORT vp = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
    g_pContext->RSSetViewports(1, &vp);

    // Deep Dark Slate Clear Color
    float clearColor[4] = { 0.06f, 0.08f, 0.12f, 1.0f };
    g_pContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);
    g_pContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    g_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    // Camera Matrices
    XMVECTOR eye = XMVectorSet(g_CamPos.x, g_CamPos.y, g_CamPos.z, 0.0f);
    XMVECTOR forward = XMVectorSet(
        std::sin(g_CamYaw) * std::cos(g_CamPitch),
        std::sin(g_CamPitch),
        std::cos(g_CamYaw) * std::cos(g_CamPitch),
        0.0f
    );
    XMVECTOR target = XMVectorAdd(eye, forward);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(eye, target, up);
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(g_FieldOfView), width / height, 0.1f, 500.0f);
    XMMATRIX viewProj = view * proj;

    // 1. Atmospheric Sky Dome Pass
    if (g_ShowSkyAtmosphere && g_pSkyVS && g_pSkyPS) {
        g_pContext->OMSetDepthStencilState(g_pDepthStateSky, 0);
        g_pContext->RSSetState(g_pRasterizerSolid);

        XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProj);
        SkyConstantBuffer skyCb;
        skyCb.mInvViewProj = XMMatrixTranspose(invViewProj);
        skyCb.vEyePos = XMFLOAT4(g_CamPos.x, g_CamPos.y, g_CamPos.z, 1.0f);
        skyCb.vSunDir = XMFLOAT4(0.4f, -0.8f, 0.45f, 0.0f);
        skyCb.vSkyZenith = XMFLOAT4(0.04f, 0.09f, 0.22f, 1.0f);
        skyCb.vSkyHorizon = XMFLOAT4(0.20f, 0.28f, 0.40f, 1.0f);
        g_pContext->UpdateSubresource(g_pSkyConstantBuffer, 0, nullptr, &skyCb, 0, 0);

        g_pContext->IASetInputLayout(nullptr);
        g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g_pContext->VSSetShader(g_pSkyVS, nullptr, 0);
        g_pContext->PSSetShader(g_pSkyPS, nullptr, 0);
        g_pContext->PSSetConstantBuffers(0, 1, &g_pSkyConstantBuffer);
        g_pContext->Draw(3, 0);
    }

    // 2. Set Up Default Pipeline for Scene Objects
    g_pContext->OMSetDepthStencilState(g_pDepthStateDefault, 1);
    g_pContext->RSSetState((g_ViewMode == ViewMode::Wireframe) ? g_pRasterizerWireframe : g_pRasterizerSolid);
    g_pContext->IASetInputLayout(g_pInputLayout);
    g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pContext->VSSetShader(g_pSceneVS, nullptr, 0);
    g_pContext->VSSetConstantBuffers(0, 1, &g_pSceneConstantBuffer);
    g_pContext->PSSetShader(g_pScenePS, nullptr, 0);
    g_pContext->PSSetConstantBuffers(0, 1, &g_pSceneConstantBuffer);

    // Update Simulated Unreal Engine Camera Position
    s_SimulatedCameraManager.CameraLocation = FVector(g_CamPos.x, g_CamPos.y, g_CamPos.z);
    s_SimulatedCameraManager.CameraRotation = FRotator(XMConvertToDegrees(g_CamPitch), XMConvertToDegrees(g_CamYaw), 0.0f);
    s_SimulatedWorld.TimeSeconds += dt * g_TimeDilation;

    // 3. Draw Checkered Arena Floor Tiles
    for (int x = -12; x <= 12; ++x) {
        for (int z = -12; z <= 18; ++z) {
            XMMATRIX world = XMMatrixScaling(1.96f, 0.1f, 1.96f) * XMMatrixTranslation((float)x * 2.0f, 0.0f, (float)z * 2.0f);
            bool alt = ((x + z) % 2 == 0);
            XMFLOAT4 floorCol = alt ? XMFLOAT4(0.12f, 0.14f, 0.18f, 1.0f) : XMFLOAT4(0.09f, 0.10f, 0.13f, 1.0f);
            RenderCube(world, viewProj, floorCol, 0.15f);
        }
    }

    // 4. Draw Arena Perimeter Boundary Walls
    for (int x = -13; x <= 13; ++x) {
        XMMATRIX wallS = XMMatrixScaling(1.95f, 1.5f, 0.5f) * XMMatrixTranslation((float)x * 2.0f, 0.75f, -25.0f);
        XMMATRIX wallN = XMMatrixScaling(1.95f, 1.5f, 0.5f) * XMMatrixTranslation((float)x * 2.0f, 0.75f, 37.0f);
        RenderCube(wallS, viewProj, XMFLOAT4(0.15f, 0.18f, 0.24f, 1.0f));
        RenderCube(wallN, viewProj, XMFLOAT4(0.15f, 0.18f, 0.24f, 1.0f));
    }
    for (int z = -12; z <= 18; ++z) {
        XMMATRIX wallW = XMMatrixScaling(0.5f, 1.5f, 1.95f) * XMMatrixTranslation(-25.0f, 0.75f, (float)z * 2.0f);
        XMMATRIX wallE = XMMatrixScaling(0.5f, 1.5f, 1.95f) * XMMatrixTranslation(25.0f, 0.75f, (float)z * 2.0f);
        RenderCube(wallW, viewProj, XMFLOAT4(0.15f, 0.18f, 0.24f, 1.0f));
        RenderCube(wallE, viewProj, XMFLOAT4(0.15f, 0.18f, 0.24f, 1.0f));
    }

    // 5. Draw Tactical Obstacle Props & Cover Crates
    struct PropCrate { float x, y, z, sx, sy, sz; XMFLOAT4 col; };
    PropCrate crates[] = {
        { -6.0f, 0.75f, 4.0f, 1.5f, 1.5f, 1.5f, XMFLOAT4(0.24f, 0.28f, 0.38f, 1.0f) },
        { -5.0f, 0.50f, 4.2f, 1.0f, 1.0f, 1.0f, XMFLOAT4(0.30f, 0.35f, 0.45f, 1.0f) },
        {  6.0f, 1.00f, 5.0f, 2.0f, 2.0f, 1.5f, XMFLOAT4(0.24f, 0.28f, 0.38f, 1.0f) },
        { -2.0f, 0.60f, 16.0f, 3.0f, 1.2f, 0.8f, XMFLOAT4(0.20f, 0.24f, 0.32f, 1.0f) },
        {  2.5f, 0.60f, 16.0f, 3.0f, 1.2f, 0.8f, XMFLOAT4(0.20f, 0.24f, 0.32f, 1.0f) },
    };
    for (const auto& c : crates) {
        XMMATRIX world = XMMatrixScaling(c.sx, c.sy, c.sz) * XMMatrixTranslation(c.x, c.y, c.z);
        RenderCube(world, viewProj, c.col, 0.3f);
    }

    // 6. Central Elevated Platform
    XMMATRIX platWorld = XMMatrixScaling(6.0f, 0.6f, 6.0f) * XMMatrixTranslation(0.0f, 0.3f, 10.0f);
    RenderCube(platWorld, viewProj, XMFLOAT4(0.18f, 0.22f, 0.30f, 1.0f));

    // Floating Rotating Holographic Weapon Pickup on Pedestal
    float spin = s_SimulatedWorld.TimeSeconds * 1.8f;
    float bob = std::sin(s_SimulatedWorld.TimeSeconds * 3.0f) * 0.15f;
    XMMATRIX pedWorld = XMMatrixScaling(0.8f, 1.0f, 0.8f) * XMMatrixTranslation(0.0f, 0.8f, 10.0f);
    RenderCube(pedWorld, viewProj, XMFLOAT4(0.12f, 0.15f, 0.20f, 1.0f));

    XMMATRIX weaponWorld = XMMatrixScaling(0.45f, 0.45f, 0.45f) * XMMatrixRotationY(spin) * XMMatrixTranslation(0.0f, 1.75f + bob, 10.0f);
    RenderCube(weaponWorld, viewProj, XMFLOAT4(0.0f, 0.85f, 1.0f, 1.0f), 0.8f);

    // 7. Draw 3D Target Mannequins (Bots)
    for (size_t i = 0; i < s_DummyBots.size(); ++i) {
        auto& bot = s_DummyBots[i];

        float osc = std::sin(s_SimulatedWorld.TimeSeconds * 1.5f + (float)i) * 0.4f;
        float bx = bot.RootLocation.X + osc;
        float by = bot.RootLocation.Y;
        float bz = bot.RootLocation.Z;

        bot.HeadBone = FVector(bx, by + 1.6f, bz);
        bot.ChestBone = FVector(bx, by + 1.05f, bz);
        bot.PelvisBone = FVector(bx, by + 0.55f, bz);

        XMFLOAT4 teamColor = bot.bIsEnemy ? XMFLOAT4(0.95f, 0.25f, 0.25f, 1.0f) : XMFLOAT4(0.22f, 0.75f, 0.95f, 1.0f);

        // Head (Cube with Visor)
        XMMATRIX headWorld = XMMatrixScaling(0.35f, 0.35f, 0.35f) * XMMatrixTranslation(bx, by + 1.6f, bz);
        RenderCube(headWorld, viewProj, XMFLOAT4(teamColor.x * 1.15f, teamColor.y * 1.15f, teamColor.z * 1.15f, 1.0f), 0.6f);

        // Head Visor Plate
        XMMATRIX visorWorld = XMMatrixScaling(0.28f, 0.12f, 0.05f) * XMMatrixTranslation(bx, by + 1.62f, bz - 0.18f);
        RenderCube(visorWorld, viewProj, XMFLOAT4(0.0f, 0.95f, 1.0f, 1.0f), 0.9f);

        // Torso / Chest Armor
        XMMATRIX torsoWorld = XMMatrixScaling(0.60f, 0.85f, 0.35f) * XMMatrixTranslation(bx, by + 1.05f, bz);
        RenderCube(torsoWorld, viewProj, teamColor, 0.35f);

        // Tactical Arms
        XMMATRIX armL = XMMatrixScaling(0.18f, 0.75f, 0.18f) * XMMatrixTranslation(bx - 0.42f, by + 1.0f, bz);
        XMMATRIX armR = XMMatrixScaling(0.18f, 0.75f, 0.18f) * XMMatrixTranslation(bx + 0.42f, by + 1.0f, bz);
        RenderCube(armL, viewProj, XMFLOAT4(0.20f, 0.22f, 0.28f, 1.0f));
        RenderCube(armR, viewProj, XMFLOAT4(0.20f, 0.22f, 0.28f, 1.0f));

        // Pelvis / Legs
        XMMATRIX legL = XMMatrixScaling(0.20f, 0.60f, 0.20f) * XMMatrixTranslation(bx - 0.18f, by + 0.30f, bz);
        XMMATRIX legR = XMMatrixScaling(0.20f, 0.60f, 0.20f) * XMMatrixTranslation(bx + 0.18f, by + 0.30f, bz);
        RenderCube(legL, viewProj, XMFLOAT4(0.16f, 0.18f, 0.22f, 1.0f));
        RenderCube(legR, viewProj, XMFLOAT4(0.16f, 0.18f, 0.22f, 1.0f));

        // Contact Ground Shadow Disc (Flat dark quad on floor)
        XMMATRIX shadowWorld = XMMatrixScaling(0.75f, 0.02f, 0.75f) * XMMatrixTranslation(bx, 0.06f, bz);
        RenderCube(shadowWorld, viewProj, XMFLOAT4(0.02f, 0.03f, 0.04f, 1.0f));
    }

    // 8. Render ImGui In-Game HUD, Target Nameplates, Dev Console & Pause Menu
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    Solar::NewFrame();

    RenderTargetNameplates(viewProj, width, height);
    RenderTacticalHUD(width, height);
    RenderDevConsole(width, height);
    RenderPauseMenu(width, height);

    ImGui::Render();
    g_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // 9. SwapChain Present
    UINT syncInterval = (g_FpsCapMode == 0) ? 1 : 0;
    g_pSwapChain->Present(syncInterval, 0);
}

// ==============================================================================
// 5. Window Procedure & Input Handling
// ==============================================================================
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_KEYDOWN:
        if (wParam == VK_DELETE || wParam == VK_OEM_3 /* ~ */) {
            g_ShowDevConsole = !g_ShowDevConsole;
            if (g_ShowDevConsole) g_ShowPauseMenu = false;
            g_MouseCaptured = (!g_ShowDevConsole && !g_ShowPauseMenu);
            ShowCursor(!g_MouseCaptured);
        } else if (wParam == VK_ESCAPE) {
            if (g_ShowDevConsole) {
                g_ShowDevConsole = false;
            } else {
                g_ShowPauseMenu = !g_ShowPauseMenu;
            }
            g_MouseCaptured = (!g_ShowDevConsole && !g_ShowPauseMenu);
            ShowCursor(!g_MouseCaptured);
        } else if (wParam == VK_TAB) {
            g_MouseCaptured = !g_MouseCaptured;
            ShowCursor(!g_MouseCaptured);
        } else if (wParam == VK_F5) {
            ExecuteConsoleCommand("inject solar.dll");
        } else if (wParam == VK_F2) {
            ExecuteConsoleCommand("spawn");
        }
        break;

    case WM_SIZE:
        if (g_pSwapChain && wParam != SIZE_MINIMIZED) {
            if (g_pRenderTargetView) { g_pRenderTargetView->Release(); g_pRenderTargetView = nullptr; }
            if (g_pDepthStencilView) { g_pDepthStencilView->Release(); g_pDepthStencilView = nullptr; }
            if (g_pDepthStencilBuffer) { g_pDepthStencilBuffer->Release(); g_pDepthStencilBuffer = nullptr; }

            UINT w = LOWORD(lParam);
            UINT h = HIWORD(lParam);
            g_pSwapChain->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);

            ID3D11Texture2D* pBackBuffer = nullptr;
            g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
            g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
            pBackBuffer->Release();

            D3D11_TEXTURE2D_DESC descDepth = {};
            descDepth.Width = w;
            descDepth.Height = h;
            descDepth.MipLevels = 1;
            descDepth.ArraySize = 1;
            descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            descDepth.SampleDesc.Count = 1;
            descDepth.Usage = D3D11_USAGE_DEFAULT;
            descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            g_pDevice->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencilBuffer);
            g_pDevice->CreateDepthStencilView(g_pDepthStencilBuffer, nullptr, &g_pDepthStencilView);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    printf("====================================================================\n");
    printf("     SOLAR WORLD - UNREAL ENGINE 5 INTERNALS TESTBED SANDBOX\n");
    printf("====================================================================\n");
    printf("[*] Process ID: %lu (0x%X)\n", GetCurrentProcessId(), GetCurrentProcessId());
    printf("[*] Direct3D 11 Hardware SwapChain Initializing...\n");

    InitializeSimulatedWorld();
    SnapshotBaselineModules();

    WNDCLASSEX wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, "UnrealWindow", nullptr };
    RegisterClassEx(&wc);

    g_hWnd = CreateWindow(
        wc.lpszClassName,
        "SolarWorld | Unreal Engine 5.4.2 Simulated Testbed [D3D11 / Internal Hookable]",
        WS_OVERLAPPEDWINDOW,
        120, 80, 1280, 720,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    if (!InitD3D(g_hWnd)) {
        printf("[!] Failed to initialize Direct3D 11 device and swapchain.\n");
        return 1;
    }

    // Initialize Dear ImGui & Solar Framework Backend
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    Solar::Render::FontManager::Get().LoadFonts(1.0f);
    Solar::Initialize();
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(g_pDevice, g_pContext);

    ShowWindow(g_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(g_hWnd);
    ShowCursor(FALSE);

    printf("[*] GWorld Exported Symbol Address: 0x%p\n", (void*)GWorld);
    printf("[*] SwapChain Address: 0x%p (Hook VTable Index 8: Present)\n", (void*)g_pSwapChain);
    printf("[*] Ready for DLL injection! Compatible with Xenos, Cheat Engine, Process Hacker, etc.\n");
    printf("[*] Controls:\n");
    printf("    [WASD] Move Camera       | [Mouse] Look\n");
    printf("    [Delete / ~] UE Console  | [Escape] Pause Menu\n");
    printf("    [Tab] Release Mouse      | [F5] Load DLL | [F2] Spawn Bot\n");
    printf("--------------------------------------------------------------------\n");

    LARGE_INTEGER freq, lastTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastTime);
    timeBeginPeriod(1);

    bool running = true;
    while (running) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) running = false;
        }
        if (!running) break;

        LARGE_INTEGER curTime;
        QueryPerformanceCounter(&curTime);
        float dt = static_cast<float>(curTime.QuadPart - lastTime.QuadPart) / static_cast<float>(freq.QuadPart);
        lastTime = curTime;
        if (dt > 0.1f) dt = 0.1f;

        RECT rc;
        GetClientRect(g_hWnd, &rc);
        float width = static_cast<float>(rc.right - rc.left);
        float height = static_cast<float>(rc.bottom - rc.top);

        // Camera Mouse Look (Only when playing and mouse captured)
        if (g_MouseCaptured && !g_ShowDevConsole && !g_ShowPauseMenu && GetForegroundWindow() == g_hWnd) {
            POINT centerPt;
            centerPt.x = (rc.right - rc.left) / 2;
            centerPt.y = (rc.bottom - rc.top) / 2;
            ClientToScreen(g_hWnd, &centerPt);

            POINT curMouse;
            GetCursorPos(&curMouse);
            float dx = static_cast<float>(curMouse.x - centerPt.x);
            float dy = static_cast<float>(curMouse.y - centerPt.y);

            g_CamYaw += dx * g_MouseSensitivity;
            g_CamPitch -= dy * g_MouseSensitivity;
            g_CamPitch = (std::clamp)(g_CamPitch, -1.45f, 1.45f);

            SetCursorPos(centerPt.x, centerPt.y);
        }

        // Camera WASD Keyboard Movement (Only when not typing in console)
        if (!g_ShowDevConsole && !g_ShowPauseMenu) {
            float moveSpeed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? 16.0f : 8.0f;
            float forwardX = std::sin(g_CamYaw);
            float forwardZ = std::cos(g_CamYaw);
            float rightX = forwardZ;
            float rightZ = -forwardX;

            if (GetAsyncKeyState('W') & 0x8000) { g_CamPos.x += forwardX * moveSpeed * dt; g_CamPos.z += forwardZ * moveSpeed * dt; }
            if (GetAsyncKeyState('S') & 0x8000) { g_CamPos.x -= forwardX * moveSpeed * dt; g_CamPos.z -= forwardZ * moveSpeed * dt; }
            if (GetAsyncKeyState('D') & 0x8000) { g_CamPos.x += rightX * moveSpeed * dt; g_CamPos.z += rightZ * moveSpeed * dt; }
            if (GetAsyncKeyState('A') & 0x8000) { g_CamPos.x -= rightX * moveSpeed * dt; g_CamPos.z -= rightZ * moveSpeed * dt; }
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) g_CamPos.y += moveSpeed * 0.8f * dt;
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) g_CamPos.y = (std::max)(1.0f, g_CamPos.y - moveSpeed * 0.8f * dt);
        }

        CheckForInjectedModules(dt);
        if (width > 0 && height > 0) {
            RenderScene3D(dt, width, height);
        }
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    Solar::Shutdown();
    ImGui::DestroyContext();

    timeEndPeriod(1);
    CleanD3D();
    DestroyWindow(g_hWnd);
    return 0;
}
