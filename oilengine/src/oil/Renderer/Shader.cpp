#include "pch/oilpch.h"
#include "Shader.h"
#include <oil/storage/Asset.h>


#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace oil{
    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!");
        return nullptr;
    }
    Ref<Shader> Shader::Create(const std::string &filepath)
    {
        switch (Renderer::GetAPI()){
            case RenderAPI::API::None: OIL_CORE_ASSERT(false, "RenderAPI::API::None currently not supported!");
            case RenderAPI::API::OpenGL: return std::make_shared<OpenGLShader>(filepath);
        }
        OIL_CORE_ASSERT(false, "Unknown RenderAPI!");
        return nullptr;
    }
    void ShaderLibrary::Add(const std::string &name, const Ref<Shader> &shader)
    {
        OIL_CORE_ASSERT(!Exists(name), "Shader already exists!");
        m_Shaders[name] = shader;
    }
    void ShaderLibrary::Add(const Ref<Shader> &shader)
    {
        auto& name = shader->GetName();
        Add(name, shader);
        
    }
    Ref<Shader> ShaderLibrary::Load(const std::string &filepath)
    {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }
    Ref<Shader> ShaderLibrary::Load(const std::string &name, const std::string &filepath)
    {
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }
    bool ShaderLibrary::Exists(const std::string &name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }
    Ref<Shader> ShaderLibrary::Get(const std::string &name)
    {
        OIL_CORE_ASSERT(Exists(name), "Shader not found!");
        return m_Shaders[name];
    }

    template <>
    ContentType AssetRef<Shader>::GetType()
    {
        return ContentType::Shader;
    }
    
}
