#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"
#include "SubTexture2D.h"

namespace oil{

    static glm::vec2 s_DefaultTexCoords[4] = { 
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}
    };

    class Renderer2D{
    public:
        static void Init();
        static void ShutDown();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();
        static void Flush();

        // Primitives
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size = {1.0f, 1.0f}, const glm::vec4& color = {1.0f, 0.0f, 1.0f, 1.0f}, float rotation = 0.0f);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size = {1.0f, 1.0f}, const glm::vec4& color = {1.0f, 0.0f, 1.0f, 1.0f}, float rotation = 0.0f);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor = {1.0f, 1.0f}, const Ref<SubTexture2D>& subTexture = nullptr, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float rotation = 0.0f);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor = {1.0f, 1.0f}, const Ref<SubTexture2D>& subTexture = nullptr, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float rotation = 0.0f);
    
        
        struct Stats{
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;

            uint32_t GetTotalVertexCount() { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() { return QuadCount * 6; }
            uint32_t GetTotalTriangleCount() { return QuadCount * 2; }

        };

        static void ResetStats();
        static Stats GetStats();
    private:
        static void StartNewBatch();
    };
}