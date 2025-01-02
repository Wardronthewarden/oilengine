#include "pch/oilpch.h"
#include "OpenGLRenderAPI.h"

#include <GLAD/glad.h>
namespace oil{
    void OpenGLRenderAPI::Init()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
    }

    void OpenGLRenderAPI::SetClearColor(const glm::vec4 &color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }
    void OpenGLRenderAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }
    void OpenGLRenderAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    void OpenGLRenderAPI::DrawIndexed(const oil::Ref<VertexArray> &vertexArray, uint32_t indexCount)
    {
        uint32_t count = indexCount ?  indexCount : vertexArray->GetIndexBuffer()->GetCount();

        OILVertenum drawMode = vertexArray->GetDrawMode();

        switch(drawMode){
            case OIL_VERT_POINTS: glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, nullptr);
                break;
            case OIL_VERT_TRIANGLES: glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
                break;
            case OIL_VERT_TRIANGLE_STRIP: glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, nullptr);
                break;
            default:
                OIL_CORE_ASSERT(false, "Invalid draw mode passed to DrawIndexed");
        }

        
    }
    void OpenGLRenderAPI::SetDepthWriteEnabled(bool enabled)
    {
        if(enabled){
            glDepthMask(GL_TRUE);
        }else{
            glDepthMask(GL_FALSE);
        }
    }
    void OpenGLRenderAPI::SetDepthTestEnabled(bool enabled)
    {
        if(enabled){
            glEnable(GL_DEPTH_TEST);
        }else{
            glDisable(GL_DEPTH_TEST);
        }
    }
    void OpenGLRenderAPI::SetFaceCullingEnabled(bool enabled)
    {
        if(enabled){
            glEnable(GL_CULL_FACE);
        }else{
            glDisable(GL_CULL_FACE);
        }
    }
    void OpenGLRenderAPI::SetDepthTestOperator(DepthOperator oper)
    {
        switch(oper){
            case DepthOperator::Always: glDepthFunc(GL_ALWAYS);         break;
            case DepthOperator::Never: glDepthFunc(GL_NEVER);           break;
            case DepthOperator::Less: glDepthFunc(GL_LESS);             break;
            case DepthOperator::Greater: glDepthFunc(GL_GREATER);       break;
            case DepthOperator::Equal: glDepthFunc(GL_EQUAL);           break;
            case DepthOperator::NotEqual: glDepthFunc(GL_NOTEQUAL);     break;
            case DepthOperator::LEqual: glDepthFunc(GL_LEQUAL);         break;
            case DepthOperator::GEqual: glDepthFunc(GL_GEQUAL);         break;
            default: OIL_CORE_WARN("Invalid depth function!");       
        }
    }
}