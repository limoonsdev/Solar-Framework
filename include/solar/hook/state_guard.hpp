#pragma once
#include <d3d11.h>
#include <vector>

namespace Solar::Hook {

    /**
     * @brief DirectX 11 Pipeline State Guard (Solar-Render Isolation)
     * Saves and restores all critical Direct3D 11 pipeline states before and after
     * ImGui rendering. Essential for Unreal Engine 4/5 (Fortnite, Valorant, Arc Raiders),
     * Unity, and high-performance game overlays and telemetry tools to prevent
     * GPU driver crashes (DXGI_ERROR_DEVICE_REMOVED), geometry pipeline mismatches, or depth tearing.
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

        // 6-Stage Shaders (Full UE4/UE5 Geometry & Compute Isolation)
        ID3D11VertexShader*   m_vertexShader = nullptr;
        ID3D11PixelShader*    m_pixelShader = nullptr;
        ID3D11GeometryShader* m_geometryShader = nullptr;
        ID3D11HullShader*     m_hullShader = nullptr;
        ID3D11DomainShader*   m_domainShader = nullptr;
        ID3D11ComputeShader*  m_computeShader = nullptr;

        // Input Assembler
        ID3D11InputLayout* m_inputLayout = nullptr;
        D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        ID3D11Buffer* m_indexBuffer = nullptr;
        DXGI_FORMAT m_indexFormat = DXGI_FORMAT_UNKNOWN;
        UINT m_indexOffset = 0;

        // Constant Buffers (14 slots per D3D11 API spec)
        static constexpr UINT kMaxConstantBuffers = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; // 14
        ID3D11Buffer* m_vsConstantBuffers[kMaxConstantBuffers] = {};
        ID3D11Buffer* m_psConstantBuffers[kMaxConstantBuffers] = {};
        ID3D11Buffer* m_gsConstantBuffers[kMaxConstantBuffers] = {};

        // Samplers & Shader Resource Views (16 slots)
        static constexpr UINT kMaxSamplers = 16;
        static constexpr UINT kMaxSRVs = 16;
        ID3D11SamplerState* m_psSamplers[kMaxSamplers] = {};
        ID3D11ShaderResourceView* m_psShaderResources[kMaxSRVs] = {};
        ID3D11SamplerState* m_vsSamplers[kMaxSamplers] = {};
        ID3D11ShaderResourceView* m_vsShaderResources[kMaxSRVs] = {};

        // Compute Shader UAVs (8 slots)
        static constexpr UINT kMaxUAVs = 8;
        ID3D11UnorderedAccessView* m_csUAVs[kMaxUAVs] = {};

        bool m_captured = false;
    };

} // namespace Solar::Hook
