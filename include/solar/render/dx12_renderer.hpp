#pragma once
#include "irenderer.hpp"

namespace Solar::Render {

    // DirectX 12 renderer abstraction stub for high performance modern graphics pipelines
    class DX12Renderer : public IRenderer {
    public:
        DX12Renderer() = default;
        virtual ~DX12Renderer() = default;

        bool Initialize(void* windowHandle) override { return true; }
        void Shutdown() override {}
        void BeginFrame() override {}
        void EndFrame() override {}
        void Resize(u32 width, u32 height) override {}
    };

} // namespace Solar::Render
