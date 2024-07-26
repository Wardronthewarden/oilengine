#include "pch/oilpch.h"
#include "Renderer3D.h"

#include "Shader.h"
#include "Material.h"
#include "VertexArray.h"

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "glm/gtc/matrix_transform.hpp"

#define MAX_TRIANGLES 10000


namespace oil{

    struct Render3DBatch{
        Render3DBatch()
            : VertexCount(0), IndexCount(0)
        {
            VBuffer = CreateRef<DataBuffer<unsigned char>>(MAX_TRIANGLES * 3 * sizeof(BaseVertex));
            IBuffer = CreateRef<DataBuffer<uint32_t>>(MAX_TRIANGLES * 3);
            vptr = VBuffer->Begin();
            iptr = IBuffer->Begin();
        }


        int AppendIndices(const Ref<DataBuffer<unsigned char>> indexBuffer){
            
            uint32_t ic = indexBuffer->GetSize() / sizeof(uint32_t);
            if (ic + IndexCount > (MAX_TRIANGLES *3)){
                return -1;
            }
            std::vector<uint32_t> indices((uint32_t*)indexBuffer->GetData(), (uint32_t*)indexBuffer->GetData() + ic);
            for (uint32_t index : indices) {
                *iptr = index + VertexCount;
                ++iptr;
                ++IndexCount;
            }

            return IndexCount;
        }


        void ResetData(){
            vptr = VBuffer->Begin();
            iptr = IBuffer->Begin();

            VertexCount = 0;
            IndexCount = 0;
        }

        

        uint32_t VertexCount = 0, IndexCount = 0;

        Ref<DataBuffer<unsigned char>> VBuffer;
        Ref<DataBuffer<uint32_t>> IBuffer;

        Render3DBatch& operator++(){
            ++VertexCount;
            return *this;
        }

        Render3DBatch operator++(int){ 
            VertexCount++;
            return *this;
        }

        template<typename T>
        friend Render3DBatch& operator<<(Render3DBatch& lhs, const T& rhs);

    private:
        unsigned char* vptr;
        uint32_t* iptr;
    };

    template<typename T>
    Render3DBatch& operator<<(Render3DBatch& lhs, const T& rhs){

        memcpy(lhs.vptr, &rhs, sizeof(T));
        lhs.vptr += sizeof(T);
        return lhs;
    }

    struct Renderer3DData{
        
        // Setup maximums
        static const uint32_t MaxTris = MAX_TRIANGLES;
        static const uint32_t MaxVertices = MaxTris * 3;
        static const uint32_t MaxIndices = MaxTris * 3;
        static const uint32_t MaxPointLights = 1000;
        static const uint32_t MaxTextureSlots = 32; //TODO: Base on GPU query

        //Mesh Vertex arrays
        Ref<VertexArray> MeshVertexArray;
        Ref<VertexBuffer> MeshVertexBuffer;
        Ref<IndexBuffer> MeshIndexBuffer;

        //temp mesh batch
        Render3DBatch* meshBatch;

        //default materials
        Ref<Material> DefaultMaterial;
        
        //Material batches
        std::unordered_map<Ref<Material>, Ref<Render3DBatch>> LitMeshes; 
        

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
        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 0;

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


        // Setup Buffers
        s_3DRenderData.meshBatch = new Render3DBatch();

        
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
        //create the default material from the default surface shader asset
        s_3DRenderData.DefaultMaterial = CreateRef<Material>(AssetManager::GetAssetReference<Shader>(AssetManager::GetHandleByName("DefaultSurfaceShader")));
        //s_3DRenderData.DefaultMaterial->SetUniform<int*>("u_Textures", samplers);

        
        s_3DRenderData.ShaderLib->Load("Light pass","Assets/Shaders/Lighting.glsl");
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("Light pass"); 
        s_3DRenderData.ActiveShader->Bind();
        //Find a better way
        s_3DRenderData.ActiveShader->SetIntArray("u_Textures", samplers, s_3DRenderData.MaxTextureSlots);

        

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

        ClearBuffers();

        StartNewBatch();
    }
    void Renderer3D::BeginScene(const EditorCamera &camera)
    {
        glm::mat4 VPmat = camera.GetVPMatrix();
        s_3DRenderData.CamPosition = camera.GetPosition();
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("First pass");


        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetMat4("u_VPMat", VPmat);

        
        ClearBuffers();

        

        StartNewBatch();
    }
    void Renderer3D::BeginScene(const Camera &camera, const glm::mat4 &transform)
    {
        glm::mat4 VPmat = camera.GetProjection() * glm::inverse(transform);
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("First pass");

        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetMat4("u_VPMat", VPmat);

        
        ClearBuffers();

        StartNewBatch();
    }

    
    void Renderer3D::ClearBuffers()
    {
        RBuffers.FBuffer->Bind();
        RenderCommand::SetClearColor({1.0f, 0.0f, 1.0f, 1.0f});
        RenderCommand::Clear();
        RBuffers.GBuffer->Bind();
        RenderCommand::Clear();
        RBuffers.GBuffer->ClearAttachment(4, -1.0f);
        RBuffers.FBuffer->ClearAttachment(1, -1);
    }

    Ref<FrameBuffer> Renderer3D::GetFrameBuffer()
    {
        return RBuffers.FBuffer;
    }

    void Renderer3D::StartNewBatch()
    {
        //Reset batch information
        s_3DRenderData.TextureSlotIndex = 0;
        s_3DRenderData.meshBatch->ResetData();

        s_3DRenderData.MeshVertexArray->Bind();
    }



    void Renderer3D::EndScene()
    {
       //Render flow
        RenderLitMeshes(); 

    }

    // Mesh drawing

    //DEPRECATED
    void Renderer3D::DrawMesh(const glm::mat4& transform, Ref<Mesh> mesh, int entityID)
    {
        SubmitMesh(transform, mesh, s_3DRenderData.DefaultMaterial, entityID);
    }
    //DEPRECATED
    void Renderer3D::DrawMesh(const glm::mat4& transform, MeshComponent &meshComp, int entityID)
    {
        SubmitMesh(transform, meshComp.mesh, s_3DRenderData.DefaultMaterial, entityID);
    }

    void Renderer3D::SubmitMesh(const glm::mat4 &transform, Ref<Mesh> mesh, Ref<Material> material, int entityID)
    {
        if(!material) material = s_3DRenderData.DefaultMaterial;
        
        if ((s_3DRenderData.LitMeshes.find(material) == s_3DRenderData.LitMeshes.end())){
            s_3DRenderData.LitMeshes[material] = CreateRef<Render3DBatch>();
        }

        //Indices
        if (s_3DRenderData.LitMeshes[material]->AppendIndices(mesh->GetIndexBuffer()) == -1){
            //Reset if buffer overflows
            RenderBatch(material, s_3DRenderData.LitMeshes[material]);
            s_3DRenderData.LitMeshes[material]->ResetData();
            SubmitMesh(transform, mesh, material, entityID);
        }

        //vertices
        Ref<DataBuffer<unsigned char>> vertexBuffer = mesh->GetVertexBuffer();
        std::vector<BaseVertex> vertices((BaseVertex*)vertexBuffer->GetData(), (BaseVertex*)vertexBuffer->GetData() + vertexBuffer->GetSize() / sizeof(BaseVertex));
        
        Render3DBatch& ref = *s_3DRenderData.LitMeshes[material];
        for (auto vert : vertices) {
            ref << transform * vert.Position;
            ref << vert.Color;
            ref << vert.TexCoord;
            ref << entityID;
            ref++;   
        }
        
    }




    void Renderer3D::RenderLitMeshes()
    {
        for(auto& it : s_3DRenderData.LitMeshes){
            RenderBatch(it.first, it.second);
            it.second->ResetData();
        }
    }

    void Renderer3D::RenderBatch(const Ref<Material> material, const Ref<Render3DBatch> batch)
    {
        //set up Vertex and Index buffers on renderer
        s_3DRenderData.MeshVertexBuffer->SetData(*(batch->VBuffer), batch->VertexCount * sizeof(BaseVertex));
        s_3DRenderData.MeshIndexBuffer->SetData(*(batch->IBuffer), batch->IndexCount);


        //Bind all textures
        for (uint32_t i = 0; i< s_3DRenderData.TextureSlotIndex; ++i){
            s_3DRenderData.TextureSlots[i]->Bind(i);            
        }

        //Bind render buffer and shader
        RBuffers.GBuffer->Bind();
        /* s_3DRenderData.ActiveShader = material->GetShader();
        s_3DRenderData.ActiveShader->Bind(); */

        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("First pass"); 
        s_3DRenderData.ActiveShader->Bind();

        //Send uniforms to shader
        //material->UploadUniforms();

        //Command draw
        RenderCommand::DrawIndexed(s_3DRenderData.MeshVertexArray, batch->IndexCount);
        
        //statistics
        s_3DRenderData.stats.DrawCalls++;
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
        s_3DRenderData.PointLightArray->SetData(s_3DRenderData.PointLightBufferBase, dataSize);

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