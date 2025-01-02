#include "pch/oilpch.h"
#include "FrameBuffer.h"

#include "oil/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace oil{

    Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification &spec)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RendererAPI::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLFrameBuffer>(spec);
        }
        OIL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }
}