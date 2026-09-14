#include "solar/render/dx11_renderer.hpp"

namespace Solar::Render {

    DX11Renderer::DX11Renderer() = default;

    DX11Renderer::~DX11Renderer() {
        Shutdown();
    }

    bool DX11Renderer::Initialize(void* windowHandle) {
        m_hwnd = reinterpret_cast<HWND>(windowHandle);
        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = m_hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            featureLevelArray, 2, D3D11_SDK_VERSION, &sd,
            &m_swapChain, &m_device, &featureLevel, &m_context
        );
        if (FAILED(hr)) return false;

        return CreateRenderTarget();
    }

    void DX11Renderer::Shutdown() {
        CleanupRenderTarget();
        if (m_swapChain) { m_swapChain->Release(); m_swapChain = nullptr; }
        if (m_context)   { m_context->Release(); m_context = nullptr; }
        if (m_device)    { m_device->Release(); m_device = nullptr; }
    }

    void DX11Renderer::BeginFrame() {
        if (m_renderTargetView && m_context) {
            const float clear_color[4] = { 0.04f, 0.045f, 0.06f, 1.0f };
            m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
            m_context->ClearRenderTargetView(m_renderTargetView, clear_color);
        }
    }

    void DX11Renderer::EndFrame() {
        if (m_swapChain) {
            m_swapChain->Present(1, 0);
        }
    }

    void DX11Renderer::Resize(u32 width, u32 height) {
        if (width == 0 || height == 0 || !m_swapChain) return;
        CleanupRenderTarget();
        m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        CreateRenderTarget();
    }

    bool DX11Renderer::CreateRenderTarget() {
        ID3D11Texture2D* pBackBuffer = nullptr;
        if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) return false;
        HRESULT hr = m_device->CreateRenderTargetView(pBackBuffer, nullptr, &m_renderTargetView);
        pBackBuffer->Release();
        return SUCCEEDED(hr);
    }

    void DX11Renderer::CleanupRenderTarget() {
        if (m_renderTargetView) {
            m_renderTargetView->Release();
            m_renderTargetView = nullptr;
        }
    }

} // namespace Solar::Render
