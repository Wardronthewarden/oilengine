#include "pch/oilpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer2D.h"
#include "Renderer3D.h"

namespace oil{

    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

    void Renderer::Init()
    {
        RenderCommand::Init();
        //Renderer2D::Init();
        Renderer3D::Init();
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
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