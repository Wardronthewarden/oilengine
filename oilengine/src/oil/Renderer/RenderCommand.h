#pragma once

#include "RenderAPI.h"

namespace oil{

    class RenderCommand{
    public:
        inline static void Init(){
            s_RenderAPI->Init();
        }

        inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height){
            s_RenderAPI->SetViewport(x, y, width, height);
        }

        inline static void SetClearColor(const glm::vec4& color){
            s_RenderAPI->SetClearColor(color);
        };
        inline static void Clear(){
            s_RenderAPI->Clear();
        };
        
        inline static void DrawIndexed(const oil::Ref<VertexArray>& vertexArray, uint32_t indexCount = 0){
            s_RenderAPI->DrawIndexed(vertexArray, indexCount);
        };

        inline static void EnableDepthTesting(){
            s_RenderAPI->SetDepthTestEnabled(true);
        };
        
        inline static void DisableDepthTesting(){
            s_RenderAPI->SetDepthTestEnabled(false);
        };

        inline static void EnableDepthWriting(){
            s_RenderAPI->SetDepthWriteEnabled(true);
        };

        inline static void DisableDepthWriting(){
            s_RenderAPI->SetDepthWriteEnabled(false);
        };

        inline static void EnableFaceCulling(){
            s_RenderAPI->SetFaceCullingEnabled(true);
        };

        inline static void DisableFaceCulling(){
            s_RenderAPI->SetFaceCullingEnabled(false);
        };

        inline static void SetDepthTestOperator(DepthOperator oper){
            s_RenderAPI->SetDepthTestOperator(oper);
        };

    private:
        static RenderAPI* s_RenderAPI; 
    };
}