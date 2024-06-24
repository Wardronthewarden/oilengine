#pragma once

#include <oil/core/core.h>
#include "oil/Renderer/Shader.h"


namespace oil{

    //Forward declarations
    template<typename T>
    class Asset;

    class Material{
    public:
        Material() = default;
        Material(Asset<Shader> shader);
        Material(const Material& other);
        ~Material();

        Asset<Shader> GetShader() const; 

        template<typename T>
        const T GetUniform(std::string name){
                std::unordered_map<std::string, T>& mapRef = GetUniformsOfType<T>(); 
                T val = mapRef.find(name);
                if (val != mapRef.end())
                    return val;
                OIL_WARN("Uniform {1} does not exist on material.", name);
                return T();
    
        }

        template<typename T>
        std::unordered_map<std::string, T>& GetUniformsOfType();

        template<typename T>
        void SetUniform(std::string name, T value){
                std::unordered_map<std::string, T>& mapRef = GetUniformsOfType<T>(); 
                mapRef[name] = value;
    
        }

        void UpdateUniforms();

    private:
        friend class Serializer;

        void SetShader(const Asset<Shader>& shader); 

    private:
        //Shader
        Asset<Shader> m_Shader;


        //Uniforms
        std::unordered_map<std::string, int> m_UniformInts;
        std::unordered_map<std::string, float> m_UniformFloats;
        std::unordered_map<std::string, glm::vec2> m_UniformFloat2s;
        std::unordered_map<std::string, glm::vec3> m_UniformFloat3s;
        std::unordered_map<std::string, glm::vec4> m_UniformFloat4s;
        std::unordered_map<std::string, glm::mat3> m_UniformMat3s;
        std::unordered_map<std::string, glm::mat4> m_UniformMat4s;

    };


}