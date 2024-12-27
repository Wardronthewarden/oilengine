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

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, TextureFormat format)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height, format);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
    }

    template <>
    ContentType AssetRef<Texture2D>::GetType()
    {
        return ContentType::Texture2D;
    }

    Ref<TextureCube> TextureCube::Create(std::vector<Ref<Texture2D>> faces)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLTextureCube>(faces);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
    }


    Ref<TextureCube> TextureCube::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLTextureCube>(width, height);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
    }
    Ref<TextureCube> TextureCube::Create(uint32_t width, uint32_t height, TextureFormat format)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return CreateRef<OpenGLTextureCube>(width, height, format);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!")
        return nullptr;
    }

    TextureTarget operator +(TextureTarget t, int i){
        return static_cast<TextureTarget>((uint32_t)t + i);
    };
    
}