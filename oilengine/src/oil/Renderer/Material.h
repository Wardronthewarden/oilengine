#pragma once

#include <oil/core/core.h>
#include "oil/storage/Asset.h"
#include "oil/Renderer/Shader.h"


namespace oil{

    class Material{
    public:
        Material() = default;
        Material(AssetRef<Shader> shader);
        Material(const Material& other);
        ~Material();

        AssetRef<Shader> GetShader() const; 

        template<typename T>
        const T GetUniform(std::string name){
                std::unordered_map<std::string, T>& mapRef = GetUniformsOfType<T>(); 
                if (mapRef.contains(name))
                    return mapRef[name];
                OIL_WARN("Uniform {0} does not exist on material.", name);
                return T();
    
        }

        template<typename T>
        std::unordered_map<std::string, T>& GetUniformsOfType();

        template<typename T>
        void SetUniform(std::string name, T value){
                std::unordered_map<std::string, T>& mapRef = GetUniformsOfType<T>(); 
                mapRef[name] = value;
    
        }
        
        template<typename T>
        void RenameUniform(std::string target, std::string newName){
            std::unordered_map<std::string, T>& mapRef = GetUniformsOfType<T>();
            mapRef[newName] = mapRef[target];
            mapRef.erase(target);
        }

        template<typename T>
        void RemoveUniform(std::string name){
            std::unordered_map<std::string, T>& mapRef = GetUniformsOfType<T>(); 
            if (mapRef.contains(name))
                mapRef.erase(name);
        }

        template<typename T>
        void ResetUniform(std::string name);

        void UpdateUniforms();

        void UploadUniforms();

    private:
        friend class Serializer;

        void SetShader(const AssetRef<Shader>& shader); 

    private:
        //Shader
        AssetRef<Shader> m_Shader;


        //Uniforms
        std::unordered_map<std::string, int> m_UniformInts;
        std::unordered_map<std::string, float> m_UniformFloats;
        std::unordered_map<std::string, glm::vec2> m_UniformFloat2s;
        std::unordered_map<std::string, glm::vec3> m_UniformFloat3s;
        std::unordered_map<std::string, glm::vec4> m_UniformFloat4s;

    };


}