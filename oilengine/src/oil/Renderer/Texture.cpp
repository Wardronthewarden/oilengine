#include "pch/oilpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace oil{
Ref<Texture2D> Texture2D::Create(const std::string &path)
{
    switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
}
}