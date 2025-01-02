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

        //Primitives
        Ref<Mesh> Cube;
        Ref<Mesh> Plane;

        //temp mesh batch
        Render3DBatch* meshBatch;

        //default materials
        AssetHandle DefaultMaterial;
        
        //Material batches
        std::unordered_map<AssetHandle, Ref<Render3DBatch>> LitMeshes; 
        

        //Light Vertex arrays
        Ref<UniformBuffer> PointLightArray;
        
        //Array location pointers
        PointLightInfo* PointLightBufferBase = nullptr;
        PointLightInfo* PointLightBufferPtr;
        uint32_t PointLightBinding = 2;
        
        //Shaders
        Ref<ShaderLibrary> ShaderLib;
        Ref<Shader> ActiveShader;

        //Engine Textures
        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotsUsed = 0;
        Ref<Texture2D> DebugTexture; 
        Ref<Texture2D> DefaultTexture; 
        Ref<Texture2D> BRDFIntegrationMap; 


        //Render targets
        std::unordered_map<std::string, Ref<Texture2D>> RenderTargets;

        //Environment map
        Ref<Texture2D> DefaultHDRI;
        Ref<TextureCube> SkyBoxTexture;
        Ref<TextureCube> DiffuseIrradianceMap;
        Ref<TextureCube> PreFilteredEnvironmentMap;

        //Unit cube
        Ref<VertexArray> CubeVertexArray;
        Ref<VertexBuffer> CubeVertexBuffer;
        Ref<IndexBuffer> CubeIndexBuffer;
        
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

        //Global uniforms
        glm::mat4 u_VPmat;
        glm::mat4 u_ViewMatrix;
        glm::mat4 u_ProjMatrix;

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

        RenderCommand::SetClearColor({1.0, 0.0, 1.0, 1.0});
        
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
        
        //Find a better way

        s_3DRenderData.ShaderLib = CreateRef<ShaderLibrary>();
        s_3DRenderData.ShaderLib->Load("PBR","Internal/Assets/src/Shaders/PBR.glsl");
        //Change this
        s_3DRenderData.ShaderLib->Add("SkyBox", AssetManager::GetAsset<Shader>(AssetManager::GetHandleByName("SkyBoxShader")));
        s_3DRenderData.ShaderLib->Add("SkyLight", AssetManager::GetAsset<Shader>(AssetManager::GetHandleByName("SkyLightShader")));
        s_3DRenderData.ShaderLib->Add("LightApply", AssetManager::GetAsset<Shader>(AssetManager::GetHandleByName("LightApplication")));
        s_3DRenderData.ShaderLib->Add("CubeProjection", AssetManager::GetAsset<Shader>(AssetManager::GetHandleByName("CubeProjection")));
        s_3DRenderData.ShaderLib->Add("CubemapConvolution", AssetManager::GetAsset<Shader>(AssetManager::GetHandleByName("CubemapConvolution")));
        s_3DRenderData.ShaderLib->Add("TestShader", AssetManager::GetAsset<Shader>(AssetManager::GetHandleByName("TestPassthroughShader")));
        s_3DRenderData.ShaderLib->Add("BRDFIntegration", AssetManager::GetAsset<Shader>(AssetManager::GetHandleByName("BRDFIntegration")));
        s_3DRenderData.ShaderLib->Add("TexDisplayShader", AssetManager::GetAsset<Shader>(AssetManager::GetHandleByName("TexDisplayShader")));


        //Setup screen quad
        s_3DRenderData.QuadVertexArray = VertexArray::Create();
        s_3DRenderData.QuadVertexBuffer = VertexBuffer::Create(quadVerticies, sizeof(quadVerticies));
        s_3DRenderData.QuadVertexBuffer->SetLayout({
            {ShaderDataType::Float2, "a_Position"},
            {ShaderDataType::Float2, "a_TexCoord"}});
        s_3DRenderData.QuadVertexArray->AddVertexBuffer(s_3DRenderData.QuadVertexBuffer);
        s_3DRenderData.QuadIndexBuffer = IndexBuffer::Create(quadIndices, sizeof(quadIndices));
        s_3DRenderData.QuadVertexArray->SetIndexBuffer(s_3DRenderData.QuadIndexBuffer);

        //Setup primitives
        s_3DRenderData.Cube = Mesh::CreateCube();
        s_3DRenderData.Plane = Mesh::CreatePlane();

        //Setup unit cube
        s_3DRenderData.CubeVertexArray = VertexArray::Create();
        s_3DRenderData.CubeVertexBuffer = VertexBuffer::Create(s_3DRenderData.Cube->GetVertexBuffer()->As<float>(), s_3DRenderData.Cube->GetVertexBuffer()->GetSize());
        s_3DRenderData.CubeVertexBuffer->SetLayout(baseLayout);
        s_3DRenderData.CubeVertexArray->AddVertexBuffer(s_3DRenderData.CubeVertexBuffer);
        s_3DRenderData.CubeIndexBuffer = IndexBuffer::Create(s_3DRenderData.Cube->GetIndexBuffer()->As<uint32_t>(),  s_3DRenderData.Cube->GetIndexBuffer()->GetSize());
        s_3DRenderData.CubeVertexArray->SetIndexBuffer(s_3DRenderData.CubeIndexBuffer);


        //Set up render buffers
        RBuffers.Init();

        TextureSettings FBSettings{};
        FBSettings.StorageType = OIL_TEXTURE_STORAGE_MUTABLE | OIL_TEXTURE_2D;
        FBSettings.Width = 1280;
        FBSettings.Height = 720;

        TextureParams FBParams{};

        FBSettings.TextureFormat = OIL_TEXTURE_FORMAT_RGBA8;
        s_3DRenderData.RenderTargets["LitScene"] = Texture2D::Create(FBSettings, FBParams);
        RBuffers.FBuffer->SetColorAttachment(s_3DRenderData.RenderTargets["LitScene"], 0);

        
        FBSettings.TextureFormat = OIL_TEXTURE_FORMAT_DEPTH24STENCIL8;
        s_3DRenderData.RenderTargets["SceneDepth"] = Texture2D::Create(FBSettings, FBParams);
        RBuffers.GBuffer->SetDepthAttachment(s_3DRenderData.RenderTargets["SceneDepth"]);
        RBuffers.FBuffer->SetDepthAttachment(s_3DRenderData.RenderTargets["SceneDepth"]);


        FBSettings.TextureFormat = OIL_TEXTURE_FORMAT_RGBA16F;
        s_3DRenderData.RenderTargets["UnlitScene"] = Texture2D::Create(FBSettings, FBParams);
        RBuffers.GBuffer->SetColorAttachment(s_3DRenderData.RenderTargets["UnlitScene"], 0);

        s_3DRenderData.RenderTargets["WorldPosition"] = Texture2D::Create(FBSettings, FBParams);
        RBuffers.GBuffer->SetColorAttachment(s_3DRenderData.RenderTargets["WorldPosition"], 1);

        s_3DRenderData.RenderTargets["WorldNormal"] = Texture2D::Create(FBSettings, FBParams);
        RBuffers.GBuffer->SetColorAttachment(s_3DRenderData.RenderTargets["WorldNormal"], 2);
        
        s_3DRenderData.RenderTargets["Texcoords"] = Texture2D::Create(FBSettings, FBParams);
        RBuffers.GBuffer->SetColorAttachment(s_3DRenderData.RenderTargets["Texcoords"], 3);
        
        FBSettings.TextureFormat = OIL_TEXTURE_FORMAT_RED16F;
        s_3DRenderData.RenderTargets["Metallic"] = Texture2D::Create(FBSettings, FBParams);
        RBuffers.GBuffer->SetColorAttachment(s_3DRenderData.RenderTargets["Metallic"], 4);
        
        s_3DRenderData.RenderTargets["Roughness"] = Texture2D::Create(FBSettings, FBParams);
        RBuffers.GBuffer->SetColorAttachment(s_3DRenderData.RenderTargets["Roughness"], 5);

        //Set up lighting info
        InitLightingInfo();

        //Default textures
        s_3DRenderData.DefaultTexture = AssetManager::GetAsset<Texture2D>(AssetManager::GetHandleByName("Checkerboard"));
        s_3DRenderData.DefaultHDRI = AssetManager::GetAsset<Texture2D>(AssetManager::GetHandleByName("default_hdri"));

        s_3DRenderData.SkyBoxTexture = CubemapFromHDRI(s_3DRenderData.DefaultHDRI);
        s_3DRenderData.SkyBoxTexture->GenerateMipmaps();
        s_3DRenderData.SkyBoxTexture->SetMinFilter(OIL_TEXTURE_FILTER_LINEAR | OIL_TEXTURE_MIPFILTER_LINEAR);
        GenerateDiffuseIrradiance();

        PreFilterEnvironmentMap();

        GenerateBRDFIntegrationMap();

        FBSettings.TextureFormat = OIL_TEXTURE_FORMAT_RGBA8;

        s_3DRenderData.DebugTexture = Texture2D::Create(FBSettings, FBParams);

        utils::SetDebugTexture(s_3DRenderData.BRDFIntegrationMap);

        //Default material
        s_3DRenderData.DefaultMaterial = AssetManager::GetHandleByName("DefaultMaterial");
    }

    void Renderer3D::ShutDown(){

    }

    void Renderer3D::BeginScene(const OrthographicCamera& camera){
        s_3DRenderData.u_VPmat = camera.GetVPMatrix();
        s_3DRenderData.CamPosition = camera.GetPosition();

        ClearBuffers();
        s_3DRenderData.TextureSlotsUsed = 0;


        StartNewBatch();
    }
    void Renderer3D::BeginScene(const EditorCamera &camera)
    {
        //set up global uniforms
        s_3DRenderData.u_VPmat = camera.GetVPMatrix();
        s_3DRenderData.u_ProjMatrix = camera.GetProjMatrix();
        s_3DRenderData.u_ViewMatrix = camera.GetViewMatrix();
        s_3DRenderData.CamPosition = camera.GetPosition();
        
        ClearBuffers();
        s_3DRenderData.TextureSlotsUsed = 0;           

        
        StartNewBatch();
    }
    void Renderer3D::BeginScene(const Camera &camera, const glm::mat4 &transform)
    {
        //set up global uniforms
        s_3DRenderData.u_VPmat = camera.GetProjection() * glm::inverse(transform);
        
        ClearBuffers();
        s_3DRenderData.TextureSlotsUsed = 0;
        //Bind all engine textures
         


        StartNewBatch();
    }

    
    void Renderer3D::ClearBuffers()
    {
        RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});

        RBuffers.FBuffer->Bind();
        RenderCommand::Clear();
        RBuffers.GBuffer->Bind();
        RenderCommand::Clear();
        RBuffers.GBuffer->ClearAttachment(7, -1);
        RenderCommand::SetClearColor({1.0f, 0.0f, 1.0f, 1.0f});

    }

    Ref<FrameBuffer> Renderer3D::GetFrameBuffer()
    {
        return RBuffers.FBuffer;
    }

    Ref<FrameBuffer> Renderer3D::GetGBuffer()
    {
        return RBuffers.GBuffer;
    }

    void Renderer3D::StartNewBatch()
    {
        //Reset batch information
        s_3DRenderData.meshBatch->ResetData();

        s_3DRenderData.MeshVertexArray->Bind();
    }



    void Renderer3D::EndScene()
    {
        //Render flow
        RenderLitMeshes(); 
    }

    uint32_t Renderer3D::GetDepthBufferID()
    {
        return s_3DRenderData.RenderTargets["SceneDepth"]->GetRendererID();
    }

    uint32_t Renderer3D::GetFrameBufferID()
    {
        return RBuffers.FBuffer->GetColorAttachmentRendererID(0);
    }

    uint32_t Renderer3D::GetBufferID(std::string bufferName)
    {
        return s_3DRenderData.RenderTargets[bufferName]->GetRendererID();
    }

    uint32_t Renderer3D::GetDebugTexture()
    {  
        return s_3DRenderData.DebugTexture->GetRendererID();
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

    void Renderer3D::SubmitMesh(const glm::mat4 &transform, Ref<Mesh> mesh, AssetHandle material, int entityID)
    {
        if(!material) material = s_3DRenderData.DefaultMaterial;
        
        if ((s_3DRenderData.LitMeshes.find(material) == s_3DRenderData.LitMeshes.end())){
            s_3DRenderData.LitMeshes[material] = CreateRef<Render3DBatch>();
        }

        //Indices
        if (s_3DRenderData.LitMeshes[material]->AppendIndices(mesh->GetIndexBuffer()) == -1){
            //Reset if buffer overflows
            RenderBatch(AssetManager::GetAsset<Material>(material), s_3DRenderData.LitMeshes[material]);
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
        RenderCommand::EnableFaceCulling();
        RenderCommand::SetDepthTestOperator(DepthOperator::Less);
        for(auto& it : s_3DRenderData.LitMeshes){
            RenderBatch(AssetManager::GetAsset<Material>(it.first), it.second);
            it.second->ResetData();
        }

    }

    void Renderer3D::RenderUnlitMeshes()
    {
        RenderCommand::EnableFaceCulling();
        RenderCommand::SetDepthTestOperator(DepthOperator::Less);

    }

    void Renderer3D::RenderBatch(const Ref<Material> material, const Ref<Render3DBatch> batch)
    {
        //set up Vertex and Index buffers on renderer
        s_3DRenderData.MeshVertexBuffer->SetData(*(batch->VBuffer), batch->VertexCount * sizeof(BaseVertex));
        s_3DRenderData.MeshIndexBuffer->SetData(*(batch->IBuffer), batch->IndexCount);


        //Bind render buffer and shader
        RBuffers.GBuffer->Bind();
        s_3DRenderData.ActiveShader = material->GetShader();
        s_3DRenderData.ActiveShader->Bind();

        s_3DRenderData.ActiveShader->SetMat4("u_VPMat", s_3DRenderData.u_VPmat);

        //Send uniforms to shader
        material->UploadUniforms();
        material->UploadTextures(s_3DRenderData.TextureSlotsUsed);

        //Command draw
        if(batch->IndexCount){
            RenderCommand::DrawIndexed(s_3DRenderData.MeshVertexArray, batch->IndexCount);
        }
        
        //statistics
        s_3DRenderData.stats.DrawCalls++;
    }


    void Renderer3D::StartLightingPass(){
        InitLightingInfo();
        RenderCommand::DisableDepthWriting();

        s_3DRenderData.QuadVertexArray->Bind();
        RBuffers.FBuffer->Bind();
        RBuffers.GBuffer->BindColorAttachments();
        RBuffers.GBuffer->BindDepthAttachment();


        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("PBR");
        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetFloat3("u_CamPos", s_3DRenderData.CamPosition);
    }
    
    void Renderer3D::SubmitLight(const glm::mat4 &transform, PointLightComponent &light, int entityID)
    {
        PointLightInfo lightInfo = light.light.GetLightInfo();

        s_3DRenderData.ActiveShader->SetFloat3("u_LightColor", lightInfo.LightColor);
        s_3DRenderData.ActiveShader->SetFloat("u_LightIntensity", lightInfo.LightIntensity);
        s_3DRenderData.ActiveShader->SetFloat3("u_LightPosition", glm::vec3(transform[3]));

        RenderCommand::DrawIndexed(s_3DRenderData.QuadVertexArray, 6);
        
    }

    Ref<TextureCube> Renderer3D::CubemapFromHDRI(Ref<Texture2D> hdri)
    {
        TextureSettings settings{};

        settings.StorageType = OIL_TEXTURE_CUBE | OIL_TEXTURE_STORAGE_MUTABLE;
        settings.Width = 1024;
        settings.Height = 1024;
        settings.TextureFormat = OIL_TEXTURE_FORMAT_RGB8;

        TextureParams params{};
        Ref<TextureCube> targetCube;
        targetCube = TextureCube::Create(settings, params); 

        CubemapFromHDRI(hdri, targetCube);

        return targetCube;
    }

    void Renderer3D::CubemapFromHDRI(Ref<Texture2D> hdri, Ref<TextureCube> targetTexture)
    {
        //bind shader and upload data
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("CubeProjection");
        s_3DRenderData.ActiveShader->Bind();
        hdri->Bind();
        s_3DRenderData.ActiveShader->SetInt("u_EquirectangularMap", 0);

        utils::RenderActiveShaderToCubemap(targetTexture);
    }

    void Renderer3D::GenerateDiffuseIrradiance()
    {
        TextureSettings settings{};

        settings.StorageType = OIL_TEXTURE_CUBE | OIL_TEXTURE_STORAGE_MUTABLE;
        settings.Width = 32;
        settings.Height = 32;
        settings.TextureFormat = s_3DRenderData.SkyBoxTexture->GetFormat();

        TextureParams params{};
        params.MinFilter = OIL_TEXTURE_FILTER_LINEAR;
        params.MagFilter = OIL_TEXTURE_FILTER_LINEAR;

        s_3DRenderData.DiffuseIrradianceMap = TextureCube::Create(settings, params);

        //bind shader and upload data
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("CubemapConvolution");
        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.SkyBoxTexture->Bind();
        s_3DRenderData.ActiveShader->SetInt("u_EnvironmentMap", 0);

        utils::RenderActiveShaderToCubemap(s_3DRenderData.DiffuseIrradianceMap);
    }

    void Renderer3D::GenerateBRDFIntegrationMap()
    {
        TextureSettings settings{};

        settings.StorageType = OIL_TEXTURE_2D | OIL_TEXTURE_STORAGE_MUTABLE;
        settings.Width = 512;
        settings.Height = 512;
        settings.TextureFormat = OIL_TEXTURE_FORMAT_RG16F;

        TextureParams params{};
        params.MinFilter = OIL_TEXTURE_FILTER_LINEAR;
        params.MagFilter = OIL_TEXTURE_FILTER_LINEAR;

        s_3DRenderData.BRDFIntegrationMap = Texture2D::Create(settings, params);
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("BRDFIntegration");

        utils::RenderActiveShaderToTexture(s_3DRenderData.BRDFIntegrationMap);
    }

    void Renderer3D::PreFilterEnvironmentMap()
    {
        TextureSettings settings{};

        settings.StorageType = s_3DRenderData.SkyBoxTexture->GetStorageType();
        settings.Width = s_3DRenderData.SkyBoxTexture->GetWidth();
        settings.Height = s_3DRenderData.SkyBoxTexture->GetHeight();
        settings.TextureFormat = s_3DRenderData.SkyBoxTexture->GetFormat();
        settings.MipLevels = 5;

        TextureParams params{};
        params.MinFilter = OIL_TEXTURE_FILTER_LINEAR | OIL_TEXTURE_MIPFILTER_LINEAR;
        params.MagFilter = OIL_TEXTURE_FILTER_LINEAR;

        s_3DRenderData.PreFilteredEnvironmentMap = TextureCube::Create(settings, params);
        s_3DRenderData.SkyBoxTexture->Bind();

        s_3DRenderData.ActiveShader->SetFloat("u_EnvironmentMap", 0);
        s_3DRenderData.ActiveShader->SetFloat("u_EnvironmentResolution", s_3DRenderData.SkyBoxTexture->GetWidth());
        uint32_t maxMipLevels = s_3DRenderData.PreFilteredEnvironmentMap->GetMipLevels();
        float roughness;
        for (uint32_t mip = 0; mip < maxMipLevels; ++mip){
            roughness = (float)mip / (float)(maxMipLevels -1);
            s_3DRenderData.ActiveShader->SetFloat("u_Roughness", roughness);
            utils::RenderActiveShaderToCubemap(s_3DRenderData.PreFilteredEnvironmentMap, mip);
        }
    }

    void Renderer3D::RenderEnvironment()
    {
        glm::mat4 vpmat = s_3DRenderData.u_ProjMatrix * glm::mat4(glm::mat3(s_3DRenderData.u_ViewMatrix));

        //Display sky box
        RenderCommand::DisableDepthWriting();
        RenderCommand::DisableFaceCulling();
        RenderCommand::SetDepthTestOperator(DepthOperator::Equal);
        RBuffers.FBuffer->Bind();

        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("SkyBox");
        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetMat4("u_VPMat", vpmat);
        s_3DRenderData.SkyBoxTexture->Bind(0);
        s_3DRenderData.ActiveShader->SetInt("u_SkyBoxTexture", 0);        

        s_3DRenderData.CubeVertexArray->Bind();
        RenderCommand::DrawIndexed(s_3DRenderData.CubeVertexArray, 36);

        //Cleanup
        RenderCommand::EnableDepthWriting();
        RenderCommand::EnableFaceCulling();
    }

    void Renderer3D::RenderIBL()
    {
        RenderCommand::DisableDepthTesting();
        RenderCommand::SetDepthTestOperator(DepthOperator::NotEqual);
        RBuffers.FBuffer->Bind();
        glm::mat4 vpmat = s_3DRenderData.u_ProjMatrix * glm::mat4(glm::mat3(s_3DRenderData.u_ViewMatrix));
        
        //Apply environment lighting
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("SkyLight");
        s_3DRenderData.ActiveShader->Bind();
        s_3DRenderData.ActiveShader->SetFloat3("u_CamPos", s_3DRenderData.CamPosition);
        s_3DRenderData.PreFilteredEnvironmentMap->Bind(10);
        s_3DRenderData.ActiveShader->SetInt("u_SpecularIrradiance", 10);
        s_3DRenderData.DiffuseIrradianceMap->Bind(11);
        s_3DRenderData.ActiveShader->SetInt("u_DiffuseIrradiance", 11); 
        s_3DRenderData.BRDFIntegrationMap->Bind(12);
        s_3DRenderData.ActiveShader->SetInt("u_BRDFlut", 12); 

        
        s_3DRenderData.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_3DRenderData.QuadVertexArray, 6);
        RenderCommand::EnableDepthTesting();

    }

    void Renderer3D::InitLightingInfo()
    {
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("PBR");
        s_3DRenderData.ActiveShader->Bind();

        int i = 0;
        s_3DRenderData.ActiveShader->SetInt("u_Albedo", i++);
        s_3DRenderData.ActiveShader->SetInt("u_Position", i++);
        s_3DRenderData.ActiveShader->SetInt("u_Normal", i++);
        s_3DRenderData.ActiveShader->SetInt("u_Texcoord", i++);
        s_3DRenderData.ActiveShader->SetInt("u_Metallic", i++);
        s_3DRenderData.ActiveShader->SetInt("u_Roughness", i++);
        s_3DRenderData.ActiveShader->SetInt("u_AO", i++);
        s_3DRenderData.ActiveShader->SetInt("u_ID", i++);
        
        s_3DRenderData.ActiveShader->SetInt("u_SceneDepth", i++);
        
        s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("SkyLight");
        s_3DRenderData.ActiveShader->Bind();

        i = 0;
        s_3DRenderData.ActiveShader->SetInt("u_Albedo", i++);
        s_3DRenderData.ActiveShader->SetInt("u_Position", i++);
        s_3DRenderData.ActiveShader->SetInt("u_Normal", i++);
        s_3DRenderData.ActiveShader->SetInt("u_Texcoord", i++);
        s_3DRenderData.ActiveShader->SetInt("u_Metallic", i++);
        s_3DRenderData.ActiveShader->SetInt("u_Roughness", i++);
        s_3DRenderData.ActiveShader->SetInt("u_AO", i++);
        s_3DRenderData.ActiveShader->SetInt("u_ID", i++);
        
        s_3DRenderData.ActiveShader->SetInt("u_SceneDepth", i++);

    }

    void Renderer3D::EndLightingPass()
    {
        RenderIBL();
        RenderEnvironment();
        //RenderUnlitMeshes();
    }
}

void oil::utils::RenderActiveShaderToTexture(Ref<Texture2D> targetTexture, uint32_t miplevel)
{
    RenderCommand::DisableDepthTesting();
    
    RBuffers.WildcardBuffer->WrapTargetTexture(targetTexture);
    RBuffers.WildcardBuffer->Bind();
    RBuffers.WildcardBuffer->SetAttachmentTargetLayer(0, 0, miplevel);

    s_3DRenderData.ActiveShader->Bind();
    s_3DRenderData.QuadVertexArray->Bind();
    RenderCommand::Clear();

    RenderCommand::DrawIndexed(s_3DRenderData.QuadVertexArray, 6);

    RenderCommand::EnableDepthTesting();
}

void oil::utils::RenderActiveShaderToCubemap(Ref<TextureCube> targetTexture, uint32_t miplevel)
{
    RenderCommand::DisableDepthTesting();
    RBuffers.WildcardBuffer->WrapTargetTexture(targetTexture, miplevel);
    RBuffers.WildcardBuffer->Bind();

    //projection and views
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    s_3DRenderData.ActiveShader->Bind();
    s_3DRenderData.ActiveShader->SetMat4("u_Projection", captureProjection);

    s_3DRenderData.CubeVertexArray->Bind();
        for (int i = 0; i < 6; ++i){
            s_3DRenderData.ActiveShader->SetMat4("u_View", captureViews[i]);
            RBuffers.WildcardBuffer->SetAttachmentTargetLayer(0, i, miplevel);

            RenderCommand::Clear();

            RenderCommand::DrawIndexed(s_3DRenderData.CubeVertexArray, 36);
    }
    RenderCommand::EnableDepthTesting();
}

void oil::utils::SetDebugTexture(Ref<Texture2D> targetTexture)
{
    s_3DRenderData.DebugTexture->Resize(targetTexture->GetWidth(), targetTexture->GetHeight());

    s_3DRenderData.ActiveShader = s_3DRenderData.ShaderLib->Get("TexDisplayShader");
    s_3DRenderData.ActiveShader->Bind();
    switch (targetTexture->GetFormat() & 0xffff0000){
        case 0x00010000: s_3DRenderData.ActiveShader->SetInt("u_InputComponents", 1);
            break;
        case 0x00020000: s_3DRenderData.ActiveShader->SetInt("u_InputComponents", 2);
            break;
        case 0x00030000: s_3DRenderData.ActiveShader->SetInt("u_InputComponents", 3);
            break;
        case 0x00040000: s_3DRenderData.ActiveShader->SetInt("u_InputComponents", 4);
            break;
    }
    targetTexture->Bind(0);
    s_3DRenderData.ActiveShader->SetInt("u_InputTexture", 0);

    RenderActiveShaderToTexture(s_3DRenderData.DebugTexture);
}
