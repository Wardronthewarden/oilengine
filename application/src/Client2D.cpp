#include <pch/oilpch.h>
#include "Client2D.h"
#include "imgui.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const char* s_MapTiles =
"wwwwwwwwwwwwwwwwwwwwwwww"
"wwwwwwwwwwwwwwwwwwwwwwww"
"wwwwwdddddddddddddwwwwww"
"wwwwwdddddddddddddwwwwww"
"wwwwwdddddddddddddwwwwww"
"wwwwwdddddddddddddwwwwww"
"wwwwwdddddddddddddwwwwww"
"wwwwwwwwwwwwwwwwwwwwwwww"
"wwwwwwwwwwwwwwwwwwwwwwww"
"wwwwwwwwwwwwwwwwwwwwwwww"
;




Client2D::Client2D()
    :Layer("Client2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Client2D::OnAttach()
{
    m_DefaultTexture = oil::Texture2D::Create("C:\\dev\\c++\\oilengine\\application\\Assets\\Textures\\Checkerboard.png");
    m_SpriteSheet = oil::Texture2D::Create("C:\\dev\\c++\\oilengine\\application\\Assets\\Game\\Textures\\RPGpack_sheet_2X.png");
    m_TextureStairs = oil::SubTexture2D::CreateFromCoords(m_SpriteSheet, {7, 6}, {128.0f, 128.0f});
    m_TextureBarrel = oil::SubTexture2D::CreateFromCoords(m_SpriteSheet, {8, 2}, {128.0f, 128.0f});
    m_TextureTree = oil::SubTexture2D::CreateFromCoords(m_SpriteSheet, {2, 1}, {128.0f, 128.0f}, { 1.0f, 2.0f });

    s_TextureMap['w'] = oil::SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 11}, {128.0f, 128.0f});
    s_TextureMap['d'] = oil::SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 11}, {128.0f, 128.0f});

    m_MapWidth = 24;
    m_MapHeight = strlen(s_MapTiles)/m_MapWidth;
       
    m_Particle.ColorBegin = {0.99f, 0.7f, 0.5f, 1.0f};
    m_Particle.ColorEnd = {0.99f, 0.35f, 0.25f, 0.8f};
    m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.2f, m_Particle.SizeEnd = 0.0f;
    m_Particle.LifeTime = 1.0f;
    m_Particle.Velocity = {0.0f, 0.0f};
    m_Particle.VelocityVariation = {3.0f, 1.0f};
    m_Particle.Position = {0.0f, 0.0f};

    m_CameraController.SetZoomLevel(5.0f);
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


    /* oil::Renderer2D::DrawQuad({-0.5f, -0.5f}, {0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, (rot += 0.25f * dt));
    oil::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f});
    oil::Renderer2D::DrawQuad({-0.5f, 0.5f}, {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f});
    oil::Renderer2D::DrawQuad({0.5f, 0.5f}, {0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}); */
    oil::Renderer2D::DrawQuad({0.0f, 0.0f, 0.1f}, {1.0f, 1.0f}, m_SpriteSheet, {1.0f, 1.0f}, m_TextureStairs);
    oil::Renderer2D::DrawQuad({0.0f, 0.3f, 0.1f}, {1.0f, 1.0f}, m_SpriteSheet, {1.0f, 1.0f}, m_TextureBarrel);
    oil::Renderer2D::DrawQuad({-1.5f, 0.5f, 0.1f}, {1.0f, 2.0f}, m_SpriteSheet, {1.0f, 1.0f}, m_TextureTree);
    //oil::Renderer2D::DrawQuad({0.0f, 0.0f, -0.05f}, {5.0f, 5.0f}, m_DefaultTexture, {15.0f, 15.0f}, m_SquareColor, (rot2 -= 0.25f * dt));


    for (uint32_t y = 0; y < m_MapHeight; ++y){
        for(uint32_t x = 0; x < m_MapWidth;  ++x){
            char tileType = s_MapTiles[x + y * m_MapWidth];
            oil::Ref<oil::SubTexture2D> texture;
            if(s_TextureMap.find(tileType) != s_TextureMap.end()){
                texture = s_TextureMap[tileType];
                oil::Renderer2D::DrawQuad({x - m_MapWidth / 2.0f, m_MapHeight - y - m_MapHeight / 2.0f, 0.0f}, {1.0f, 1.0f}, m_SpriteSheet, {1.0f, 1.0f}, texture);
                continue;
            }
            oil::Renderer2D::DrawQuad({x - m_MapWidth / 2.0f, m_MapHeight - y - m_MapHeight / 2.0f, 0.0f}, {1.0f, 1.0f}, m_DefaultTexture, {1.0f, 1.0f});
        }
    }

    // Testing renderer stress       
    oil::Renderer2D::EndScene();

   /*  oil::Renderer2D::BeginScene(m_CameraController.GetCamera());
    for(float y = -5.0f; y <= 5.0f; y += 0.1f){
        for(float x = -5.0f; x <= 5.0f; x += 0.1f){
            oil::Renderer2D::DrawQuad( {x, y}, {0.085, 0.085}, m_SquareColor);
    }
    }
    oil::Renderer2D::EndScene(); */

    if(oil::Input::IsMouseButtonPressed(OIL_MOUSE_BUTTON_LEFT)){

        auto [x,y] = oil::Input::GetMousePos();
        auto width = oil::Application::Get().GetWindow().GetWidth();
        auto height = oil::Application::Get().GetWindow().GetHeight();

        auto bounds = m_CameraController.GetBounds();
        auto pos = m_CameraController.GetCamera().GetPosition();
        x = (x/width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
        y = bounds.GetHeight() * 0.5f - (y/height) * bounds.GetHeight();
        m_Particle.Position = { x + pos.x, y+ pos.y};
        for (int i = 0; i<5; ++i)
            m_ParticleSystem.Emit(m_Particle);
    }

    //m_ParticleSystem.OnUpdate(dt);
    //m_ParticleSystem.OnRender(m_CameraController.GetCamera());
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
