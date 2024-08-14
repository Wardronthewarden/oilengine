#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "SubTexture2D.h"
#include "Mesh.h"
#include "Light.h"
#include "oil/Renderer/Camera.h"
#include "oil/Renderer/EditorCamera.h"
#include "oil/Scene/Component.h"

namespace oil{

    struct Render3DBatch;
    struct RenderBuffers;
    class Material;

    class Renderer3D{
        public:
            static void Init();
            static void ShutDown();
                
            static void BeginScene(const OrthographicCamera& camera);
            static void BeginScene(const EditorCamera& camera);
            static void BeginScene(const Camera& camera, const glm::mat4& transform);
            static void EndScene();

            //Queries
            static uint32_t GetDepthBufferID();
            static uint32_t GetFrameBufferID();
            static uint32_t GetSceneNormalBufferID();

            //Sprites

            static void DrawSprite();

            //Meshes
            static void SubmitMesh(const glm::mat4& transform, Ref<Mesh> mesh, AssetHandle material, int entityID);

            static void DrawMesh(const glm::mat4& transform, Ref<Mesh> mesh, int entityID);
            static void DrawMesh(const glm::mat4& transform, MeshComponent& meshComp, int entityID);

            //Lights

            static void SubmitLight(const glm::mat4& transform, PointLightComponent& light, int entityID);
            static void SubmitLight(const glm::mat4& transform, SpotLightComponent& light, int entityID);
            static void SubmitLight(const glm::mat4& transform, DirecLightComponent& light, int entityID);


            //Lighting
            static void InitLightingInfo();
            static void RenderLighting();
            static void StartLightingPass();

            //Environment
            static void RenderEnvironment();

            //Control steps
            static void ClearBuffers();
            static void RenderLitMeshes();

            //Batching
            static void RenderBatch(const Ref<Material> material, const Ref<Render3DBatch> batch);
            

            //Stats
            struct Stats{
                uint32_t DrawCalls = 0;
                uint32_t TriCount = 0;

                uint32_t GetTotalVertexCount() { return TriCount * 4; }
                uint32_t GetTotalIndexCount( ) { return TriCount * 6; }
                uint32_t GetTotalTriangleCount() { return TriCount * 2; }

            };

            static void ResetStats() { return ; };
            static Stats GetStats();

            static Ref<FrameBuffer> GetFrameBuffer();
            static Ref<FrameBuffer> GetGBuffer();

        private:
            static void StartNewBatch();
            
    };

    struct RenderBuffers{
        Ref<FrameBuffer> FBuffer;
        Ref<FrameBuffer> GBuffer;

        void Init(){
            //Initialize GBuffer
            FrameBufferSpecification fbSpec;
            fbSpec.Attachments = {  FrameBufferTextureFormat::RGBA16F,      //albedo
                                    FrameBufferTextureFormat::RGBA16F,      //position
                                    FrameBufferTextureFormat::RGB16F,       //normal
                                    FrameBufferTextureFormat::RGBA16F,      //texcoord
                                    FrameBufferTextureFormat::R_FLOAT,      //metallic
                                    FrameBufferTextureFormat::R_FLOAT,      //roughness
                                    FrameBufferTextureFormat::R_FLOAT,      //AO
                                    FrameBufferTextureFormat::R_FLOAT,      //entity ID
                                    FrameBufferTextureFormat::Depth  
                                };
            fbSpec.Width = 1280;
            fbSpec.Height = 720;
            GBuffer = FrameBuffer::Create(fbSpec);

            //Initialize FrameBuffer
            fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::R_INT, FrameBufferTextureFormat::Depth };
            fbSpec.Width = 1280;
            fbSpec.Height = 720;
            FBuffer = FrameBuffer::Create(fbSpec);
        }

        void Resize(uint32_t width, uint32_t height){
            FBuffer->Resize(width, height);
            GBuffer->Resize(width, height);
        }
    };
}