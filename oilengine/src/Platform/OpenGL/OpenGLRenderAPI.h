#pragma once

#include "oil/Renderer/RenderAPI.h"

namespace oil{
    class OpenGLRenderAPI : public RenderAPI{
    public:
        virtual void Init() override;

        virtual void SetClearColor(const glm::vec4& color) override;
        virtual void Clear() override;

        virtual void DrawIndexed(const oil::Ref<VertexArray>& vertexArray) override;

    };
}