#pragma once
#include <d3d11.h>
#include <vector>

namespace Solar::Hook {

    /**
     * @brief DirectX 11 State Guard
     * Saves and restores all critical Direct3D 11 pipeline states before and after
     * ImGui rendering. Essential for internal game cheats (CS2, Valorant, Apex, Rust)
     * to prevent game crashes, flickering, depth/blend corruption, or viewport tearing.
     */
    class DX11StateGuard {
    public:
        DX11StateGuard() = default;
        ~DX11StateGuard();

        // Non-copyable
        DX11StateGuard(const DX11StateGuard&) = delete;
        DX11StateGuard& operator=(const DX11StateGuard&) = delete;

        void Capture(ID3D11DeviceContext* ctx);
        void Restore();

    private:
        ID3D11DeviceContext* m_ctx = nullptr;

        // Viewports & Scissor Rects
        UINT m_numViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        D3D11_VIEWPORT m_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
        UINT m_numScissorRects = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        D3D11_RECT m_scissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};

        // Rasterizer State
        ID3D11RasterizerState* m_rasterizerState = nullptr;

        // Output Merger States
        ID3D11BlendState* m_blendState = nullptr;
        FLOAT m_blendFactor[4] = {};
        UINT m_sampleMask = 0;
        ID3D11DepthStencilState* m_depthStencilState = nullptr;
        UINT m_stencilRef = 0;

        // Render Targets & Depth Stencil
        ID3D11RenderTargetView* m_renderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
        ID3D11DepthStencilView* m_depthStencilView = nullptr;

        // Shaders
        ID3D11VertexShader* m_vertexShader = nullptr;
        ID3D11PixelShader* m_pixelShader = nullptr;
        ID3D11GeometryShader* m_geometryShader = nullptr;

        // Input Assembler
        ID3D11InputLayout* m_inputLayout = nullptr;
        D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        ID3D11Buffer* m_indexBuffer = nullptr;
        DXGI_FORMAT m_indexFormat = DXGI_FORMAT_UNKNOWN;
        UINT m_indexOffset = 0;

        // Constant Buffers & Samplers
        ID3D11Buffer* m_vsConstantBuffers[4] = {};
        ID3D11Buffer* m_psConstantBuffers[4] = {};
        ID3D11SamplerState* m_psSamplers[4] = {};
        ID3D11ShaderResourceView* m_psShaderResources[4] = {};

        bool m_captured = false;
    };

} // namespace Solar::Hook
