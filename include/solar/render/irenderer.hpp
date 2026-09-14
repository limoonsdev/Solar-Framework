#pragma once
#include "solar/core/types.hpp"

namespace Solar::Render {

    class IRenderer {
    public:
        virtual ~IRenderer() = default;
        virtual bool Initialize(void* windowHandle) = 0;
        virtual void Shutdown() = 0;
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Resize(u32 width, u32 height) = 0;
    };

} // namespace Solar::Render
