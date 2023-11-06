#include "Client2D.h"
#include "imgui.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;


Client2D::Client2D()
    :Layer("Client2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Client2D::OnAttach()
{
    m_SquareVA = oil::VertexArray::Create();

    float squareVertices[5 * 4] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
    };  
    
    std::cout << "Current path is " << fs::current_path() << '\n';

    oil::Ref<oil::VertexBuffer> squareVB; 
    squareVB.reset(oil::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
    squareVB->SetLayout({
            {oil::ShaderDataType::Float3, "a_Position"},
            {oil::ShaderDataType::Float2, "a_Texcoord"}
});
    m_SquareVA->AddVertexBuffer(squareVB);

    uint32_t squareIndices[6] = {0,1,2, 2,3,0};
    oil::Ref<oil::IndexBuffer> squareIB;
    squareIB.reset(oil::IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t)));
    m_SquareVA->SetIndexBuffer(squareIB);

    m_FlatColorShader = oil::Shader::Create("Assets/Shaders/UniformColor.glsl");

    
    oil::RenderCommand::SetClearColor({0.1f,0.1f,0.1f,1});
}

void Client2D::OnDetach()
{
}

void Client2D::OnUpdate(oil::Timestep dt)
{
    std::cout << "Current path is " << fs::current_path() << '\n';
    OIL_CORE_WARN("HELLO");
    //Update 
    m_CameraController.OnUpdate(dt);

    oil::RenderCommand::SetClearColor({1.0f, 0.1f, 0.1f, 1});
    oil::RenderCommand::Clear();

    oil::Renderer::BeginScene(m_CameraController.GetCamera());

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

    std::dynamic_pointer_cast<oil::OpenGLShader>(m_FlatColorShader)->Bind();
    std::dynamic_pointer_cast<oil::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);


    oil::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

       
    oil::Renderer::EndScene();
}

void Client2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

void Client2D::OnEvent(oil::Event &event)
{
    m_CameraController.OnEvent(event);
}
