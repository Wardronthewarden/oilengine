#include <pch/oilpch.h>
#include "Client2D.h"
#include "imgui.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>




Client2D::Client2D()
    :Layer("Client2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Client2D::OnAttach()
{
    m_DefaultTexture = oil::Texture2D::Create("C:\\dev\\c++\\oilengine\\application\\Assets\\Textures\\Checkerboard.png");
       
}

void Client2D::OnDetach()
{
}

void Client2D::OnUpdate(oil::Timestep dt)
{
    //Update 
    m_CameraController.OnUpdate(dt);

    oil::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    oil::RenderCommand::Clear();

    oil::Renderer2D::BeginScene(m_CameraController.GetCamera());


    //TODO: shader_setMat4, shader_setFloat4

    //std::dynamic_pointer_cast<oil::OpenGLShader>(m_FlatColorShader)->Bind();
    //std::dynamic_pointer_cast<oil::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

    oil::Renderer2D::DrawQuad({-1.0f, 0.0f}, {1.0f, 0.5f}, {1.0f, 0.5f, 0.3f, 1.0f});
    oil::Renderer2D::DrawQuad({0.5f, 0.5f}, {0.5f, 0.75f}, {0.2f, 0.5f, 0.8f, 1.0f}, (rot +=3.0f*dt));
    oil::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {10.0f, 10.0f}, m_DefaultTexture, (rot +=3.0f*dt));
    //oil::Renderer2D::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

       
    oil::Renderer2D::EndScene();
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
