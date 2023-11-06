#include "pch/oilpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace oil{
    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
    }

Ref<Texture2D> Texture2D::Create(const std::string &path)
{
    switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
}
}