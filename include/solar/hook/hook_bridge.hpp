#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <windows.h>
#include <functional>
#include "solar/hook/state_guard.hpp"

namespace Solar::Hook {

    /**
     * @brief Internal Hook Bridge
     * Simplifies hooking IDXGISwapChain::Present, ResizeBuffers, and WndProc.
     * Guarantees crash-proof rendering by managing render target lifecycle
     * and automatically protecting DirectX state.
     */
    class HookBridge {
    public:
        static HookBridge& Get();

        bool Initialize(IDXGISwapChain* swapChain, HWND hWnd);
        void Shutdown();

        // Call before rendering your UI / ESP
        void BeginFrame();

        // Call after rendering your UI / ESP (restores game state)
        void EndFrame();

        // Must be called inside hooked ResizeBuffers (pre-call)
        void OnPreResize();

        // Must be called inside hooked ResizeBuffers (post-call)
        void OnPostResize();

        // Safe WndProc hook handler - returns true if Solar consumed the input
        bool WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // Control whether the game receives input when Solar menu is open
        void SetInputCapture(bool capture) { m_menuOpen = capture; }
        bool IsInputCaptured() const { return m_menuOpen; }

        ID3D11Device* GetDevice() const { return m_device; }
        ID3D11DeviceContext* GetContext() const { return m_context; }
        HWND GetWindowHandle() const { return m_hWnd; }

    private:
        HookBridge() = default;
        ~HookBridge() = default;

        bool CreateRenderTarget();
        void CleanupRenderTarget();

        IDXGISwapChain* m_swapChain = nullptr;
        ID3D11Device* m_device = nullptr;
        ID3D11DeviceContext* m_context = nullptr;
        ID3D11RenderTargetView* m_renderTargetView = nullptr;
        HWND m_hWnd = nullptr;

        DX11StateGuard m_stateGuard;
        bool m_initialized = false;
        bool m_menuOpen = true;
    };

} // namespace Solar::Hook
