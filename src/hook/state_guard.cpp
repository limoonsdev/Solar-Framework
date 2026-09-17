#include "solar/hook/state_guard.hpp"

namespace Solar::Hook {

    DX11StateGuard::~DX11StateGuard() {
        Restore();
    }

    void DX11StateGuard::Capture(ID3D11DeviceContext* ctx) {
        if (!ctx) return;
        m_ctx = ctx;

        // Viewports and Scissor
        m_numViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        m_ctx->RSGetViewports(&m_numViewports, m_viewports);

        m_numScissorRects = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        m_ctx->RSGetScissorRects(&m_numScissorRects, m_scissorRects);

        // Rasterizer
        m_ctx->RSGetState(&m_rasterizerState);

        // Output Merger Blend & Depth
        m_ctx->OMGetBlendState(&m_blendState, m_blendFactor, &m_sampleMask);
        m_ctx->OMGetDepthStencilState(&m_depthStencilState, &m_stencilRef);

        // Render Targets
        m_ctx->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, m_renderTargetViews, &m_depthStencilView);

        // 6-Stage Shaders (VS, PS, GS, HS, DS, CS)
        m_ctx->VSGetShader(&m_vertexShader, nullptr, nullptr);
        m_ctx->PSGetShader(&m_pixelShader, nullptr, nullptr);
        m_ctx->GSGetShader(&m_geometryShader, nullptr, nullptr);
        m_ctx->HSGetShader(&m_hullShader, nullptr, nullptr);
        m_ctx->DSGetShader(&m_domainShader, nullptr, nullptr);
        m_ctx->CSGetShader(&m_computeShader, nullptr, nullptr);

        // Explicitly unbind Hull, Domain and Geometry shaders during Solar-Render / ImGui pass
        // to prevent UE4/UE5 topology/tessellation pipeline crashes on DrawIndexed
        m_ctx->HSSetShader(nullptr, nullptr, 0);
        m_ctx->DSSetShader(nullptr, nullptr, 0);
        m_ctx->GSSetShader(nullptr, nullptr, 0);

        // Input Assembler
        m_ctx->IAGetInputLayout(&m_inputLayout);
        m_ctx->IAGetPrimitiveTopology(&m_topology);
        m_ctx->IAGetIndexBuffer(&m_indexBuffer, &m_indexFormat, &m_indexOffset);

        // Full 14-Slot Constant Buffers (VS, PS, GS)
        m_ctx->VSGetConstantBuffers(0, kMaxConstantBuffers, m_vsConstantBuffers);
        m_ctx->PSGetConstantBuffers(0, kMaxConstantBuffers, m_psConstantBuffers);
        m_ctx->GSGetConstantBuffers(0, kMaxConstantBuffers, m_gsConstantBuffers);

        // Samplers & SRVs (16 slots)
        m_ctx->PSGetSamplers(0, kMaxSamplers, m_psSamplers);
        m_ctx->PSGetShaderResources(0, kMaxSRVs, m_psShaderResources);
        m_ctx->VSGetSamplers(0, kMaxSamplers, m_vsSamplers);
        m_ctx->VSGetShaderResources(0, kMaxSRVs, m_vsShaderResources);

        // Compute Shader Unordered Access Views (UAVs)
        m_ctx->CSGetUnorderedAccessViews(0, kMaxUAVs, m_csUAVs);

        m_captured = true;
    }

    void DX11StateGuard::Restore() {
        if (!m_captured || !m_ctx) return;

        // Restore Viewports and Scissor
        m_ctx->RSSetViewports(m_numViewports, m_viewports);
        m_ctx->RSSetScissorRects(m_numScissorRects, m_scissorRects);

        // Restore Rasterizer
        m_ctx->RSSetState(m_rasterizerState);
        if (m_rasterizerState) { m_rasterizerState->Release(); m_rasterizerState = nullptr; }

        // Restore Output Merger
        m_ctx->OMSetBlendState(m_blendState, m_blendFactor, m_sampleMask);
        if (m_blendState) { m_blendState->Release(); m_blendState = nullptr; }

        m_ctx->OMSetDepthStencilState(m_depthStencilState, m_stencilRef);
        if (m_depthStencilState) { m_depthStencilState->Release(); m_depthStencilState = nullptr; }

        m_ctx->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, m_renderTargetViews, m_depthStencilView);
        for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
            if (m_renderTargetViews[i]) { m_renderTargetViews[i]->Release(); m_renderTargetViews[i] = nullptr; }
        }
        if (m_depthStencilView) { m_depthStencilView->Release(); m_depthStencilView = nullptr; }

        // Restore All 6 Shader Stages
        m_ctx->VSSetShader(m_vertexShader, nullptr, 0);
        if (m_vertexShader) { m_vertexShader->Release(); m_vertexShader = nullptr; }

        m_ctx->PSSetShader(m_pixelShader, nullptr, 0);
        if (m_pixelShader) { m_pixelShader->Release(); m_pixelShader = nullptr; }

        m_ctx->GSSetShader(m_geometryShader, nullptr, 0);
        if (m_geometryShader) { m_geometryShader->Release(); m_geometryShader = nullptr; }

        m_ctx->HSSetShader(m_hullShader, nullptr, 0);
        if (m_hullShader) { m_hullShader->Release(); m_hullShader = nullptr; }

        m_ctx->DSSetShader(m_domainShader, nullptr, 0);
        if (m_domainShader) { m_domainShader->Release(); m_domainShader = nullptr; }

        m_ctx->CSSetShader(m_computeShader, nullptr, 0);
        if (m_computeShader) { m_computeShader->Release(); m_computeShader = nullptr; }

        // Restore Input Assembler
        m_ctx->IASetInputLayout(m_inputLayout);
        if (m_inputLayout) { m_inputLayout->Release(); m_inputLayout = nullptr; }

        m_ctx->IASetPrimitiveTopology(m_topology);
        m_ctx->IASetIndexBuffer(m_indexBuffer, m_indexFormat, m_indexOffset);
        if (m_indexBuffer) { m_indexBuffer->Release(); m_indexBuffer = nullptr; }

        // Restore 14-Slot Constant Buffers
        m_ctx->VSSetConstantBuffers(0, kMaxConstantBuffers, m_vsConstantBuffers);
        for (UINT i = 0; i < kMaxConstantBuffers; ++i) {
            if (m_vsConstantBuffers[i]) { m_vsConstantBuffers[i]->Release(); m_vsConstantBuffers[i] = nullptr; }
        }

        m_ctx->PSSetConstantBuffers(0, kMaxConstantBuffers, m_psConstantBuffers);
        for (UINT i = 0; i < kMaxConstantBuffers; ++i) {
            if (m_psConstantBuffers[i]) { m_psConstantBuffers[i]->Release(); m_psConstantBuffers[i] = nullptr; }
        }

        m_ctx->GSSetConstantBuffers(0, kMaxConstantBuffers, m_gsConstantBuffers);
        for (UINT i = 0; i < kMaxConstantBuffers; ++i) {
            if (m_gsConstantBuffers[i]) { m_gsConstantBuffers[i]->Release(); m_gsConstantBuffers[i] = nullptr; }
        }

        // Restore Samplers & SRVs
        m_ctx->PSSetSamplers(0, kMaxSamplers, m_psSamplers);
        for (UINT i = 0; i < kMaxSamplers; ++i) {
            if (m_psSamplers[i]) { m_psSamplers[i]->Release(); m_psSamplers[i] = nullptr; }
        }

        m_ctx->PSSetShaderResources(0, kMaxSRVs, m_psShaderResources);
        for (UINT i = 0; i < kMaxSRVs; ++i) {
            if (m_psShaderResources[i]) { m_psShaderResources[i]->Release(); m_psShaderResources[i] = nullptr; }
        }

        m_ctx->VSSetSamplers(0, kMaxSamplers, m_vsSamplers);
        for (UINT i = 0; i < kMaxSamplers; ++i) {
            if (m_vsSamplers[i]) { m_vsSamplers[i]->Release(); m_vsSamplers[i] = nullptr; }
        }

        m_ctx->VSSetShaderResources(0, kMaxSRVs, m_vsShaderResources);
        for (UINT i = 0; i < kMaxSRVs; ++i) {
            if (m_vsShaderResources[i]) { m_vsShaderResources[i]->Release(); m_vsShaderResources[i] = nullptr; }
        }

        // Restore Compute Shader UAVs
        UINT uavInitialCounts[kMaxUAVs] = { (UINT)-1, (UINT)-1, (UINT)-1, (UINT)-1, (UINT)-1, (UINT)-1, (UINT)-1, (UINT)-1 };
        m_ctx->CSSetUnorderedAccessViews(0, kMaxUAVs, m_csUAVs, uavInitialCounts);
        for (UINT i = 0; i < kMaxUAVs; ++i) {
            if (m_csUAVs[i]) { m_csUAVs[i]->Release(); m_csUAVs[i] = nullptr; }
        }

        m_captured = false;
        m_ctx = nullptr;
    }

} // namespace Solar::Hook
