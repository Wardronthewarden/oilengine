#include "pch/oilpch.h"
#include "Renderer2D.h"

#include "Shader.h"
#include "VertexArray.h"

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "glm/gtc/matrix_transform.hpp"

namespace oil{
    
    struct QuadVertex{
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;

        // Editor
        int EntityID = 0;
    };

    struct Renderer2DData{
        static const uint32_t MaxQuads = 10000;
        static const uint32_t MaxVertices = MaxQuads * 4;
        static const uint32_t MaxIndices = MaxQuads * 6;
        static const uint32_t MaxTextureSlots = 32; //TODO: Base on GPU query

        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> TextureShader;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1;

        glm::vec4 QuadVertexPositions[4];

        Renderer2D::Stats stats;
    };

    static Renderer2DData s_2DRenderData;
    static size_t quadVertexCount = 4;
    static glm::vec2 defaultTexCoords[4] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
    };

    void Renderer2D::Init(){

        s_2DRenderData.QuadVertexArray = VertexArray::Create();
    
    OIL_INFO("Current working directory is: {0}", std::filesystem::current_path());

    s_2DRenderData.QuadVertexBuffer = VertexBuffer::Create(s_2DRenderData.MaxVertices * sizeof(QuadVertex));
    s_2DRenderData.QuadVertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float2, "a_Texcoord"},
            {ShaderDataType::Float, "a_TexIndex"},
            {ShaderDataType::Int, "a_EntityID"}
});
    s_2DRenderData.QuadVertexArray->AddVertexBuffer(s_2DRenderData.QuadVertexBuffer);

    s_2DRenderData.QuadVertexBufferBase = new QuadVertex[s_2DRenderData.MaxVertices];


    uint32_t* quadIndices = new uint32_t[s_2DRenderData.MaxIndices];

    uint32_t offset = 0;
    for (uint32_t i = 0; i< s_2DRenderData.MaxIndices;){
        quadIndices[i++] = offset + 0; 
        quadIndices[i++] = offset + 1; 
        quadIndices[i++] = offset + 2;

        quadIndices[i++] = offset + 2; 
        quadIndices[i++] = offset + 3; 
        quadIndices[i++] = offset + 0;

        offset +=4;
    }

    Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_2DRenderData.MaxIndices);
    s_2DRenderData.QuadVertexArray->SetIndexBuffer(quadIB);

    delete[] quadIndices;

    int32_t samplers[s_2DRenderData.MaxTextureSlots];
    for (uint32_t i = 0; i<s_2DRenderData.MaxTextureSlots; ++i)
        samplers[i] = i;

    s_2DRenderData.TextureShader = Shader::Create("Assets/Shaders/Texture.glsl"); 
    s_2DRenderData.TextureShader->Bind();
    s_2DRenderData.TextureShader->SetIntArray("u_Textures", samplers, s_2DRenderData.MaxTextureSlots);


    s_2DRenderData.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    s_2DRenderData.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
    s_2DRenderData.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
    s_2DRenderData.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    }
    void Renderer2D::ShutDown(){

    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera){
        s_2DRenderData.TextureShader->Bind();
        s_2DRenderData.TextureShader->SetMat4("u_VPMat", camera.GetVPMatrix());

        StartNewBatch();
    }
    void Renderer2D::BeginScene(const EditorCamera &camera)
    {
        glm::mat4 VPmat = camera.GetVPMatrix();

        s_2DRenderData.TextureShader->Bind();
        s_2DRenderData.TextureShader->SetMat4("u_VPMat", VPmat);

        StartNewBatch();
    }
    void Renderer2D::BeginScene(const Camera &camera, const glm::mat4 &transform)
    {
        glm::mat4 VPmat = camera.GetProjection() * glm::inverse(transform);

        s_2DRenderData.TextureShader->Bind();
        s_2DRenderData.TextureShader->SetMat4("u_VPMat", VPmat);

        StartNewBatch();
    }
    void Renderer2D::EndScene()
    {
        uint32_t dataSize = (uint8_t *)s_2DRenderData.QuadVertexBufferPtr - (uint8_t *)s_2DRenderData.QuadVertexBufferBase;
        s_2DRenderData.QuadVertexBuffer->SetData(s_2DRenderData.QuadVertexBufferBase, dataSize);

        Flush();
    }

    void Renderer2D::Flush()
    {
        for (uint32_t i = 0; i< s_2DRenderData.TextureSlotIndex; ++i){
            s_2DRenderData.TextureSlots[i]->Bind(i);
        }

        RenderCommand::DrawIndexed(s_2DRenderData.QuadVertexArray, s_2DRenderData.QuadIndexCount);
        
        //statistics
        s_2DRenderData.stats.DrawCalls++;
    }

    void Renderer2D::StartNewBatch(){
        //Reset batch information 
        s_2DRenderData.QuadIndexCount = 0;
        s_2DRenderData.QuadVertexBufferPtr = s_2DRenderData.QuadVertexBufferBase;

        s_2DRenderData.TextureSlotIndex = 1;

        s_2DRenderData.QuadVertexArray->Bind();
    }

    void Renderer2D::DrawQuad(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
    {
        if (s_2DRenderData.QuadIndexCount >= Renderer2DData::MaxIndices){
            EndScene();
            StartNewBatch();
        }
        
        const float texIndex  = 0.0f;

        for(size_t i = 0; i < quadVertexCount; ++i){
            s_2DRenderData.QuadVertexBufferPtr->Position = transform * s_2DRenderData.QuadVertexPositions[i];
            s_2DRenderData.QuadVertexBufferPtr->Color = color;
            s_2DRenderData.QuadVertexBufferPtr->TexCoord = defaultTexCoords[i];
            s_2DRenderData.QuadVertexBufferPtr->TexIndex = texIndex;
            s_2DRenderData.QuadVertexBufferPtr->EntityID = entityID;
            ++s_2DRenderData.QuadVertexBufferPtr;
        }

        s_2DRenderData.QuadIndexCount +=6;

        //statistics
        s_2DRenderData.stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture, const glm::vec2 &tilingFactor, const Ref<SubTexture2D> &subTexture, const glm::vec4 &color, int entityID)
    {
        const glm::vec2 *texCoords;
        if (subTexture)
            texCoords = subTexture->GetTexCoords();
        else
            texCoords = defaultTexCoords;
        
        if (s_2DRenderData.QuadIndexCount >= Renderer2DData::MaxIndices){
            EndScene();
            StartNewBatch();
        }
        
        float textureIndex = 0.0f;

        for (uint32_t i = 0; i< s_2DRenderData.TextureSlotIndex; ++i){
            if (*s_2DRenderData.TextureSlots[i].get() == *texture.get()){
                textureIndex = (float)i;
                break;
            };
        }
        
        if (textureIndex == 0.0f){
            textureIndex = (float)s_2DRenderData.TextureSlotIndex;
            s_2DRenderData.TextureSlots[s_2DRenderData.TextureSlotIndex] = texture;
            ++s_2DRenderData.TextureSlotIndex;
        }

        for(size_t i = 0; i < quadVertexCount; ++i){
            s_2DRenderData.QuadVertexBufferPtr->Position = transform * s_2DRenderData.QuadVertexPositions[i];
            s_2DRenderData.QuadVertexBufferPtr->Color = color;
            s_2DRenderData.QuadVertexBufferPtr->TexCoord = texCoords[i];
            s_2DRenderData.QuadVertexBufferPtr->TexIndex = textureIndex;
            s_2DRenderData.QuadVertexBufferPtr->EntityID = entityID;
            ++s_2DRenderData.QuadVertexBufferPtr;
        }

        s_2DRenderData.QuadIndexCount +=6;

        //statistics
        s_2DRenderData.stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, float rotation)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, color, rotation);
    }
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation){

        if (s_2DRenderData.QuadIndexCount >= Renderer2DData::MaxIndices){
            EndScene();
            StartNewBatch();
        }
        
        const float texIndex  = 0.0f;

        glm::mat4 transform;
         if(rotation == 0.0f){
            transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        }else{
            transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0,0,1)), {size.x, size.y, 1.0f});
        }
        DrawQuad(transform, color);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor, const Ref<SubTexture2D>& subTexture, const glm::vec4& color, float rotation){
        DrawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, subTexture, color, rotation);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor, const Ref<SubTexture2D>& subTexture, const glm::vec4& color, float rotation){
    
        glm::mat4 transform;
         if(rotation == 0.0f){
            transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        }else{
            transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0,0,1)), {size.x, size.y, 1.0f});
        }

        DrawQuad(transform, texture, tilingFactor, subTexture, color);
    }
    void Renderer2D::DrawSprite(const glm::mat4 &transform, SpriteRendererComponent &src, int entityID)
    {
        if (src.Texture)
            DrawQuad(transform, src.Texture, {src.TilingFactor, src.TilingFactor}, nullptr, src.Color, entityID);
        else
            DrawQuad(transform, src.Color, entityID);
    }
    void Renderer2D::ResetStats()
    {
        memset(&s_2DRenderData.stats, 0, sizeof(Stats));
    }
    Renderer2D::Stats Renderer2D::GetStats()
    {
        return s_2DRenderData.stats;
    }
}