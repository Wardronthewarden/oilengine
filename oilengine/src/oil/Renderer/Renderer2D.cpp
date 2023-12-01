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
    };

    struct Renderer2DData{
        static const uint32_t MaxQuads = 10000;
        static const uint32_t MaxVertices = MaxQuads * 4;
        static const uint32_t MaxIndices = MaxQuads * 6;
        static const uint32_t MaxTextureSlots = 32; //TODO: Base on GPU query

        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WhiteTexture;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1;

        glm::vec4 QuadVertexPositions[4];

        Renderer2D::Stats stats;
    };

    static Renderer2DData s_RenderData;
    static size_t quadVertexCount = 4;
    static glm::vec2 defaultTexCoords[4] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
    };

    void Renderer2D::Init(){

        s_RenderData.QuadVertexArray = VertexArray::Create();
    
    OIL_INFO("Current working directory is: {0}", std::filesystem::current_path());

    s_RenderData.QuadVertexBuffer = VertexBuffer::Create(s_RenderData.MaxVertices * sizeof(QuadVertex));
    s_RenderData.QuadVertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float2, "a_Texcoord"},
            {ShaderDataType::Float, "a_TexIndex"}
});
    s_RenderData.QuadVertexArray->AddVertexBuffer(s_RenderData.QuadVertexBuffer);

    s_RenderData.QuadVertexBufferBase = new QuadVertex[s_RenderData.MaxVertices];


    uint32_t* quadIndices = new uint32_t[s_RenderData.MaxIndices];

    uint32_t offset = 0;
    for (uint32_t i = 0; i< s_RenderData.MaxIndices;){
        quadIndices[i++] = offset + 0; 
        quadIndices[i++] = offset + 1; 
        quadIndices[i++] = offset + 2;

        quadIndices[i++] = offset + 2; 
        quadIndices[i++] = offset + 3; 
        quadIndices[i++] = offset + 0;

        offset +=4;
    }

    Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_RenderData.MaxIndices);
    s_RenderData.QuadVertexArray->SetIndexBuffer(quadIB);

    delete[] quadIndices;

    s_RenderData.WhiteTexture = Texture2D::Create(1,1);
    uint32_t whiteTextureData = 0xffffffff;
    s_RenderData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

    int32_t samplers[s_RenderData.MaxTextureSlots];
    for (uint32_t i = 0; i<s_RenderData.MaxTextureSlots; ++i)
        samplers[i] = i;

    s_RenderData.TextureShader = Shader::Create("Assets/Shaders/Texture.glsl"); 
    s_RenderData.TextureShader->Bind();
    s_RenderData.TextureShader->SetIntArray("u_Textures", samplers, s_RenderData.MaxTextureSlots);

    //initialize all tex slots to 0
    s_RenderData.TextureSlots[0] = s_RenderData.WhiteTexture;

    s_RenderData.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    s_RenderData.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
    s_RenderData.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
    s_RenderData.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    }
    void Renderer2D::ShutDown(){

    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera){
        s_RenderData.TextureShader->Bind();
        s_RenderData.TextureShader->SetMat4("u_VPMat", camera.GetVPMatrix());

        StartNewBatch();
    }
    void Renderer2D::BeginScene(const Camera &camera, const glm::mat4& transform)
    {
        glm::mat4 VPmat = camera.GetProjection() * glm::inverse(transform);

        s_RenderData.TextureShader->Bind();
        s_RenderData.TextureShader->SetMat4("u_VPMat", VPmat);

        StartNewBatch();
    }
    void Renderer2D::EndScene()
    {
        uint32_t dataSize = (uint8_t *)s_RenderData.QuadVertexBufferPtr - (uint8_t *)s_RenderData.QuadVertexBufferBase;
        s_RenderData.QuadVertexBuffer->SetData(s_RenderData.QuadVertexBufferBase, dataSize);

        Flush();
    }

    void Renderer2D::Flush()
    {
        for (uint32_t i = 0; i< s_RenderData.TextureSlotIndex; ++i){
            s_RenderData.TextureSlots[i]->Bind(i);
        }

        RenderCommand::DrawIndexed(s_RenderData.QuadVertexArray, s_RenderData.QuadIndexCount);
        
        //statistics
        s_RenderData.stats.DrawCalls++;
    }

    void Renderer2D::StartNewBatch(){
        //Reset batch information
        s_RenderData.QuadIndexCount = 0;
        s_RenderData.QuadVertexBufferPtr = s_RenderData.QuadVertexBufferBase;

        s_RenderData.TextureSlotIndex = 1;
    }

    void Renderer2D::DrawQuad(const glm::mat4 &transform, const glm::vec4 &color)
    {
        if (s_RenderData.QuadIndexCount >= Renderer2DData::MaxIndices){
            EndScene();
            StartNewBatch();
        }
        
        const float texIndex  = 0.0f;

        for(size_t i = 0; i < quadVertexCount; ++i){
            s_RenderData.QuadVertexBufferPtr->Position = transform * s_RenderData.QuadVertexPositions[i];
            s_RenderData.QuadVertexBufferPtr->Color = color;
            s_RenderData.QuadVertexBufferPtr->TexCoord = defaultTexCoords[i];
            s_RenderData.QuadVertexBufferPtr->TexIndex = texIndex;
            ++s_RenderData.QuadVertexBufferPtr;
        }

        s_RenderData.QuadIndexCount +=6;

        //statistics
        s_RenderData.stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture, const glm::vec2 &tilingFactor, const Ref<SubTexture2D> &subTexture, const glm::vec4 &color)
    {
        const glm::vec2 *texCoords;
        if (subTexture)
            texCoords = subTexture->GetTexCoords();
        else
            texCoords = defaultTexCoords;
        
        if (s_RenderData.QuadIndexCount >= Renderer2DData::MaxIndices){
            EndScene();
            StartNewBatch();
        }
        
        float textureIndex = 0.0f;

        for (uint32_t i = 1; i< s_RenderData.TextureSlotIndex; ++i){
            if (*s_RenderData.TextureSlots[i].get() == *texture.get()){
                textureIndex = (float)i;
                break;
            };
        }
        
        if (textureIndex == 0.0f){
            textureIndex = (float)s_RenderData.TextureSlotIndex;
            s_RenderData.TextureSlots[s_RenderData.TextureSlotIndex] = texture;
            ++s_RenderData.TextureSlotIndex;
        }

        for(size_t i = 0; i < quadVertexCount; ++i){
            s_RenderData.QuadVertexBufferPtr->Position = transform * s_RenderData.QuadVertexPositions[i];
            s_RenderData.QuadVertexBufferPtr->Color = color;
            s_RenderData.QuadVertexBufferPtr->TexCoord = texCoords[i];
            s_RenderData.QuadVertexBufferPtr->TexIndex = textureIndex;
            ++s_RenderData.QuadVertexBufferPtr;
        }

        s_RenderData.QuadIndexCount +=6;

        //statistics
        s_RenderData.stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, float rotation)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, color, rotation);
    }
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation){

        if (s_RenderData.QuadIndexCount >= Renderer2DData::MaxIndices){
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
    void Renderer2D::ResetStats()
    {
        memset(&s_RenderData.stats, 0, sizeof(Stats));
    }
    Renderer2D::Stats Renderer2D::GetStats()
    {
        return s_RenderData.stats;
    }
}