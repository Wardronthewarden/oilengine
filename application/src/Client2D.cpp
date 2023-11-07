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
    fps = 1.0f/dt.GetSeconds();

    //stats
    oil::Renderer2D::ResetStats();
    PROFILE_SCOPE("Client2D::Render", m_ProfileResults);

    oil::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    oil::RenderCommand::Clear();

    oil::Renderer2D::BeginScene(m_CameraController.GetCamera());


    oil::Renderer2D::DrawQuad({-0.5f, -0.5f}, {0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, (rot += 0.25f * dt));
    oil::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f});
    oil::Renderer2D::DrawQuad({-0.5f, 0.5f}, {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f});
    oil::Renderer2D::DrawQuad({0.5f, 0.5f}, {0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f});
    oil::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {10.0f, 10.0f}, m_DefaultTexture, {10.0f, 10.0f});
    oil::Renderer2D::DrawQuad({0.0f, 0.0f, -0.05f}, {5.0f, 5.0f}, m_DefaultTexture, {15.0f, 15.0f}, m_SquareColor, (rot2 -= 0.25f * dt));


    // Testing renderer stress       
    oil::Renderer2D::EndScene();

    oil::Renderer2D::BeginScene(m_CameraController.GetCamera());
    for(float y = -5.0f; y <= 5.0f; y += 0.1f){
        for(float x = -5.0f; x <= 5.0f; x += 0.1f){
            oil::Renderer2D::DrawQuad( {x, y}, {0.085, 0.085}, m_SquareColor);
    }
    }
    oil::Renderer2D::EndScene();
}

void Client2D::OnImGuiRender()
{
    ImGui::Begin("Settings");

    auto stats = oil::Renderer2D::GetStats();

    ImGui::Text("General Stats:");
    ImGui::Text("FPS: %.2f", fps);


    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Triangles: %d", stats.GetTotalTriangleCount());
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

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
