#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace oil{

    enum class ShaderDomain{
        None = 0,
        Surface,
        PostProcess,
        Engine
    };

    enum class ShaderModel{
        None = 0,
        Lit,
        Unlit
    };

    enum class UniformType{
        None = 0,
        Float,
        Float2, Float3, Float4,
        Int,
        Mat3x3, Mat4x4,

        Texture2D
    };

    struct ShaderUniform{
        UniformType Type;
        std::string Name;
    };

    class Shader{
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void Recompile(std::string srcPath) = 0;

        virtual void SetInt(const std::string& name, const int& value) = 0;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
        virtual void SetFloat(const std::string& name, const float& value) = 0;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
        virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

        virtual int GetInt(const std::string& name) = 0;
        virtual std::vector<int> GetIntArray(const std::string& name) = 0;
        virtual float GetFloat(const std::string& name) = 0;
        virtual glm::vec2 GetFloat2(const std::string& name) = 0;
        virtual glm::vec3 GetFloat3(const std::string& name) = 0;
        virtual glm::vec4 GetFloat4(const std::string& name) = 0;
        virtual glm::mat3 GetMat3(const std::string& name) = 0;
        virtual glm::mat4 GetMat4(const std::string& name) = 0;
        
        virtual std::vector<ShaderUniform> GetUniformNames() const = 0;

        virtual const std::string& GetName() const = 0;
        virtual const std::string& GetPath() const = 0;
        virtual const ShaderDomain GetShaderDomain() const = 0;
        virtual const ShaderModel GetShaderModel() const = 0;

        static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        static Ref<Shader> Create(const std::string& filepath);
    private:
        uint32_t m_RendererID;
    };

    class ShaderLibrary{
    public:
        void Add(const std::string& name, const Ref<Shader>& shader);
        void Add(const Ref<Shader>& shader);
        Ref<Shader> Load(const std::string& filepath);
        Ref<Shader> Load(const std::string& name, const std::string& filepath);

        bool Exists(const std::string& name) const;

        Ref<Shader> Get(const std::string& name);
    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    };

}