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

        // Shaders
        m_ctx->VSGetShader(&m_vertexShader, nullptr, nullptr);
        m_ctx->PSGetShader(&m_pixelShader, nullptr, nullptr);
        m_ctx->GSGetShader(&m_geometryShader, nullptr, nullptr);

        // Input Assembler
        m_ctx->IAGetInputLayout(&m_inputLayout);
        m_ctx->IAGetPrimitiveTopology(&m_topology);
        m_ctx->IAGetIndexBuffer(&m_indexBuffer, &m_indexFormat, &m_indexOffset);

        // Constant Buffers & Samplers
        m_ctx->VSGetConstantBuffers(0, 4, m_vsConstantBuffers);
        m_ctx->PSGetConstantBuffers(0, 4, m_psConstantBuffers);
        m_ctx->PSGetSamplers(0, 4, m_psSamplers);
        m_ctx->PSGetShaderResources(0, 4, m_psShaderResources);

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

        // Restore Shaders
        m_ctx->VSSetShader(m_vertexShader, nullptr, 0);
        if (m_vertexShader) { m_vertexShader->Release(); m_vertexShader = nullptr; }

        m_ctx->PSSetShader(m_pixelShader, nullptr, 0);
        if (m_pixelShader) { m_pixelShader->Release(); m_pixelShader = nullptr; }

        m_ctx->GSSetShader(m_geometryShader, nullptr, 0);
        if (m_geometryShader) { m_geometryShader->Release(); m_geometryShader = nullptr; }

        // Restore Input Assembler
        m_ctx->IASetInputLayout(m_inputLayout);
        if (m_inputLayout) { m_inputLayout->Release(); m_inputLayout = nullptr; }

        m_ctx->IASetPrimitiveTopology(m_topology);
        m_ctx->IASetIndexBuffer(m_indexBuffer, m_indexFormat, m_indexOffset);
        if (m_indexBuffer) { m_indexBuffer->Release(); m_indexBuffer = nullptr; }

        // Restore Constant Buffers & Samplers
        m_ctx->VSSetConstantBuffers(0, 4, m_vsConstantBuffers);
        for (int i = 0; i < 4; ++i) {
            if (m_vsConstantBuffers[i]) { m_vsConstantBuffers[i]->Release(); m_vsConstantBuffers[i] = nullptr; }
        }

        m_ctx->PSSetConstantBuffers(0, 4, m_psConstantBuffers);
        for (int i = 0; i < 4; ++i) {
            if (m_psConstantBuffers[i]) { m_psConstantBuffers[i]->Release(); m_psConstantBuffers[i] = nullptr; }
        }

        m_ctx->PSSetSamplers(0, 4, m_psSamplers);
        for (int i = 0; i < 4; ++i) {
            if (m_psSamplers[i]) { m_psSamplers[i]->Release(); m_psSamplers[i] = nullptr; }
        }

        m_ctx->PSSetShaderResources(0, 4, m_psShaderResources);
        for (int i = 0; i < 4; ++i) {
            if (m_psShaderResources[i]) { m_psShaderResources[i]->Release(); m_psShaderResources[i] = nullptr; }
        }

        m_captured = false;
        m_ctx = nullptr;
    }

} // namespace Solar::Hook
