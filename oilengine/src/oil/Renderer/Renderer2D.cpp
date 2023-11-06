#include "pch/oilpch.h"
#include "Renderer2D.h"

#include "Shader.h"
#include "VertexArray.h"

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "glm/gtc/matrix_transform.hpp"

namespace oil{
    struct Renderer2DStorage{
        Ref<VertexArray> QuadVertexArray;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WhiteTexture;   
    };

    static Renderer2DStorage* s_RenderData;

    void Renderer2D::Init(){
        s_RenderData = new Renderer2DStorage();

        s_RenderData->QuadVertexArray = VertexArray::Create();

    float squareVertices[5 * 4] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
    };  
    
    OIL_INFO("Current path is: {0}", std::filesystem::current_path());

    Ref<VertexBuffer> squareVB; 
    squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
    squareVB->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float2, "a_Texcoord"}
});
    s_RenderData->QuadVertexArray->AddVertexBuffer(squareVB);

    uint32_t squareIndices[6] = {0,1,2, 2,3,0};
    Ref<IndexBuffer> squareIB;
    squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t)));
    s_RenderData->QuadVertexArray->SetIndexBuffer(squareIB);

    s_RenderData->WhiteTexture = Texture2D::Create(1,1);
    uint32_t whiteTextureData = 0xffffffff;
    s_RenderData->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

    s_RenderData->TextureShader = Shader::Create("Assets/Shaders/Texture.glsl"); 
    s_RenderData->TextureShader->Bind();
    s_RenderData->TextureShader->SetInt("u_Texture", 0);

    }
    void Renderer2D::ShutDown(){
        delete s_RenderData;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera){
        s_RenderData->TextureShader->Bind();
        s_RenderData->TextureShader->SetMat4("u_VPMat", camera.GetVPMatrix());
    }
    void Renderer2D::EndScene(){
        
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation){
        DrawQuad({position.x, position.y, 0.0f}, size, color, rotation);
    }
    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation){
        s_RenderData->TextureShader->Bind();
        s_RenderData->TextureShader->SetFloat4("u_Color", color);
        s_RenderData->WhiteTexture->Bind();

        glm::mat4 transform;
        if(rotation == 0.0f){
            transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        }else{
            transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0,0,1)), {size.x, size.y, 1.0f});
        }
        s_RenderData->TextureShader->SetMat4("u_Transform",transform);


        s_RenderData->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_RenderData->QuadVertexArray);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& color, float rotation){
        DrawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, color, rotation);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& color, float rotation){
        s_RenderData->TextureShader->SetFloat4("u_Color", color);
        s_RenderData->TextureShader->SetFloat2("u_TilingFactor", tilingFactor);
        texture->Bind();
        
        s_RenderData->TextureShader->Bind();
        glm::mat4 transform;

        if(rotation == 0.0f){
            transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        }else{
            OIL_TRACE("Rotated {0}!", 1);
            transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0,0,1)), {size.x, size.y, 1.0f});
        }
        s_RenderData->TextureShader->SetMat4("u_Transform", transform);

        s_RenderData->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_RenderData->QuadVertexArray);
    }
}