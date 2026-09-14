#include "solar/hook/hook_bridge.hpp"
#include "solar/solar.hpp"
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Solar::Hook {

    HookBridge& HookBridge::Get() {
        static HookBridge instance;
        return instance;
    }

    bool HookBridge::CreateRenderTarget() {
        if (!m_swapChain || !m_device) return false;

        ID3D11Texture2D* backBuffer = nullptr;
        HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        if (FAILED(hr) || !backBuffer) return false;

        hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
        backBuffer->Release();

        return SUCCEEDED(hr);
    }

    void HookBridge::CleanupRenderTarget() {
        if (m_renderTargetView) {
            m_renderTargetView->Release();
            m_renderTargetView = nullptr;
        }
    }

    bool HookBridge::Initialize(IDXGISwapChain* swapChain, HWND hWnd) {
        if (m_initialized) return true;
        if (!swapChain) return false;

        m_swapChain = swapChain;
        m_hWnd = hWnd;

        HRESULT hr = m_swapChain->GetDevice(IID_PPV_ARGS(&m_device));
        if (FAILED(hr) || !m_device) return false;

        m_device->GetImmediateContext(&m_context);
        if (!m_context) return false;

        if (!CreateRenderTarget()) return false;

        // Initialize Dear ImGui Core
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplWin32_Init(m_hWnd);
        ImGui_ImplDX11_Init(m_device, m_context);

        // Initialize Solar Framework subsystem
        Solar::Initialize();

        m_initialized = true;
        return true;
    }

    void HookBridge::Shutdown() {
        if (!m_initialized) return;

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupRenderTarget();

        if (m_context) { m_context->Release(); m_context = nullptr; }
        if (m_device)  { m_device->Release();  m_device = nullptr; }
        m_swapChain = nullptr;
        m_initialized = false;
    }

    void HookBridge::BeginFrame() {
        if (!m_initialized) return;

        // Capture full DirectX state to prevent game crash
        m_stateGuard.Capture(m_context);

        // Ensure render target view is bound
        m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Solar::NewFrame();
    }

    void HookBridge::EndFrame() {
        if (!m_initialized) return;

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Restore game DirectX pipeline state exactly as it was
        m_stateGuard.Restore();
    }

    void HookBridge::OnPreResize() {
        CleanupRenderTarget();
    }

    void HookBridge::OnPostResize() {
        CreateRenderTarget();
    }

    bool HookBridge::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (!m_initialized) return false;

        // Forward to ImGui Win32 implementation
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
            return true;
        }

        // When menu is active, consume mouse and keyboard input to prevent game from shooting/moving
        if (m_menuOpen) {
            switch (msg) {
                case WM_LBUTTONDOWN: case WM_LBUTTONUP:
                case WM_RBUTTONDOWN: case WM_RBUTTONUP:
                case WM_MBUTTONDOWN: case WM_MBUTTONUP:
                case WM_MOUSEWHEEL:  case WM_MOUSEMOVE:
                case WM_KEYDOWN:     case WM_KEYUP:
                case WM_CHAR:
                    return true;
                default:
                    break;
            }
        }

        return false;
    }

} // namespace Solar::Hook
