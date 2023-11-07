#include <oil.h>

//-------Entry point------------

#include "oil/core/EntryPoint.h"

//------------------------------

#include "oilengine_export.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Client2D.h"


class ExampleLayer : public oil::Layer{
public:
    ExampleLayer()
    : Layer("Example"), m_CameraController(1280.0f / 720.0f, true), m_SquarePosition(0.0f) {
    m_SquareVA = oil::VertexArray::Create();

    float squareVertices[5 * 4] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
    };    

    oil::Ref<oil::VertexBuffer> squareVB = oil::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
    squareVB->SetLayout({
            {oil::ShaderDataType::Float3, "a_Position"},
            {oil::ShaderDataType::Float2, "a_TexCoord"}
});
    m_SquareVA->AddVertexBuffer(squareVB);

    uint32_t squareIndices[6] = {0,1,2, 2,3,0};
    oil::Ref<oil::IndexBuffer> squareIB = oil::IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t));
    m_SquareVA->SetIndexBuffer(squareIB);

    auto flatColorShader = m_ShaderLibrary.Load("C:\\dev\\c++\\oilengine\\application\\Assets\\Shaders\\UniformColor.glsl");
    auto textureShader = m_ShaderLibrary.Load("C:\\dev\\c++\\oilengine\\application\\Assets\\Shaders\\Texture.glsl");

    m_Texture = oil::Texture2D::Create("C:\\dev\\c++\\oilengine\\application\\Assets\\Textures\\Checkerboard.png");
    m_ChernoLogoTex = oil::Texture2D::Create("C:\\dev\\c++\\oilengine\\application\\Assets\\Textures\\ChernoLogo.png");

    std::dynamic_pointer_cast<oil::OpenGLShader>(textureShader)->Bind();
    std::dynamic_pointer_cast<oil::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

    
    oil::RenderCommand::SetClearColor({0.1f,0.1f,0.1f,1});
}

    void OnUpdate(oil::Timestep dt) override{

        // Update
        m_CameraController.OnUpdate(dt);

        //Render
        oil::RenderCommand::Clear();

        //FULL IMPLEMENTATION GOAL
        //Renderer::BeginScene(camera, lights, environment);

        //TEMPORARY IMPLEMENTATION
        oil::Renderer::BeginScene(m_CameraController.GetCamera());

        //Draw array of squares
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        m_CurrentShader = m_ShaderLibrary.Get("UniformColor");
        std::dynamic_pointer_cast<oil::OpenGLShader>(m_CurrentShader)->Bind();
        std::dynamic_pointer_cast<oil::OpenGLShader>(m_CurrentShader)->UploadUniformFloat3("u_Color", m_SquareColor);

        for (int y = 0; y < 20; y++){
            for (int x = 0; x < 20; x++){
                glm::vec3 pos(x*0.11f, y*0.11f, 0.0f);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
                oil::Renderer::Submit(m_CurrentShader, m_SquareVA, transform);
            }
        }

        m_CurrentShader = m_ShaderLibrary.Get("Texture");
        m_Texture->Bind();
        oil::Renderer::Submit(m_CurrentShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        m_ChernoLogoTex->Bind();
        oil::Renderer::Submit(m_CurrentShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
       
        oil::Renderer::EndScene();

    }

    virtual void OnImGuiRender() override{
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

    void OnEvent(oil::Event& event) override{
        m_CameraController.OnEvent(event);
        
        //TODO: camera zoom on resizing!
        //if (e.GetEventType() == oil::EventType::WindowResize){
        //    auto& re = (oil::WindowResizeEvent&) e;

        //    float zoom = (float)re.GetWidth() / 1280.0f;

        //    m_CameraController.SetZoomLevel(zoom);
        //}
    }

    bool OnKeyPressedEvent(oil::KeyPressedEvent& event){

        return false;
    }

    private:
        oil::OrthographicCameraController m_CameraController;
        
        oil::ShaderLibrary m_ShaderLibrary;

        oil::Ref<oil::Texture2D> m_Texture, m_ChernoLogoTex;

        oil::Ref<oil::VertexArray> m_SquareVA;

        glm::vec3 m_SquarePosition;
        glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.7f};
        oil::Ref<oil::Shader> m_CurrentShader;

};

class Barrel : public oil::Application{
public:
    Barrel() {
        //PushLayer(new ExampleLayer());
        PushLayer(new Client2D());
    };
    ~Barrel() {};



};


oil::Application* oil::CreateApplication(){
    OIL_INFO("Initialized client app!");
    return new Barrel();
}