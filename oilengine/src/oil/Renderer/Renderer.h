#pragma once

#include "RenderCommand.h"

#include "OrthographicCamera.h"
#include "Shader.h"

namespace oil{

    class Renderer{
    public:
        static void BeginScene(OrthographicCamera& camera);
        static void EndScene();

        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

        inline static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
    private:
        struct SceneData{
            glm::mat4 VPMat;
        };

        static SceneData* m_SceneData;
    };
}