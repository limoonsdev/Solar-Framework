#pragma once
#include "irenderer.hpp"
#include <d3d11.h>
#include <dxgi.h>

namespace Solar::Render {

    class DX11Renderer : public IRenderer {
    public:
        DX11Renderer();
        virtual ~DX11Renderer();

        bool Initialize(void* windowHandle) override;
        void Shutdown() override;
        void BeginFrame() override;
        void EndFrame() override;
        void Resize(u32 width, u32 height) override;

        ID3D11Device* GetDevice() const { return m_device; }
        ID3D11DeviceContext* GetContext() const { return m_context; }
        IDXGISwapChain* GetSwapChain() const { return m_swapChain; }

    private:
        ID3D11Device* m_device = nullptr;
        ID3D11DeviceContext* m_context = nullptr;
        IDXGISwapChain* m_swapChain = nullptr;
        ID3D11RenderTargetView* m_renderTargetView = nullptr;
        HWND m_hwnd = nullptr;

        bool CreateRenderTarget();
        void CleanupRenderTarget();
    };

} // namespace Solar::Render
