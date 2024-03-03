#include "pch/oilpch.h"
#include "Renderer3D.h"

#include "Shader.h"
#include "VertexArray.h"

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "glm/gtc/matrix_transform.hpp"

namespace oil{


    struct Renderer3DData{
        
        // Setup maximums
        static const uint32_t MaxTris = 10000;
        static const uint32_t MaxVertices = MaxTris * 3;
        static const uint32_t MaxIndices = MaxTris * 3;
        static const uint32_t MaxPointLights = 1000;
        static const uint32_t MaxTextureSlots = 32; //TODO: Base on GPU query

        //Mesh Vertex arrays
        Ref<VertexArray> MeshVertexArray;
        Ref<VertexBuffer> MeshVertexBuffer;
        Ref<IndexBuffer> MeshIndexBuffer;

        //Array location pointers
        BaseVertex* VertexBufferBase = nullptr;
        BaseVertex* VertexBufferPtr;
        
        uint32_t* IndexBufferBase = nullptr;
        uint32_t* IndexBufferPtr;
        
        
        //Light Vertex arrays
        Ref<UniformBuffer> PointLightArray;
        
        //Array location pointers
        PointLightInfo* PointLightBufferBase = nullptr;
        PointLightInfo* PointLightBufferPtr;
        uint32_t PointLightBinding = 2;
        
        //Shaders
        Ref<ShaderLibrary> ShaderLib;
        Ref<Shader> ActiveShader;

        //Textures
        Ref<Texture2D> WhiteTexture;
        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1;

        //Screen quad
        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<IndexBuffer> QuadIndexBuffer;

        //Counts
        uint32_t IndexCount = 0;
        uint32_t VertexCount = 0;
        uint32_t PointLightCount = 0;

        //Scene info
        glm::vec3 CamPosition;

        //Statistics
        Renderer3D::Stats stats;
    };


    //Create renderer data storage
    static Renderer3DData s_3DRenderData;
    static RenderBuffers RBuffers;

    static float quadVerticies[] = {
        -1.0f,  1.0f,  0.0f, 1.0f, //Top left
        -1.0f, -1.0f,  0.0f, 0.0f, //Bottom left
         1.0f, -1.0f,  1.0f, 0.0f, //Bottom right
         1.0f,  1.0f,  1.0f, 1.0f  //Top rignt
    };

    static uint32_t quadIndices[] = {
        0, 1, 2, 0, 2, 3
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

        //Create light buffers
        s_3DRenderData.PointLightArray = UniformBuffer::Create(s_3DRenderData.MaxPointLights * sizeof(PointLightInfo));


        //Setup White texture
        s_3DRenderData.WhiteTexture = Texture2D::Create(1,1);
        uint32_t whiteTextureData = 0xffffffff;
        s_3DRenderData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        // Setup Buffers
        s_3DRenderData.VertexBufferBase = new BaseVertex[s_3DRenderData.MaxVertices];
        s_3DRenderData.IndexBufferBase = new uint32_t[s_3DRenderData.MaxIndices];
        s_3DRenderData.PointLightBufferBase = new PointLightInfo[s_3DRenderData.MaxPointLights];
        
        // Setup texture slots
        int32_t samplers[s_3DRenderData.MaxTextureSlots];
        for (uint32_t i = 0; i<s_3DRenderData.MaxTextureSlots; ++i)
            samplers[i] = i;

        // Setup base shader
        s_3DRenderData.ShaderLib = CreateRef<ShaderLibrary>();

        s_3DRenderData.ShaderLib->Load("First pass", "Assets/Shaders/Principled.glsl");
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("First pass"); 
        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetIntArray("u_Textures", samplers, s_3DRenderData.MaxTextureSlots);

        
        s_3DRenderData.ShaderLib->Load("Light pass","Assets/Shaders/Lighting.glsl");
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("Light pass"); 
        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetIntArray("u_Textures", samplers, s_3DRenderData.MaxTextureSlots);


        //initialize tex 0 to white
        s_3DRenderData.TextureSlots[0] = s_3DRenderData.WhiteTexture;
        

        //Setup screen quad
        s_3DRenderData.QuadVertexArray = VertexArray::Create();
        s_3DRenderData.QuadVertexBuffer = VertexBuffer::Create(quadVerticies, sizeof(quadVerticies));
        s_3DRenderData.QuadVertexBuffer->SetLayout({
            {ShaderDataType::Float2, "a_Position"},
            {ShaderDataType::Float2, "a_TexCoord"}});
        s_3DRenderData.QuadVertexArray->AddVertexBuffer(s_3DRenderData.QuadVertexBuffer);
        s_3DRenderData.QuadIndexBuffer = IndexBuffer::Create(quadIndices, sizeof(quadIndices));
        s_3DRenderData.QuadVertexArray->SetIndexBuffer(s_3DRenderData.QuadIndexBuffer);

        RBuffers.Init();
    }

    void Renderer3D::ShutDown(){

    }

    void Renderer3D::BeginScene(const OrthographicCamera& camera){
        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetMat4("u_VPMat", camera.GetVPMatrix());

        RBuffers.GBuffer->Bind();
        
        RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1});
        RenderCommand::Clear();

        StartNewBatch();
    }
    void Renderer3D::BeginScene(const EditorCamera &camera)
    {
        RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 0.0f});
        glm::mat4 VPmat = camera.GetVPMatrix();
        s_3DRenderData.CamPosition = camera.GetPosition();
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("First pass");


        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetMat4("u_VPMat", VPmat);

        RBuffers.GBuffer->Bind();
        RenderCommand::Clear();
        
        StartNewBatch();
    }
    void Renderer3D::BeginScene(const Camera &camera, const glm::mat4 &transform)
    {
        glm::mat4 VPmat = camera.GetProjection() * glm::inverse(transform);
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("First pass");

        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetMat4("u_VPMat", VPmat);

        
        RBuffers.FBuffer->Bind();
        RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 0.0f});
        RenderCommand::Clear();
        RBuffers.GBuffer->Bind();
        RenderCommand::Clear();

        StartNewBatch();
    }

    Ref<FrameBuffer> Renderer3D::GetFrameBuffer()
    {
        return RBuffers.FBuffer;
    }

    void Renderer3D::StartNewBatch()
    {
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

        RBuffers.GBuffer->Bind();
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("First pass");
        s_3DRenderData.ActiveShader->Bind();
        //s_3DRenderData.QuadVertexArray->Bind();
        //RenderCommand::DrawIndexed(s_3DRenderData.QuadVertexArray, 6);



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
    void Renderer3D::SubmitLight(const glm::mat4 &transform, PointLightComponent &light, int entityID)
    {
        PointLightInfo lightInfo = light.light.GetLightInfo();

        s_3DRenderData.PointLightBufferPtr->LightPosition = glm::vec4(transform[3]);
        s_3DRenderData.PointLightBufferPtr->LightColor = lightInfo.LightColor;
        s_3DRenderData.PointLightBufferPtr->LightIntensity = lightInfo.LightIntensity;
        s_3DRenderData.PointLightBufferPtr->EntityID = entityID;
        ++s_3DRenderData.PointLightBufferPtr;
        ++s_3DRenderData.PointLightCount;
    }

        void Renderer3D::StartLightingPass(){
        
    }

    void Renderer3D::InitLightingInfo()
    {
        s_3DRenderData.PointLightCount = 0;
        s_3DRenderData.PointLightBufferPtr = s_3DRenderData.PointLightBufferBase;

        s_3DRenderData.PointLightArray->Bind();
    }

    void Renderer3D::RenderLighting()
    {
        uint32_t dataSize = (uint8_t *)s_3DRenderData.PointLightBufferPtr - (uint8_t *)s_3DRenderData.PointLightBufferBase;
        s_3DRenderData.PointLightArray->SetData(s_3DRenderData.VertexBufferBase, dataSize);

        s_3DRenderData.QuadVertexArray->Bind();

        RBuffers.FBuffer->Bind();
        RenderCommand::Clear();
        RBuffers.GBuffer->BindColorAttachments();

        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("Light pass");
        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetFloat3("u_CamPos", s_3DRenderData.CamPosition);
        RenderCommand::DrawIndexed(s_3DRenderData.QuadVertexArray, 6);

        RBuffers.GBuffer->UnbindColorAttachments();
    }
}