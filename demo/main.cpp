#include <windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <tchar.h>
#include <shellscalingapi.h>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

#include "solar/solar.hpp"
#include "solar/render/fa_solid_data.hpp"
#include "demo_app.hpp"
#include <vector>
#include <cstdio>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shcore.lib")
#pragma comment(lib, "dwmapi.lib")
#include <dwmapi.h>

struct ACCENT_POLICY { int State; int Flags; int Color; int AnimationId; };
struct WINCOMPATTRDATA { int Attr; PVOID Data; ULONG Size; };
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);

static void EnableWindowAcrylicBlur(HWND hwnd) {
    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    HMODULE hUser32 = GetModuleHandleA("user32.dll");
    if (!hUser32) return;
    auto fn = (pSetWindowCompositionAttribute)GetProcAddress(hUser32, "SetWindowCompositionAttribute");
    if (fn) {
        ACCENT_POLICY accent = { 4, 2, 0x00FFFFFF, 0 }; // ACCENT_ENABLE_ACRYLICBLURBEHIND
        WINCOMPATTRDATA data = { 19, &accent, sizeof(accent) };
        fn(hwnd, &data);
    }
}

static ID3D11Device*           g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

static void SaveBackBufferToBmp(ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapChain, const char* filepath) {
    if (!device || !context || !swapChain) return;
    ID3D11Texture2D* pBackBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer))) return;

    D3D11_TEXTURE2D_DESC desc;
    pBackBuffer->GetDesc(&desc);

    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.MiscFlags = 0;

    ID3D11Texture2D* pStaging = nullptr;
    if (FAILED(device->CreateTexture2D(&desc, nullptr, &pStaging))) {
        pBackBuffer->Release();
        return;
    }

    context->CopyResource(pStaging, pBackBuffer);

    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(context->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped))) {
        BITMAPFILEHEADER bfh = { 0 };
        BITMAPINFOHEADER bih = { 0 };
        bfh.bfType = 0x4D42;
        bfh.bfOffBits = sizeof(bfh) + sizeof(bih);
        bfh.bfSize = bfh.bfOffBits + desc.Width * desc.Height * 4;

        bih.biSize = sizeof(bih);
        bih.biWidth = desc.Width;
        bih.biHeight = -(LONG)desc.Height;
        bih.biPlanes = 1;
        bih.biBitCount = 32;
        bih.biCompression = BI_RGB;

        FILE* f = fopen(filepath, "wb");
        if (f) {
            fwrite(&bfh, sizeof(bfh), 1, f);
            fwrite(&bih, sizeof(bih), 1, f);
            const unsigned char* src = (const unsigned char*)mapped.pData;
            std::vector<unsigned char> row(desc.Width * 4);
            for (UINT y = 0; y < desc.Height; ++y) {
                const unsigned char* rowSrc = src + y * mapped.RowPitch;
                for (UINT x = 0; x < desc.Width; ++x) {
                    row[x * 4 + 0] = rowSrc[x * 4 + 2];
                    row[x * 4 + 1] = rowSrc[x * 4 + 1];
                    row[x * 4 + 2] = rowSrc[x * 4 + 0];
                    row[x * 4 + 3] = 255;
                }
                fwrite(row.data(), desc.Width * 4, 1, f);
            }
            fclose(f);
        }
        context->Unmap(pStaging, 0);
    }
    pStaging->Release();
    pBackBuffer->Release();
}

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    AttachConsole(ATTACH_PARENT_PROCESS);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    const char* cmdLine = GetCommandLineA();
    printf("[DEMO] CmdLine: %s\n", cmdLine);

    // 1. High-DPI Per-Monitor v2 Awareness to eliminate all DWM bilinear scaling blur
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"SolarFrameworkClass", nullptr };
    ::RegisterClassExW(&wc);

    HDC screenDC = GetDC(NULL);
    int screenDpi = GetDeviceCaps(screenDC, LOGPIXELSX);
    ReleaseDC(NULL, screenDC);
    float initialDpiScale = (screenDpi > 0) ? (static_cast<float>(screenDpi) / 96.0f) : 1.0f;
    if (initialDpiScale < 1.0f) initialDpiScale = 1.0f;

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = static_cast<int>(1120 * initialDpiScale);
    int winH = static_cast<int>(750 * initialDpiScale);
    int posX = (screenW - winW) / 2;
    int posY = (screenH - winH) / 2;

    HWND hwnd = ::CreateWindowW(
        wc.lpszClassName,
        L"Solar Framework v1.0.1 | Industrial ImGui Suite (Ultra-Crisp 1:1 DPI)",
        WS_OVERLAPPEDWINDOW,
        posX, posY, winW, winH,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    // Windows 11 Dark Mode Titlebar & Mica/Dark framing
    BOOL darkMode = TRUE;
    ::DwmSetWindowAttribute(hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &darkMode, sizeof(darkMode));
    EnableWindowAcrylicBlur(hwnd);

    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Initialize backend renderer early so ImGuiBackendFlags_RendererHasTextures is set
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Retrieve exact window DPI scale factor
    UINT winDpi = GetDpiForWindow(hwnd);
    float dpiScale = (winDpi > 0) ? (static_cast<float>(winDpi) / 96.0f) : initialDpiScale;
    if (dpiScale < 1.0f) dpiScale = 1.0f;

    // Initialize Razor-Sharp High-DPI Fonts & Gaming Typography Suite via FontManager
    Solar::Render::FontManager::Get().LoadFonts(dpiScale);

    // Note: With modern Dear ImGui backends, io.Fonts->Build() is called automatically by ImGui_ImplDX11_NewFrame().

    // Initialize Solar Framework & Sound System
    Solar::Initialize();
    Solar::DemoApp::Get().Initialize();

    ImVec4 clear_color = ImVec4(0.043f, 0.051f, 0.075f, 1.00f);

    const char* fullCmd = GetCommandLineA();
    bool autoScreenshot = (strstr(fullCmd, "--screenshot") != nullptr);
    int screenshotFrame = 0;
    if (autoScreenshot) {
        printf("[DEMO] autoScreenshot enabled! skip_intro=%d\n", strstr(fullCmd, "--skip-intro") != nullptr);
        if (strstr(fullCmd, "--skip-intro") != nullptr) {
            Solar::UI::SplashScreen::Get().Start(0.001f);
            Solar::UI::WelcomeScreen::Get().Hide();
        }
        if (strstr(fullCmd, "--screenshot-radar") != nullptr) {
            Solar::DemoApp::Get().SetCurrentTab(2);
            Solar::DemoApp::Get().SetMiscSubTab(0);
        } else if (strstr(fullCmd, "--screenshot-watermarks") != nullptr) {
            Solar::DemoApp::Get().SetCurrentTab(2);
            Solar::DemoApp::Get().SetMiscSubTab(1);
        } else if (strstr(fullCmd, "--screenshot-themes") != nullptr) {
            Solar::DemoApp::Get().SetCurrentTab(6);
            Solar::DemoApp::Get().SetThemeSubTab(0);
        } else if (strstr(fullCmd, "--screenshot-widgets") != nullptr) {
            Solar::DemoApp::Get().SetCurrentTab(3);
            Solar::DemoApp::Get().SetWidgetsPage(1);
        } else if (strstr(fullCmd, "--screenshot-visuals") != nullptr) {
            Solar::DemoApp::Get().SetCurrentTab(1);
        }
    } else {
        // Normal interactive launch: quick 0.5s splash transition straight into workspace
        Solar::UI::SplashScreen::Get().Start(0.5f);
        Solar::UI::WelcomeScreen::Get().Hide();
    }

    bool done = false;
    while (!done) {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done) break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Solar::NewFrame();

        ImVec2 centerPos = ImVec2(
            (io.DisplaySize.x - 940.0f) * 0.5f,
            (io.DisplaySize.y - 620.0f) * 0.5f
        );
        ImGui::SetNextWindowPos(centerPos, ImGuiCond_FirstUseEver);

        if (autoScreenshot && (io.MousePos.x < 0.0f || io.MousePos.x > io.DisplaySize.x)) {
            io.MousePos = ImVec2(centerPos.x + 380.0f, centerPos.y + 190.0f);
        }

        Solar::DemoApp::Get().Render();

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x, clear_color.y, clear_color.z, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);

        if (autoScreenshot) {
            screenshotFrame++;
            if (screenshotFrame >= 15) {
                const char* targetFile = "demo_capture.bmp";
                if (strstr(fullCmd, "--screenshot-visuals") != nullptr) targetFile = "visuals_capture.bmp";
                else if (strstr(fullCmd, "--screenshot-watermarks") != nullptr) targetFile = "watermarks_capture.bmp";
                else if (strstr(fullCmd, "--screenshot-themes") != nullptr) targetFile = "themes_capture.bmp";
                else if (strstr(fullCmd, "--screenshot-radar") != nullptr) targetFile = "radar_capture.bmp";
                else if (strstr(fullCmd, "--screenshot-widgets") != nullptr) targetFile = "widgets_capture.bmp";
                char envPath[MAX_PATH];
                if (GetEnvironmentVariableA("SOLAR_SCREENSHOT_PATH", envPath, sizeof(envPath)) > 0) {
                    targetFile = envPath;
                }
                printf("[DEMO] Capturing screenshot to %s on frame %d\n", targetFile, screenshotFrame);
                SaveBackBufferToBmp(g_pd3dDevice, g_pd3dDeviceContext, g_pSwapChain, targetFile);
                printf("[DEMO] Capture complete!\n");
                done = true;
            }
        }
    }

    Solar::Shutdown();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
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

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd,
        &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext
    );

    if (res == DXGI_ERROR_UNSUPPORTED) {
        res = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
            featureLevelArray, 2, D3D11_SDK_VERSION, &sd,
            &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext
        );
    }
    if (res != S_OK) return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_DPICHANGED: {
        RECT* const prcNewWindow = (RECT*)lParam;
        SetWindowPos(hWnd, NULL,
            prcNewWindow->left, prcNewWindow->top,
            prcNewWindow->right - prcNewWindow->left,
            prcNewWindow->bottom - prcNewWindow->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
