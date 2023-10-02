#include "pch/oilpch.h"
#include "Renderer.h"

namespace oil{

    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        m_SceneData->VPMat = camera.GetVPMatrix();
    }
    void Renderer::EndScene()
    {
    }
    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray> &vertexArray)
    {
        shader->Bind();
        shader->UploadUniformMat4("u_VPMat", m_SceneData->VPMat);

        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }
}