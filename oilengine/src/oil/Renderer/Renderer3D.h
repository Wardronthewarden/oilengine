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

    namespace utils{
        //OFFLINE RENDERING ONLY
        static void RenderActiveShaderToTexture(Ref<Texture2D> targetTexture, uint32_t miplevel = 0);
        static void RenderActiveShaderToCubemap(Ref<TextureCube> targetTexture, uint32_t miplevel = 0);
        static void SetDebugTexture(Ref<Texture2D> targetTexture);
    }

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
            static uint32_t GetBufferID(std::string bufferName);
            static uint32_t GetDebugTexture();

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
            static void StartLightingPass();
            static void EndLightingPass();


            //Environment
            static Ref<TextureCube> CubemapFromHDRI(Ref<Texture2D> hdri);
            static void CubemapFromHDRI(Ref<Texture2D> hdri, Ref<TextureCube> targetTexture);
            static void GenerateDiffuseIrradiance();
            static void GenerateBRDFIntegrationMap();
            static void PreFilterEnvironmentMap();
            static void RenderEnvironment();
            static void RenderIBL();

            //Control steps
            static void ClearBuffers();
            static void RenderLitMeshes();
            static void RenderUnlitMeshes();

            //Batching
            static void RenderBatch(const Ref<Material> material, const Ref<Render3DBatch> batch);

            //utils
            
            //-----------------------------------------------------------------------------------------
            //we will try to create a new approach, building architectural blocks first

            //define pass
            //static void DefinePass(const RenderPassInfo& info);

            //render to quad
            //static void DrawToQuad(const Ref<Material> material, const Ref<RenderData> renderData, const Ref<FrameBuffer> framebuffer);

            //-----------------------------------------------------------------------------------------

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
        Ref<FrameBuffer> WildcardBuffer;

        void Init(){
            //Initialize GBuffer
            FrameBufferSpecification fbSpec;
            fbSpec.Attachments = {  OIL_TEXTURE_FORMAT_RGBA16F,         //albedo
                                    OIL_TEXTURE_FORMAT_RGBA16F,         //position
                                    OIL_TEXTURE_FORMAT_RGBA16F,         //normal
                                    OIL_TEXTURE_FORMAT_RGBA16F,         //texcoord
                                    OIL_TEXTURE_FORMAT_RED16F,          //metallic
                                    OIL_TEXTURE_FORMAT_RED16F,          //roughness
                                    OIL_TEXTURE_FORMAT_RED16F,          //AO
                                    OIL_TEXTURE_FORMAT_RED16,           //entity ID
                                    OIL_TEXTURE_FORMAT_DEPTH24STENCIL8  
                                };
            fbSpec.Width = 1280;
            fbSpec.Height = 720;
            GBuffer = FrameBuffer::Create(fbSpec);
            
            //Initialize FrameBuffer
            fbSpec.Attachments = { OIL_TEXTURE_FORMAT_RGBA8, OIL_TEXTURE_FORMAT_DEPTH24STENCIL8 };
            FBuffer = FrameBuffer::Create(fbSpec);

            //Initialize wildcard buffer
            fbSpec.Attachments = { OIL_TEXTURE_FORMAT_RGBA8 };
            WildcardBuffer = FrameBuffer::Create(fbSpec);
        }

        void Resize(uint32_t width, uint32_t height){
            FBuffer->Resize(width, height);
            GBuffer->Resize(width, height);
        }
    };
}