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

    PROFILE_SCOPE("Client2D::Render", m_ProfileResults);
    oil::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    oil::RenderCommand::Clear();

    oil::Renderer2D::BeginScene(m_CameraController.GetCamera());


    oil::Renderer2D::DrawQuad({-0.5f, -0.5f}, {0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, (rot += 0.25f * dt));
    oil::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f});
    oil::Renderer2D::DrawQuad({-0.5f, 0.5f}, {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f});
    oil::Renderer2D::DrawQuad({0.5f, 0.5f}, {0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f});
    //oil::Renderer2D::DrawQuad({0.5f, 0.5f}, {0.5f, 0.75f}, {0.2f, 0.5f, 0.8f, 1.0f}, glm::radians(rot +=3.0f*dt));
    oil::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {10.0f, 10.0f}, m_DefaultTexture, {10.0f, 10.0f});
    oil::Renderer2D::DrawQuad({0.0f, 0.0f, -0.0f}, {5.0f, 5.0f}, m_DefaultTexture, {15.0f, 15.0f}, {1.0f, 0.2f, 1.0f, 1.0f}, (rot2 -= 0.25f * dt));

       
    oil::Renderer2D::EndScene();
}

void Client2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    for (auto& result : m_ProfileResults){
        char label[50];
        strcpy(label, "%.3fms   ");
        strcat(label, result.Name);
        
        ImGui::Text(label, result.Time);
    }
    m_ProfileResults.clear();
    ImGui::End();
}

void Client2D::OnEvent(oil::Event &event)
{
    m_CameraController.OnEvent(event);
}
