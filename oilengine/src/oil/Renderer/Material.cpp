#include <pch/oilpch.h>
#include "oil/Renderer/Material.h"
#include <oil/Renderer/Shader.h>
#include "oil/storage/Asset.h"
#include "Material.h"

namespace oil{
    Material::Material(AssetRef<Shader> shader)
        : m_Shader(shader)
    {
        UpdateUniforms();
    }

    Material::Material(const Material& other){
        m_Shader = other.m_Shader;
        m_UniformInts = other.m_UniformInts;
        m_UniformFloats = other.m_UniformFloats;
        m_UniformFloat2s = other.m_UniformFloat2s;
        m_UniformFloat3s = other.m_UniformFloat3s;
        m_UniformFloat4s = other.m_UniformFloat4s;
    }

    Material::~Material(){}

    AssetRef<Shader> Material::GetShader() const
    {
        return m_Shader; 
    }

    template <>
    std::unordered_map<std::string, int> &Material::GetUniformsOfType<int>()
    {
        return m_UniformInts;
    }
    template <>
    std::unordered_map<std::string, float> &Material::GetUniformsOfType<float>()
    {
        return m_UniformFloats;
    }
    template <>
    std::unordered_map<std::string, glm::vec2> &Material::GetUniformsOfType<glm::vec2>()
    {
        return m_UniformFloat2s;
    }
    template <>
    std::unordered_map<std::string, glm::vec3> &Material::GetUniformsOfType<glm::vec3>()
    {
        return m_UniformFloat3s;
    }
    template <>
    std::unordered_map<std::string, glm::vec4> &Material::GetUniformsOfType<glm::vec4>()
    {
        return m_UniformFloat4s;
    }
    template <typename T>
    std::unordered_map<std::string, T> &Material::GetUniformsOfType()
    {
        return nullptr;
    }

    template <>
    void Material::ResetUniform<float>(std::string name)
    {
        std::unordered_map<std::string, float>& mapRef = GetUniformsOfType<float>(); 
        if (mapRef.contains(name))
            mapRef[name] = m_Shader->GetFloat(name);
    }

    template <>
    void Material::ResetUniform<glm::vec2>(std::string name)
    {
        std::unordered_map<std::string, glm::vec2>& mapRef = GetUniformsOfType<glm::vec2>(); 
        if (mapRef.contains(name))
            mapRef[name] = m_Shader->GetFloat2(name);
    }

    template <>
    void Material::ResetUniform<glm::vec3>(std::string name)
    {
        std::unordered_map<std::string, glm::vec3>& mapRef = GetUniformsOfType<glm::vec3>(); 
        if (mapRef.contains(name))
            mapRef[name] = m_Shader->GetFloat3(name);
    }

    template <>
    void Material::ResetUniform<glm::vec4>(std::string name)
    {
        std::unordered_map<std::string, glm::vec4>& mapRef = GetUniformsOfType<glm::vec4>(); 
        if (mapRef.contains(name))
            mapRef[name] = m_Shader->GetFloat4(name);
    }

    template <>
    void Material::ResetUniform<int>(std::string name)
    {
        std::unordered_map<std::string, int>& mapRef = GetUniformsOfType<int>(); 
        if (mapRef.contains(name))
            mapRef[name] = m_Shader->GetInt(name);
    }

    void Material::UpdateUniforms()
    {
        std::vector<ShaderUniform> uniforms = m_Shader->GetUniformNames();
        for (const auto& uniform : uniforms){
            switch (uniform.Type){
                case UniformType::Float:{
                    if(!m_UniformFloats.contains(uniform.Name)){
                        m_UniformFloats[uniform.Name] = m_Shader->GetFloat(uniform.Name);
                    }
                    break;
                }
                case UniformType::Float2:{
                    if(!m_UniformFloat2s.contains(uniform.Name)){
                        m_UniformFloat2s[uniform.Name] = m_Shader->GetFloat2(uniform.Name);
                    }
                    break;
                }
                case UniformType::Float3:{
                    if(!m_UniformFloat3s.contains(uniform.Name)){
                        m_UniformFloat3s[uniform.Name] = m_Shader->GetFloat3(uniform.Name);
                    }
                    break;                    
                }
                case UniformType::Float4:{
                    if(!m_UniformFloat4s.contains(uniform.Name)){
                        m_UniformFloat4s[uniform.Name] = m_Shader->GetFloat4(uniform.Name);
                    }
                    break;
                }
                case UniformType::Int:{
                    if(!m_UniformInts.contains(uniform.Name)){
                        m_UniformInts[uniform.Name] = m_Shader->GetInt(uniform.Name);
                    }
                    break;
                }
                case UniformType::Mat3x3: break;
                case UniformType::Mat4x4: break;
                default:
                    OIL_CORE_ERROR("Uniform type not implemented on material UpdateUniforms!");
            }
        }
    }

    void Material::UploadUniforms()
    {

        for (const auto &it : m_UniformInts)
        {
            m_Shader->SetInt(it.first, it.second);
        }
        for (const auto& it : m_UniformFloats){
            m_Shader->SetFloat(it.first, it.second);
        }
        for (const auto& it : m_UniformFloat2s){
            m_Shader->SetFloat2(it.first, it.second);
        }
        for (const auto& it : m_UniformFloat3s){
            m_Shader->SetFloat3(it.first, it.second);
        }
        for (const auto& it : m_UniformFloat4s){
            m_Shader->SetFloat4(it.first, it.second);
        }
    }

    void Material::SetShader(const AssetRef<Shader> &shader)
    {
        m_Shader = shader; 
        std::vector<ShaderUniform> uniforms = m_Shader->GetUniformNames();
        for (const auto& uniform : uniforms){
            switch(uniform.Type){
                case UniformType::Int:{
                    if(!m_UniformInts.contains(uniform.Name))  
                        m_UniformInts[uniform.Name] = m_Shader->GetInt(uniform.Name);
                    break;
                }
                case UniformType::Float:{
                    if(!m_UniformFloats.contains(uniform.Name)) 
                        m_UniformFloats[uniform.Name] = m_Shader->GetFloat(uniform.Name);
                    break;
                }
                case UniformType::Float2:{
                    if(!m_UniformFloat2s.contains(uniform.Name)) 
                        m_UniformFloat2s[uniform.Name] = m_Shader->GetFloat2(uniform.Name);
                    break;
                }
                case UniformType::Float3:{
                    if(!m_UniformFloat3s.contains(uniform.Name)) 
                        m_UniformFloat3s[uniform.Name] = m_Shader->GetFloat3(uniform.Name);
                    break;
                }
                case UniformType::Float4:{
                    if(!m_UniformFloat4s.contains(uniform.Name)) 
                        m_UniformFloat4s[uniform.Name] = m_Shader->GetFloat4(uniform.Name);
                    break;
                }
                case UniformType::Mat3x3: break;
                case UniformType::Mat4x4: break;
                default:{
                    OIL_CORE_ERROR("Type not implemented on shader uniforms!");
                    break;
                }
            }
        }
    }

    template <>
    ContentType AssetRef<Material>::GetType()
    {
        return ContentType::Material;
    }

}