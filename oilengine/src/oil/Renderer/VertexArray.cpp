#include "pch/oilpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace oil{
    VertexArray *VertexArray::Create()
    {
        switch (Renderer::GetAPI()){
            case RendererAPI::None: OIL_CORE_ASSERT(false, "RendererAPI::None currently not supported!");
            case RendererAPI::OpenGL: return new OpenGLVertexArray();
        }
        OIL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }
}

