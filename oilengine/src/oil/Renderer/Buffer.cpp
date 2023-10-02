#include "pch/oilpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace oil{
    VertexBuffer *VertexBuffer::Create(float *vertices, uint32_t size)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RendererAPI::None currently not supported!");
            case RenderAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);
        }
        OIL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }

    IndexBuffer *IndexBuffer::Create(uint32_t* vertices, uint32_t size)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return new OpenGLIndexBuffer(vertices, size);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI::API!")
        return nullptr;
    }
}