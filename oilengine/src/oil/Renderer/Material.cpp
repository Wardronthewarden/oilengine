#include <pch/oilpch.h>
#include "oil/Renderer/Material.h"
#include <oil/Renderer/Shader.h>
#include "oil/storage/Asset.h"

namespace oil{
    Material::Material(Asset<Shader> shader)
        : m_Shader(shader)
    {

    }

    Material::Material(const Material& other){
        m_Shader = other.m_Shader;
        m_UniformInts = other.m_UniformInts;
        m_UniformFloats = other.m_UniformFloats;
        m_UniformFloat2s = other.m_UniformFloat2s;
        m_UniformFloat3s = other.m_UniformFloat3s;
        m_UniformFloat4s = other.m_UniformFloat4s;
        m_UniformMat3s = other.m_UniformMat3s;
        m_UniformMat4s = other.m_UniformMat4s;
    }

    Material::~Material(){}

    Asset<Shader> Material::GetShader() const
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
    template <>
    std::unordered_map<std::string, glm::mat3> &Material::GetUniformsOfType<glm::mat3>()
    {
        return m_UniformMat3s;
    }
    template <>
    std::unordered_map<std::string, glm::mat4> &Material::GetUniformsOfType<glm::mat4>()
    {
        return m_UniformMat4s;
    }
    template <typename T>
    std::unordered_map<std::string, T> &Material::GetUniformsOfType()
    {
        return nullptr;
    }
    
    void Material::UpdateUniforms(){
        for (const auto& it : m_UniformInts){
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
        for (const auto& it : m_UniformMat3s){
            m_Shader->SetMat3(it.first, it.second);
        }
        for (const auto& it : m_UniformMat4s){
            m_Shader->SetMat4(it.first, it.second);
        }
    }

    void Material::SetShader(const Asset<Shader> &shader)
    {
        m_Shader = shader; 
    }

    template <>
    ContentType Asset<Material>::GetType()
    {
        return ContentType::Material;
    }

}