#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
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
struct ConstantBuffer {
    XMMATRIX mWorldViewProj;
    XMFLOAT4 vColor;
    XMFLOAT4 vLightDir;
};

struct Vertex {
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
};

static const char* g_ShaderSource = R"(
cbuffer ConstantBuffer : register(b0) {
    matrix WorldViewProj;
    float4 Color;
    float4 LightDir;
};

struct VS_INPUT {
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 Normal : TEXCOORD0;
    float4 Color : COLOR0;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), WorldViewProj);
    output.Normal = input.Normal;
    output.Color = Color;
    return output;
}

float4 PS(PS_INPUT input) : SV_Target {
    float3 light = normalize(-LightDir.xyz);
    float diff = max(dot(normalize(input.Normal), light), 0.25f);
    return float4(input.Color.rgb * diff, input.Color.a);
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
static ID3D11VertexShader*      g_pVertexShader = nullptr;
static ID3D11PixelShader*       g_pPixelShader = nullptr;
static ID3D11InputLayout*       g_pInputLayout = nullptr;
static ID3D11Buffer*            g_pCubeVertexBuffer = nullptr;
static ID3D11Buffer*            g_pCubeIndexBuffer = nullptr;
static ID3D11Buffer*            g_pFloorVertexBuffer = nullptr;
static ID3D11Buffer*            g_pConstantBuffer = nullptr;
static ID3D11RasterizerState*   g_pRasterizerState = nullptr;
static ID3D11DepthStencilState* g_pDepthStencilState = nullptr;

// 3D FPS Camera State
static XMFLOAT3 g_CamPos(0.0f, 2.0f, -8.0f);
static float    g_CamPitch = 0.0f;
static float    g_CamYaw = 0.0f;
static bool     g_MouseCaptured = true;
static POINT    g_LastMousePos = { 0, 0 };

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
        s_DummyBots[i].bIsEnemy = (i != 0); // First bot is friendly, rest are enemies
        s_DummyBots[i].bIsDormant = false;
        s_DummyBots[i].HeadBone = FVector(positions[i].X, positions[i].Y + 1.65f, positions[i].Z);
        s_DummyBots[i].ChestBone = FVector(positions[i].X, positions[i].Y + 1.15f, positions[i].Z);
        s_DummyBots[i].PelvisBone = FVector(positions[i].X, positions[i].Y + 0.65f, positions[i].Z);
        s_SimulatedLevel.Actors.push_back(&s_DummyBots[i]);
    }

    s_SimulatedCameraManager.CameraLocation = FVector(g_CamPos.x, g_CamPos.y, g_CamPos.z);
    s_SimulatedCameraManager.CameraRotation = FRotator(g_CamPitch, g_CamYaw, 0.0f);
    s_SimulatedCameraManager.FieldOfView = 80.0f;

    s_SimulatedPlayerController.PlayerCameraManager = &s_SimulatedCameraManager;
    s_SimulatedPlayerController.AcknowledgedPawn = nullptr;

    s_SimulatedLocalPlayer.PlayerController = &s_SimulatedPlayerController;
    s_SimulatedGameInstance.LocalPlayers.push_back(&s_SimulatedLocalPlayer);

    s_SimulatedWorld.PersistentLevel = &s_SimulatedLevel;
    s_SimulatedWorld.OwningGameInstance = &s_SimulatedGameInstance;
    s_SimulatedWorld.TimeSeconds = 0.0f;

    GWorld = &s_SimulatedWorld;
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

    // Rasterizer State
    D3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.DepthClipEnable = TRUE;
    g_pDevice->CreateRasterizerState(&rastDesc, &g_pRasterizerState);

    // Depth Stencil State
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    g_pDevice->CreateDepthStencilState(&dsDesc, &g_pDepthStencilState);

    // Compile Shaders
    ID3DBlob* pVSBlob = nullptr;
    ID3DBlob* pPSBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    hr = D3DCompile(g_ShaderSource, strlen(g_ShaderSource), "VS", nullptr, nullptr, "VS", "vs_4_0", 0, 0, &pVSBlob, &pErrorBlob);
    if (FAILED(hr)) {
        if (pErrorBlob) { printf("VS error: %s\n", (char*)pErrorBlob->GetBufferPointer()); pErrorBlob->Release(); }
        return false;
    }
    hr = D3DCompile(g_ShaderSource, strlen(g_ShaderSource), "PS", nullptr, nullptr, "PS", "ps_4_0", 0, 0, &pPSBlob, &pErrorBlob);
    if (FAILED(hr)) {
        if (pErrorBlob) { printf("PS error: %s\n", (char*)pErrorBlob->GetBufferPointer()); pErrorBlob->Release(); }
        return false;
    }

    g_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader);
    g_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    g_pDevice->CreateInputLayout(layout, 2, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pInputLayout);
    pVSBlob->Release();
    pPSBlob->Release();

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

    // Constant Buffer
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    g_pDevice->CreateBuffer(&cbd, nullptr, &g_pConstantBuffer);

    return true;
}

static void CleanD3D() {
    if (g_pConstantBuffer) g_pConstantBuffer->Release();
    if (g_pCubeIndexBuffer) g_pCubeIndexBuffer->Release();
    if (g_pCubeVertexBuffer) g_pCubeVertexBuffer->Release();
    if (g_pInputLayout) g_pInputLayout->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pDepthStencilState) g_pDepthStencilState->Release();
    if (g_pRasterizerState) g_pRasterizerState->Release();
    if (g_pDepthStencilView) g_pDepthStencilView->Release();
    if (g_pDepthStencilBuffer) g_pDepthStencilBuffer->Release();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pContext) g_pContext->Release();
    if (g_pDevice) g_pDevice->Release();
}

static void RenderCube(const XMMATRIX& world, const XMMATRIX& viewProj, const XMFLOAT4& color) {
    ConstantBuffer cb;
    cb.mWorldViewProj = XMMatrixTranspose(world * viewProj);
    cb.vColor = color;
    cb.vLightDir = XMFLOAT4(0.4f, -0.8f, 0.45f, 0.0f);
    g_pContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    g_pContext->IASetVertexBuffers(0, 1, &g_pCubeVertexBuffer, &stride, &offset);
    g_pContext->IASetIndexBuffer(g_pCubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    g_pContext->DrawIndexed(36, 0, 0);
}

static void RenderFrame(float dt) {
    if (!g_pContext || !g_pSwapChain) return;

    RECT rc;
    GetClientRect(g_hWnd, &rc);
    float width = static_cast<float>(rc.right - rc.left);
    float height = static_cast<float>(rc.bottom - rc.top);
    if (width <= 0 || height <= 0) return;

    D3D11_VIEWPORT vp = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
    g_pContext->RSSetViewports(1, &vp);

    // Deep Obsidian / Unreal Dark Slate Clear Color
    float clearColor[4] = { 0.08f, 0.09f, 0.12f, 1.0f };
    g_pContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);
    g_pContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    g_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
    g_pContext->OMSetDepthStencilState(g_pDepthStencilState, 1);
    g_pContext->RSSetState(g_pRasterizerState);

    g_pContext->IASetInputLayout(g_pInputLayout);
    g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pContext->VSSetShader(g_pVertexShader, nullptr, 0);
    g_pContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pContext->PSSetShader(g_pPixelShader, nullptr, 0);
    g_pContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);

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
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(80.0f), width / height, 0.1f, 300.0f);
    XMMATRIX viewProj = view * proj;

    // Update Simulated Unreal Engine Camera Position
    s_SimulatedCameraManager.CameraLocation = FVector(g_CamPos.x, g_CamPos.y, g_CamPos.z);
    s_SimulatedCameraManager.CameraRotation = FRotator(XMConvertToDegrees(g_CamPitch), XMConvertToDegrees(g_CamYaw), 0.0f);
    s_SimulatedWorld.TimeSeconds += dt;

    // 1. Draw 3D Checkered Floor Tiles
    for (int x = -10; x <= 10; ++x) {
        for (int z = -10; z <= 15; ++z) {
            XMMATRIX world = XMMatrixScaling(1.95f, 0.1f, 1.95f) * XMMatrixTranslation((float)x * 2.0f, 0.0f, (float)z * 2.0f);
            bool alt = ((x + z) % 2 == 0);
            XMFLOAT4 floorCol = alt ? XMFLOAT4(0.12f, 0.14f, 0.18f, 1.0f) : XMFLOAT4(0.09f, 0.10f, 0.13f, 1.0f);
            RenderCube(world, viewProj, floorCol);
        }
    }

    // 2. Draw Arena Obstacle Pillars
    float pillarXs[] = { -7.0f, 7.0f, -7.0f, 7.0f };
    float pillarZs[] = { 3.0f, 3.0f, 12.0f, 12.0f };
    for (int p = 0; p < 4; ++p) {
        XMMATRIX world = XMMatrixScaling(1.2f, 5.0f, 1.2f) * XMMatrixTranslation(pillarXs[p], 2.5f, pillarZs[p]);
        RenderCube(world, viewProj, XMFLOAT4(0.20f, 0.24f, 0.32f, 1.0f));
    }

    // 3. Draw 3D Target Mannequins (Bots)
    for (size_t i = 0; i < s_DummyBots.size(); ++i) {
        auto& bot = s_DummyBots[i];

        // Slight target oscillation
        float osc = std::sin(s_SimulatedWorld.TimeSeconds * 1.5f + (float)i) * 0.4f;
        float bx = bot.RootLocation.X + osc;
        float by = bot.RootLocation.Y;
        float bz = bot.RootLocation.Z;

        bot.HeadBone = FVector(bx, by + 1.6f, bz);
        bot.ChestBone = FVector(bx, by + 1.05f, bz);
        bot.PelvisBone = FVector(bx, by + 0.55f, bz);

        XMFLOAT4 teamColor = bot.bIsEnemy ? XMFLOAT4(0.95f, 0.25f, 0.25f, 1.0f) : XMFLOAT4(0.22f, 0.75f, 0.95f, 1.0f);

        // Head (Cube)
        XMMATRIX headWorld = XMMatrixScaling(0.35f, 0.35f, 0.35f) * XMMatrixTranslation(bx, by + 1.6f, bz);
        RenderCube(headWorld, viewProj, XMFLOAT4(teamColor.x * 1.15f, teamColor.y * 1.15f, teamColor.z * 1.15f, 1.0f));

        // Torso / Chest
        XMMATRIX torsoWorld = XMMatrixScaling(0.60f, 0.85f, 0.35f) * XMMatrixTranslation(bx, by + 1.05f, bz);
        RenderCube(torsoWorld, viewProj, teamColor);

        // Pelvis / Legs
        XMMATRIX legL = XMMatrixScaling(0.20f, 0.60f, 0.20f) * XMMatrixTranslation(bx - 0.18f, by + 0.30f, bz);
        XMMATRIX legR = XMMatrixScaling(0.20f, 0.60f, 0.20f) * XMMatrixTranslation(bx + 0.18f, by + 0.30f, bz);
        RenderCube(legL, viewProj, XMFLOAT4(0.18f, 0.20f, 0.24f, 1.0f));
        RenderCube(legR, viewProj, XMFLOAT4(0.18f, 0.20f, 0.24f, 1.0f));
    }

    // 4. Hookable Present Call
    g_pSwapChain->Present(1, 0);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_KEYDOWN:
        if (wParam == VK_TAB) {
            g_MouseCaptured = !g_MouseCaptured;
            ShowCursor(!g_MouseCaptured);
        } else if (wParam == VK_F5) {
            // Built-in shortcut to manually load solar.dll for injection testing
            HMODULE hMod = LoadLibraryA("solar.dll");
            if (!hMod) hMod = LoadLibraryA("solar_lib.dll");
            if (hMod) {
                printf("[MANUAL INJECT] Successfully loaded module via LoadLibraryA!\n");
            } else {
                printf("[MANUAL INJECT] solar.dll not found in working directory. Use your preferred injector.\n");
            }
        } else if (wParam == VK_F2) {
            // Spawn an extra bot
            AActor extraBot;
            extraBot.CharacterName = "TargetDummy_Spawned";
            extraBot.RootLocation = FVector(g_CamPos.x + std::sin(g_CamYaw) * 5.0f, 1.2f, g_CamPos.z + std::cos(g_CamYaw) * 5.0f);
            extraBot.Health = 100.0f;
            extraBot.MaxHealth = 100.0f;
            extraBot.bIsEnemy = true;
            s_DummyBots.push_back(extraBot);
            s_SimulatedLevel.Actors.push_back(&s_DummyBots.back());
            printf("[SPAWN] Spawned extra target dummy bot!\n");
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

    ShowWindow(g_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(g_hWnd);
    ShowCursor(FALSE);

    printf("[*] GWorld Exported Symbol Address: 0x%p\n", (void*)GWorld);
    printf("[*] SwapChain Address: 0x%p (Hook VTable Index 8: Present)\n", (void*)g_pSwapChain);
    printf("[*] Ready for DLL injection! Compatible with Xenos, Cheat Engine, Process Hacker, etc.\n");
    printf("[*] Controls: [WASD] Move | [Mouse] Look | [Tab] Release Mouse | [F5] Load DLL | [F2] Spawn Bot\n");
    printf("--------------------------------------------------------------------\n");

    LARGE_INTEGER freq, lastTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastTime);

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

        // Camera Mouse Look
        if (g_MouseCaptured && GetForegroundWindow() == g_hWnd) {
            POINT centerPt;
            RECT rc;
            GetClientRect(g_hWnd, &rc);
            centerPt.x = (rc.right - rc.left) / 2;
            centerPt.y = (rc.bottom - rc.top) / 2;
            ClientToScreen(g_hWnd, &centerPt);

            POINT curMouse;
            GetCursorPos(&curMouse);
            float dx = static_cast<float>(curMouse.x - centerPt.x);
            float dy = static_cast<float>(curMouse.y - centerPt.y);

            const float sens = 0.0022f;
            g_CamYaw += dx * sens;
            g_CamPitch -= dy * sens;
            g_CamPitch = (std::clamp)(g_CamPitch, -1.45f, 1.45f);

            SetCursorPos(centerPt.x, centerPt.y);
        }

        // Camera WASD Keyboard Movement
        float moveSpeed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? 14.0f : 7.0f;
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

        CheckForInjectedModules(dt);
        RenderFrame(dt);
    }

    CleanD3D();
    DestroyWindow(g_hWnd);
    return 0;
}
