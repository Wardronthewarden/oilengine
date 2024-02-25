#include "pch/oilpch.h"
#include "Renderer3D.h"

#include "Shader.h"
#include "VertexArray.h"

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "glm/gtc/matrix_transform.hpp"

namespace oil{


    struct Renderer3DData{
        static const uint32_t MaxTris = 10000;
        static const uint32_t MaxVertices = MaxTris * 3;
        static const uint32_t MaxIndices = MaxTris * 3;
        static const uint32_t MaxTextureSlots = 32; //TODO: Base on GPU query

        Ref<VertexArray> MeshVertexArray;
        Ref<VertexBuffer> MeshVertexBuffer;
        Ref<IndexBuffer> MeshIndexBuffer;
        Ref<Shader> PrincipledShader;
        Ref<Texture2D> WhiteTexture;

        uint32_t IndexCount = 0;
        uint32_t VertexCount = 0;
        BaseVertex* VertexBufferBase = nullptr;
        BaseVertex* VertexBufferPtr;
        
        uint32_t* IndexBufferBase = nullptr;
        uint32_t* IndexBufferPtr;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1;

        glm::vec4 BaseVertexPositions[4];

        Renderer3D::Stats stats;
    };

    static Renderer3DData s_3DRenderData;
    static size_t BaseVertexCount = 4;
    static glm::vec2 defaultTexCoords[4] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
    };

    void Renderer3D::Init(){

        OIL_INFO("Current working directory is: {0}", std::filesystem::current_path());
        
        //Create vertex array
        s_3DRenderData.MeshVertexArray = VertexArray::Create();

        //Create vertex buffer
        s_3DRenderData.MeshVertexBuffer = VertexBuffer::Create(s_3DRenderData.MaxVertices * sizeof(BaseVertex));
        s_3DRenderData.MeshVertexBuffer->SetLayout(baseLayout);
        s_3DRenderData.MeshVertexArray->AddVertexBuffer(s_3DRenderData.MeshVertexBuffer);


        //Create index buffer
        s_3DRenderData.MeshIndexBuffer = IndexBuffer::Create(s_3DRenderData.MaxIndices);
        s_3DRenderData.MeshVertexArray->SetIndexBuffer(s_3DRenderData.MeshIndexBuffer);


        //Setup White texture
        s_3DRenderData.WhiteTexture = Texture2D::Create(1,1);
        uint32_t whiteTextureData = 0xffffffff;
        s_3DRenderData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        // Setup Buffers
        s_3DRenderData.VertexBufferBase = new BaseVertex[s_3DRenderData.MaxVertices];
        s_3DRenderData.IndexBufferBase = new uint32_t[s_3DRenderData.MaxIndices];
        
        // Setup texture slots
        int32_t samplers[s_3DRenderData.MaxTextureSlots];
        for (uint32_t i = 0; i<s_3DRenderData.MaxTextureSlots; ++i)
            samplers[i] = i;

        // Setup base shader
        s_3DRenderData.PrincipledShader = Shader::Create("Assets/Shaders/Principled.glsl"); 
        s_3DRenderData.PrincipledShader->Bind();
        s_3DRenderData.PrincipledShader->SetIntArray("u_Textures", samplers, s_3DRenderData.MaxTextureSlots);

        //initialize all tex slots to 0
        s_3DRenderData.TextureSlots[0] = s_3DRenderData.WhiteTexture;
    }

    void Renderer3D::ShutDown(){

    }

    void Renderer3D::BeginScene(const OrthographicCamera& camera){
        s_3DRenderData.PrincipledShader->Bind();
        s_3DRenderData.PrincipledShader->SetMat4("u_VPMat", camera.GetVPMatrix());

        StartNewBatch();
    }
    void Renderer3D::BeginScene(const EditorCamera &camera)
    {
        glm::mat4 VPmat = camera.GetVPMatrix();

        s_3DRenderData.PrincipledShader->Bind();
        s_3DRenderData.PrincipledShader->SetMat4("u_VPMat", VPmat);

        StartNewBatch();
    }
    void Renderer3D::BeginScene(const Camera &camera, const glm::mat4 &transform)
    {
        glm::mat4 VPmat = camera.GetProjection() * glm::inverse(transform);

        s_3DRenderData.PrincipledShader->Bind();
        s_3DRenderData.PrincipledShader->SetMat4("u_VPMat", VPmat);

        StartNewBatch();
    }

    void Renderer3D::StartNewBatch(){
        //Reset batch information
        s_3DRenderData.IndexCount = 0;
        s_3DRenderData.VertexCount = 0;
        s_3DRenderData.VertexBufferPtr = s_3DRenderData.VertexBufferBase;
        s_3DRenderData.IndexBufferPtr = s_3DRenderData.IndexBufferBase;

        s_3DRenderData.TextureSlotIndex = 1;

        s_3DRenderData.MeshVertexArray->Bind();
    }

    void Renderer3D::EndScene()
    {
        uint32_t dataSize = (uint8_t *)s_3DRenderData.VertexBufferPtr - (uint8_t *)s_3DRenderData.VertexBufferBase;
        s_3DRenderData.MeshVertexBuffer->SetData(s_3DRenderData.VertexBufferBase, dataSize);
        s_3DRenderData.MeshIndexBuffer->SetData(s_3DRenderData.IndexBufferBase, s_3DRenderData.IndexCount);

        Flush();
    }

    void Renderer3D::Flush()
    {
        for (uint32_t i = 0; i< s_3DRenderData.TextureSlotIndex; ++i){
            s_3DRenderData.TextureSlots[i]->Bind(i);            
        }

        s_3DRenderData.MeshVertexArray->SetIndexBuffer(s_3DRenderData.MeshIndexBuffer);


        RenderCommand::DrawIndexed(s_3DRenderData.MeshVertexArray, s_3DRenderData.IndexCount);
        
        //statistics
        s_3DRenderData.stats.DrawCalls++;
    }
    

    //Mesh drawing
    void Renderer3D::DrawMesh(const glm::mat4& transform, Mesh &mesh, int entityID)
    {
        if (s_3DRenderData.IndexCount >= Renderer3DData::MaxIndices){
            EndScene();
            StartNewBatch();
        }
        
        const float texIndex  = 0.0f;


        std::vector<uint32_t> indices = mesh.GetIndices();

        uint32_t count = s_3DRenderData.VertexCount;

        for (uint32_t index : indices) {
            *s_3DRenderData.IndexBufferPtr = index + count;
            ++s_3DRenderData.IndexBufferPtr;
            ++s_3DRenderData.IndexCount;
        }

        std::vector<BaseVertex> vertices = mesh.GetVertices();

        for (auto vert : vertices) {
            s_3DRenderData.VertexBufferPtr->Position = transform * vert.Position;
            s_3DRenderData.VertexBufferPtr->Color = vert.Color;
            s_3DRenderData.VertexBufferPtr->TexCoord = vert.TexCoord;
            s_3DRenderData.VertexBufferPtr->EntityID = entityID;
            ++s_3DRenderData.VertexBufferPtr;
            ++s_3DRenderData.VertexCount;
        }

        //statistics
        s_3DRenderData.stats.TriCount += (s_3DRenderData.IndexCount - count) / 3;

    }

    void Renderer3D::DrawMesh(const glm::mat4& transform, MeshComponent &meshComp, int entityID)
    {
        DrawMesh(transform, meshComp.mesh, entityID);
    }
}