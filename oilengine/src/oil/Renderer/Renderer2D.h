#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"

namespace oil{

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
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor = {1.0f, 1.0f}, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float rotation = 0.0f);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor = {1.0f, 1.0f}, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float rotation = 0.0f);
    };
}