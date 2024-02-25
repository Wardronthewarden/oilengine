#include "pch/oilpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace oil{
    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RendererAPI::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
        }
        OIL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(float *vertices, uint32_t size)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RendererAPI::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices, size);
        }
        OIL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t count)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(count);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI::API!")
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t *vertices, uint32_t count)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(vertices, count);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI::API!")
        return nullptr;
    }
}