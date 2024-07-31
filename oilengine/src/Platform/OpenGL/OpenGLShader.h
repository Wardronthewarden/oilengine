#pragma once

#include "oil/Renderer/Shader.h"
#include <glm/glm.hpp>

//TODO: Remove
typedef unsigned int GLenum;

namespace oil{

    class OpenGLShader : public Shader{
    public:
        OpenGLShader(const std::string& filepath);
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void Recompile(std::string srcPath) override;

        virtual const std::string& GetName() const override { return m_Name; }
        virtual const std::string& GetPath() const override { return m_Path; }
        virtual const ShaderDomain GetShaderDomain() const override { return m_ShaderDomain;}
        virtual const ShaderModel GetShaderModel() const override { return m_ShaderModel;}


        virtual void SetInt(const std::string& name, const int& value) override;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
        virtual void SetFloat(const std::string& name, const float& value) override;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
        virtual void SetMat3(const std::string& name, const glm::mat3& value) override;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

        virtual int GetInt(const std::string& name) override;
        virtual std::vector<int> GetIntArray(const std::string& name) override;
        virtual float GetFloat(const std::string& name) override;
        virtual glm::vec2 GetFloat2(const std::string& name) override;
        virtual glm::vec3 GetFloat3(const std::string& name) override;
        virtual glm::vec4 GetFloat4(const std::string& name) override;
        virtual glm::mat3 GetMat3(const std::string& name) override;
        virtual glm::mat4 GetMat4(const std::string& name) override;

        virtual std::vector<ShaderUniform> GetUniformNames() const override;

        UniformType GlTypeToUniformType(GLenum type) const;


        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

        void UploadUniformInt(const std::string& name, int value);
        void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
    private:
        void Compile(std::unordered_map<GLenum, std::string>& shaderSources);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        std::string ReadFile(const std::string& filepath);
    private:
        uint32_t m_RendererID;
        std::string m_Name;
        std::string m_Path = "";
        ShaderDomain m_ShaderDomain = ShaderDomain::None;
        ShaderModel m_ShaderModel = ShaderModel::None;
    };
}