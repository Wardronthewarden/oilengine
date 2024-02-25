#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"
#include "SubTexture2D.h"
#include "Mesh.h"
#include "oil/Renderer/Camera.h"
#include "oil/Renderer/EditorCamera.h"
#include "oil/Scene/Component.h"

namespace oil{

    class Renderer3D{
        public:
            static void Init();
            static void ShutDown();
                
            static void BeginScene(const OrthographicCamera& camera);
            static void BeginScene(const EditorCamera& camera);
            static void BeginScene(const Camera& camera, const glm::mat4& transform);
            static void EndScene();
            static void Flush();

            //Sprites

            static void DrawSprite();

            //Meshes

            static void DrawMesh(const glm::mat4& transform, Mesh& mesh, int entityID);
            static void DrawMesh(const glm::mat4& transform, MeshComponent& meshComp, int entityID);

            //Lights

            static void SubmitLight(const glm::mat4& transform, PointLightComponent& light, int entityID);
            static void SubmitLight(const glm::mat4& transform, SpotLightComponent& light, int entityID);
            static void SubmitLight(const glm::mat4& transform, DirecLightComponent& light, int entityID);


            //Shading steps

            

            //Stats
            struct Stats{
                uint32_t DrawCalls = 0;
                uint32_t TriCount = 0;

                uint32_t GetTotalVertexCount() { return TriCount * 4; }
                uint32_t GetTotalIndexCount() { return TriCount * 6; }
                uint32_t GetTotalTriangleCount() { return TriCount * 2; }

            };

            static void ResetStats();
            static Stats GetStats();
        private:
            static void StartNewBatch();
    };
}