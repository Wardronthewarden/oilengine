#include "pch/oilpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace oil{
    Ref<Texture2D> Texture2D::Create(TextureSettings settings, TextureParams params)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(settings, params);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
    }

    template <>
    ContentType AssetRef<Texture2D>::GetType()
    {
        return ContentType::Texture2D;
    }

    Ref<TextureCube> TextureCube::Create(TextureSettings settings, TextureParams params)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLTextureCube>(settings, params);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
    }
    
}