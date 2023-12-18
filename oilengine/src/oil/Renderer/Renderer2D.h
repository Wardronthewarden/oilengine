#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"
#include "SubTexture2D.h"
#include "oil/Renderer/Camera.h"
#include "oil/Renderer/EditorCamera.h"
#include "oil/Scene/Component.h"

namespace oil{

    class Renderer2D{
    public:
        static void Init();
        static void ShutDown();

        static void BeginScene(const OrthographicCamera& camera);
        static void BeginScene(const EditorCamera& camera);
        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void EndScene();
        static void Flush();


        // Primitives
        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor = {1.0f, 1.0f}, const Ref<SubTexture2D>& subTexture = nullptr, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, int entityID = -1);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size = {1.0f, 1.0f}, const glm::vec4& color = {1.0f, 0.0f, 1.0f, 1.0f}, float rotation = 0.0f);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size = {1.0f, 1.0f}, const glm::vec4& color = {1.0f, 0.0f, 1.0f, 1.0f}, float rotation = 0.0f);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor = {1.0f, 1.0f}, const Ref<SubTexture2D>& subTexture = nullptr, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float rotation = 0.0f);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor = {1.0f, 1.0f}, const Ref<SubTexture2D>& subTexture = nullptr, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float rotation = 0.0f);
    
        static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);
        
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