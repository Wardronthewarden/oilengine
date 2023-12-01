#include <pch/oilpch.h>
#include "EditorLayer.h"
#include "imgui.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace oil{

static const char* s_MapTiles =
"wwwwwwwwwwwwwwwwwwwwwwww"
"wwwwwwwwwwwwwwwwwwwwwwww"
"wwwwwdddddddddddddwwwwww"
"wwwwwdddddddddddddwwwwww"
"wwwwwdddddddddddddwwwwww"
"wwwwwdddddddddddddwwwwww"
"wwwwwdddddddddddddwwwwww"
"wwwwwwwwwwwwwwwwwdwwwwww"
"wwwwwwwwwwwwwwwwwdwwwwww"
"wwwwwwwwwwwwwwwwwwwwwwww"
;




EditorLayer::EditorLayer()
    :Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true)
{
}

void EditorLayer::OnAttach()
{
    //Scene
    m_ActiveScene = CreateRef<Scene>();


    // Entity
    m_SquareEntity = m_ActiveScene->CreateEntity("Square");
    m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});


    //Textures
    m_DefaultTexture = Texture2D::Create("C:\\dev\\c++\\oilengine\\application\\Assets\\Textures\\Checkerboard.png");
    m_SpriteSheet = Texture2D::Create("C:\\dev\\c++\\oilengine\\application\\Assets\\Game\\Textures\\RPGpack_sheet_2X.png");
    m_TextureStairs = SubTexture2D::CreateFromCoords(m_SpriteSheet, {7, 6}, {128.0f, 128.0f});
    m_TextureBarrel = SubTexture2D::CreateFromCoords(m_SpriteSheet, {8, 2}, {128.0f, 128.0f});
    m_TextureTree = SubTexture2D::CreateFromCoords(m_SpriteSheet, {2, 1}, {128.0f, 128.0f}, { 1.0f, 2.0f });

    //Tilemap
    s_TextureMap['w'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 11}, {128.0f, 128.0f});
    s_TextureMap['d'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 11}, {128.0f, 128.0f});

    m_MapWidth = 24;
    m_MapHeight = strlen(s_MapTiles)/m_MapWidth;

    //Framebuffer
    FrameBufferSpecification fbSpec;
    fbSpec.Width = 1280;
    fbSpec.Height = 720;
    m_FrameBuffer = FrameBuffer::Create(fbSpec);


    //Camera
    m_CameraController.SetZoomLevel(5.0f);

    m_CameraEntity = m_ActiveScene->CreateEntity("Camera");
    m_CameraEntity.AddComponent<CameraComponent>();

    class TestScript : public ScriptableEntity{
    public:
        void OnCreate(){
        }

        void OnDestroy(){};

        void OnUpdate(Timestep dt){
            auto& transform = GetComponent<TransformComponent>().Transform;
            float speed = 5.0f;
            if (Input::IsKeyPressed(OIL_KEY_A))
                transform[3][0] -= speed*dt;
            if (Input::IsKeyPressed(OIL_KEY_D))
                transform[3][0] += speed*dt;
            if (Input::IsKeyPressed(OIL_KEY_W))
                transform[3][1] += speed*dt;
            if (Input::IsKeyPressed(OIL_KEY_S))
                transform[3][1] -= speed*dt;
        }
    };
    m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<TestScript>();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(Timestep dt)
{
    if (FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
        m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
        (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)){
            m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }


    //Update 
    if(m_ViewportFocused)
        m_CameraController.OnUpdate(dt);
    fps = 1.0f/dt.GetSeconds();

    //Update scene

    //stats
    Renderer2D::ResetStats();
    PROFILE_SCOPE("EditorLayer::Render", m_ProfileResults);
    m_FrameBuffer->Bind();

    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();

    m_ActiveScene->OnUpdate(dt);


    m_FrameBuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
    
    static bool dockingEnabled = true;
    if(dockingEnabled){

    //----------------------------------------------------------
    static bool DockSpaceOpen = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &DockSpaceOpen, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            if(ImGui::MenuItem("Exit")) Application::Get().CloseApplication();
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    ImGui::Begin("Stats");


    auto stats = Renderer2D::GetStats();

    if (m_SquareEntity){
        auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
        ImGui::Text("%s", tag.c_str());
        auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
        ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));

        ImGui::Separator();
    }

    auto& camera = m_CameraEntity.GetComponent<CameraComponent>().Camera;
    float orthoSize = camera.GetOrthographicSize();
    if(ImGui::DragFloat("Camera Ortho Size", &orthoSize)){
        camera.SetOrthographicSize(orthoSize);
    };

    ImGui::Text("General Stats:");
    ImGui::Text("FPS: %.2f", fps);


    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Triangles: %d", stats.GetTotalTriangleCount());
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());


    ImGui::Separator();
    ImGui::Text("Profiling results: ");
    for (auto& result : m_ProfileResults){
        char label[50];
        strcpy(label, "%.3fms  | ");
        strcat(label, result.Name);
        
        ImGui::Text(label, result.Time);
    }
    m_ProfileResults.clear();
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0,0});
    ImGui::Begin("Viewport");

    m_ViewportFocused = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
    
    
    Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y};


    uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
    ImGui::Image((void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    ImGui::End();
    ImGui::PopStyleVar();


    ImGui::End();
    }
}

void EditorLayer::OnEvent(Event &event)
{
    m_CameraController.OnEvent(event);
}

}