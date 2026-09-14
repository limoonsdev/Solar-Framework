# 🔌 Integration Guide: Solar Framework in Cheats & Loaders

This guide provides real-world step-by-step instructions for integrating **Solar Framework** into game hacking projects, internal DLLs, DirectX 11 hooks (MinHook / Kiero), and external transparent overlays.

---

## 1. Internal DLL Hook (DirectX 11 Present Hook via Kiero / MinHook)

In an internal cheat DLL, you hook `IDXGISwapChain::Present` to render ImGui.

### Step A: Initialize ImGui & Solar once inside `Present`

```cpp
#include <d3d11.h>
#include <dxgi.h>
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>
#include <solar/solar.hpp>

static bool g_Initialized = false;
static ID3D11Device* g_Device = nullptr;
static ID3D11DeviceContext* g_Context = nullptr;
static ID3D11RenderTargetView* g_RenderTarget = nullptr;
static HWND g_GameHwnd = nullptr;

HRESULT __stdcall HookedPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (!g_Initialized) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_Device))) {
            g_Device->GetImmediateContext(&g_Context);

            DXGI_SWAP_CHAIN_DESC desc;
            pSwapChain->GetDesc(&desc);
            g_GameHwnd = desc.OutputWindow;

            ID3D11Texture2D* pBackBuffer = nullptr;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            g_Device->CreateRenderTargetView(pBackBuffer, NULL, &g_RenderTarget);
            pBackBuffer->Release();

            // Initialize ImGui
            ImGui::CreateContext();
            ImGui_ImplWin32_Init(g_GameHwnd);
            ImGui_ImplDX11_Init(g_Device, g_Context);

            // Initialize Solar Framework
            Solar::Initialize();

            g_Initialized = true;
        }
    }

    // Begin ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Solar frame tick (handles toasts and animations)
    Solar::NewFrame();

    // Render Watermark
    Solar::WatermarkInfo wm;
    wm.frameworkName = "SOLAR";
    wm.username = "SolarDev";
    wm.pingMs = 15;
    Solar::Watermark::Render(wm);

    // Render Menu on INSERT key toggle
    static bool menuOpen = true;
    if (GetAsyncKeyState(VK_INSERT) & 1) {
        menuOpen = !menuOpen;
    }

    if (menuOpen) {
        if (Solar::Widgets::BeginWindow("Solar Framework Menu", &menuOpen, ImVec2(890, 600))) {
            Solar::Widgets::RenderTitlebar("SOLAR", "INTERNAL v1.0", &menuOpen);

            // Put your cheat tabs, toggles, sliders here...
        }
        Solar::Widgets::EndWindow();
    }

    // Render ImGui draw lists
    ImGui::Render();
    g_Context->OMSetRenderTargets(1, &g_RenderTarget, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}
```

---

## 2. Window Message Hook (WndProc)

Hook the game's window procedure so ImGui can capture mouse clicks and keyboard inputs when the menu is open:

```cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall HookedWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // If menu is open and ImGui wants input, consume the message
    if (g_MenuOpen && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return true;
    }

    // Toggle menu with INSERT
    if (msg == WM_KEYDOWN && wParam == VK_INSERT) {
        g_MenuOpen = !g_MenuOpen;
        return 0;
    }

    return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}
```

---

## 3. External Transparent Overlay

If you are developing an external cheat (e.g. Kernel driver + UserMode Overlay or Topmost Transparent Window):

1. Create a `WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST` window.
2. Initialize Direct3D 11 with transparency:
   ```cpp
   MARGINS margins = { -1 };
   DwmExtendFrameIntoClientArea(hwnd, &margins);
   ```
3. Set background clear color to `ImVec4(0, 0, 0, 0)`.
4. Call `Solar::Initialize()` and render your ESP overlays and Solar menu seamlessly on top of any game screen.
