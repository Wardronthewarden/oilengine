#include "pch/oilpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace oil{

    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

    void Renderer::Init()
    {
        RenderCommand::Init();
    }

    void Renderer::BeginScene(OrthographicCamera &camera)
    {
        m_SceneData->VPMat = camera.GetVPMatrix();
    }
    void Renderer::EndScene()
    {
    }
    void Renderer::Submit(const oil::Ref<Shader>& shader, const oil::Ref<VertexArray> &vertexArray, const glm::mat4 transform)
    {
        shader->Bind();
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_VPMat", m_SceneData->VPMat);
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }
}