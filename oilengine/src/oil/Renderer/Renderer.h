#pragma once

#include "RenderCommand.h"

#include "OrthographicCamera.h"
#include "Shader.h"

namespace oil{

    class Renderer{
    public:
        static void Init();
        static void OnWindowResize(uint32_t width, uint32_t height);

        static void BeginScene(OrthographicCamera& camera);
        static void EndScene();

        static void Submit(const oil::Ref<Shader>& shader, const oil::Ref<VertexArray>& vertexArray, const glm::mat4 transform = glm::mat4(1));

        inline static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
    private:
        struct SceneData{
            glm::mat4 VPMat;
        };

        static SceneData* m_SceneData;
    };
}