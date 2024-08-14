#pragma once

#include <glm/glm.hpp>
#include "VertexArray.h"

namespace oil{

    enum class DepthOperator{
        Always,
        Never,
        Less,
        Greater,
        Equal,
        NotEqual,
        LEqual,
        GEqual
    };

    class RenderAPI{
    public:
        enum class API{
            None = 0, OpenGL = 1
        };
    public:
        virtual void Init() = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void Clear() = 0;

        //switch modes
        virtual void SetDepthWriteEnabled(bool enabled) = 0;
        virtual void SetDepthTestEnabled(bool enabled) = 0;
        virtual void SetFaceCullingEnabled(bool enabled) = 0;
        virtual void SetDepthTestOperator(DepthOperator oper) = 0;


        virtual void DrawIndexed(const oil::Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

        inline static API GetAPI() { return s_API; }

    private:
        static API s_API;
    };

}