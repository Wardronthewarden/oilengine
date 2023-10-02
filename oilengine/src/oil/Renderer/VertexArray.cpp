#include "pch/oilpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace oil{
    VertexArray *VertexArray::Create()
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return new OpenGLVertexArray();
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
    }
}

