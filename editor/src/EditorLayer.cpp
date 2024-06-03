#include <pch/oilpch.h>
#include "EditorLayer.h"
#include "imgui.h"
#include "imgui_internal.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "oil/Scene/SceneSerializer.h"
#include "oil/Utils/PlatformUtils.h"
#include "oil/Math/Math.h"

#include "ImGuizmo.h"

namespace oil{

    extern const std::filesystem::path g_AssetPath;



EditorLayer::EditorLayer()
    :Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true)
{
}

void EditorLayer::OnAttach()
{
    //Scene
    m_ActiveScene = CreateRef<Scene>();

    m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    m_ActiveSceneFilepath = "Assets/Scenes/cube.oil";

    //Asset management setup
    //TODO: draw from init file
    OpenScene(m_ActiveSceneFilepath);

    //Framebuffer
    m_FrameBuffer = Renderer3D::GetFrameBuffer();


    //Textures
    m_DefaultTexture = Texture2D::Create("Internal/Assets/Textures/Checkerboard.png");
    m_IconPlay = Texture2D::Create("Internal/Assets/Textures/PlayButton.png");
    m_IconStop = Texture2D::Create("Internal/Assets/Textures/StopButton.png");


#if 0

    // Entity
    m_SquareEntity = m_ActiveScene->CreateEntity("Square");
    m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});

    auto OtherSquare = m_ActiveScene->CreateEntity("Other Square");
    OtherSquare.AddComponent<SpriteRendererComponent>(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});


    m_SpriteSheet = Texture2D::Create("C:\\dev\\c++\\oilengine\\application\\Assets\\Game\\Textures\\RPGpack_sheet_2X.png");
    m_TextureStairs = SubTexture2D::CreateFromCoords(m_SpriteSheet, {7, 6}, {128.0f, 128.0f});
    m_TextureBarrel = SubTexture2D::CreateFromCoords(m_SpriteSheet, {8, 2}, {128.0f, 128.0f});
    m_TextureTree = SubTexture2D::CreateFromCoords(m_SpriteSheet, {2, 1}, {128.0f, 128.0f}, { 1.0f, 2.0f });

    //Tilemap
    s_TextureMap['w'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 11}, {128.0f, 128.0f});
    s_TextureMap['d'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 11}, {128.0f, 128.0f});



    //Camera
    m_CameraController.SetZoomLevel(5.0f);

    m_CameraEntity = m_ActiveScene->CreateEntity("Camera");
    m_CameraEntity.AddComponent<CameraComponent>();


    

    class TestScript : public ScriptableEntity{
    public:

        void OnUpdate(Timestep dt){
            auto& translation = GetComponent<TransformComponent>().Translation;
            float speed = 5.0f;
            if (Input::IsKeyPressed(OIL_KEY_A))
                translation.x -= speed*dt;
            if (Input::IsKeyPressed(OIL_KEY_D))
                translation.x += speed*dt;
            if (Input::IsKeyPressed(OIL_KEY_W))
                translation.y += speed*dt;
            if (Input::IsKeyPressed(OIL_KEY_S))
                translation.y -= speed*dt;
        }
    };
    m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<TestScript>();

#endif
    m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;

    // Editor utilities
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    m_ContentBrowserPanel.Init();
    m_EditorCamera = EditorCamera(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

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
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }


    //Update 
    if(m_ViewportFocused){
        m_CameraController.OnUpdate(dt);
        m_EditorCamera.OnUpdate(dt);
    }
    fps = 1.0f/dt.GetSeconds();


    //Update scene

    //stats
    //Renderer3D::ResetStats();
    //PROFILE_SCOPE("EditorLayer::Render", m_ProfileResults);
    m_FrameBuffer->Bind();

    // Clear entity ID attachment
    m_FrameBuffer->ClearAttachment(1, -1);

    switch (m_SceneState){
        case SceneState::Edit:{
            m_ActiveScene->OnUpdateEditor(dt, m_EditorCamera);
            break;
        }
        case SceneState::Play:{
            m_ActiveScene->OnUpdate(dt);
            break;
        }
    }


    auto [mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
    my = viewportSize.y -my;

    int mouseX = (int)mx;
    int mouseY = (int)my;

    if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y){
        int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
        m_HoveredEntity = pixelData < 0 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.GetContent().get());
    }

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

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &DockSpaceOpen, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("New", "Ctrl+N")){
                NewScene();
            }

            if(ImGui::MenuItem("Open...", "Ctrl+O")){
                OpenScene();
            }

            if(ImGui::MenuItem("Save", "Ctrl+S")){
                SaveScene();
            }

            if(ImGui::MenuItem("Save As...", "Ctrl+Shift+S")){
                SaveSceneAs();
            }

            if(ImGui::MenuItem("Import...", "Ctrl+I")){
                Import();
            }

            if(ImGui::MenuItem("Exit")) Application::Get().CloseApplication();
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    
    RenderViewport();

    UI_Toolbar();

    ImGui::End();
    m_SceneHierarchyPanel.OnImGuiRender();
    m_ContentBrowserPanel.OnImGuiRender();
    }
}

void EditorLayer::OnEvent(Event &event)
{
    m_CameraController.OnEvent(event);
    m_EditorCamera.OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(OIL_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    dispatcher.Dispatch<KeyReleasedEvent>(OIL_BIND_EVENT_FN(EditorLayer::OnKeyReleased));
    dispatcher.Dispatch<MouseButtonPressedEvent>(OIL_BIND_EVENT_FN(EditorLayer::OnMousePressed));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(OIL_BIND_EVENT_FN(EditorLayer::OnMouseReleased));
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent &e)
{
    //Shortcuts
    if (e.GetRepeatCount() > 0)
        return false;

    m_ControlPressed = Input::IsKeyPressed(OIL_KEY_LEFT_CONTROL) || Input::IsKeyPressed(OIL_KEY_RIGHT_CONTROL);
    m_ShiftPressed = Input::IsKeyPressed(OIL_KEY_LEFT_SHIFT) || Input::IsKeyPressed(OIL_KEY_RIGHT_SHIFT);
    m_AltPressed = Input::IsKeyPressed(OIL_KEY_LEFT_ALT);

    switch (e.GetKeyCode()){
        case OIL_KEY_S:{
            if (m_ControlPressed && m_ShiftPressed)
                SaveSceneAs();
            else if (m_ControlPressed)
                SaveScene();
            break;
        }
        
        case OIL_KEY_N:{
            if (m_ControlPressed)
                NewScene();
            break;
        }
        
        case OIL_KEY_O:{
            if (m_ControlPressed)
                OpenScene();
            break;
        }

        case OIL_KEY_I:{
            if (m_ControlPressed)
               Import();
            break;
        }

        case OIL_KEY_Q:{
            if (!Input::IsMouseButtonPressed(OIL_MOUSE_BUTTON_RIGHT))
                m_GizmoType = -1;
            break;
        }
        case OIL_KEY_W:{
            if (!Input::IsMouseButtonPressed(OIL_MOUSE_BUTTON_RIGHT))
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        }
        case OIL_KEY_E:{
            if (!Input::IsMouseButtonPressed(OIL_MOUSE_BUTTON_RIGHT))
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
        }
        case OIL_KEY_R:{
            if (!Input::IsMouseButtonPressed(OIL_MOUSE_BUTTON_RIGHT))
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
            break;
        }

        case OIL_KEY_F:{
            Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
            if (selectedEntity) {
                if (selectedEntity.HasComponent<TransformComponent>()){
                    m_EditorCamera.SetFocalPoint(selectedEntity.GetComponent<TransformComponent>().Translation);
                    m_EditorCamera.OrbitAroundFocalPoint();
                }
            }
            break;
        }

        case OIL_KEY_LEFT_ALT:
        case OIL_KEY_RIGHT_ALT:{
            if (Input::IsMouseButtonPressed(OIL_MOUSE_BUTTON_LEFT)){
                    m_EditorCamera.SetInitialMousePosition({Input::GetMouseX(), Input::GetMouseY()});
                    Application::Get().GetWindow().SetCursorMode(CursorMode::CursorDisabled);
            }
            break;
        }

        default:
            break;
    }
    return false;
}
bool EditorLayer::OnKeyReleased(KeyReleasedEvent &e)
{
    m_ControlPressed = Input::IsKeyPressed(OIL_KEY_LEFT_CONTROL) || Input::IsKeyPressed(OIL_KEY_RIGHT_CONTROL);
    m_ShiftPressed = Input::IsKeyPressed(OIL_KEY_LEFT_SHIFT) || Input::IsKeyPressed(OIL_KEY_RIGHT_SHIFT);
    m_AltPressed = Input::IsKeyPressed(OIL_KEY_LEFT_ALT);
    
    switch(e.GetKeyCode()){

        case OIL_KEY_LEFT_ALT:
        case OIL_KEY_RIGHT_ALT:{
            if (Input::IsMouseButtonPressed(OIL_MOUSE_BUTTON_LEFT)){
                Application::Get().GetWindow().SetCursorMode(CursorMode::CursorNormal);
            }
            break;
        }
    }
    return false;
}
bool EditorLayer::OnMousePressed(MouseButtonPressedEvent &e)
{
    if (m_ViewportFocused){
        switch (e.GetMouseButton()){
            case OIL_MOUSE_BUTTON_RIGHT:{
                m_EditorCamera.SetInitialMousePosition({Input::GetMouseX(), Input::GetMouseY()});
                Application::Get().GetWindow().SetCursorMode(CursorMode::CursorDisabled);
                break;
            }
            case OIL_MOUSE_BUTTON_LEFT:{
                if (m_AltPressed){
                    m_EditorCamera.SetInitialMousePosition({Input::GetMouseX(), Input::GetMouseY()});
                    Application::Get().GetWindow().SetCursorMode(CursorMode::CursorDisabled);
                } else {
                    if (!ImGuizmo::IsOver())
                        m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
                }
                break;
            }
            case OIL_MOUSE_BUTTON_MIDDLE:{
                if (m_AltPressed){
                    m_EditorCamera.SetInitialMousePosition({Input::GetMouseX(), Input::GetMouseY()});
                    Application::Get().GetWindow().SetCursorMode(CursorMode::CursorDisabled);
                }
                break;
            }
            default:
                break;
        }

    }
    return true;
}
bool EditorLayer::OnMouseReleased(MouseButtonReleasedEvent &e)
{
    switch (e.GetMouseButton()){
        case OIL_MOUSE_BUTTON_RIGHT:{
            Application::Get().GetWindow().SetCursorMode(CursorMode::CursorNormal);
            break;
        }
        case OIL_MOUSE_BUTTON_LEFT:{
            Application::Get().GetWindow().SetCursorMode(CursorMode::CursorNormal);
            break;
        }
        case OIL_MOUSE_BUTTON_MIDDLE:{
            Application::Get().GetWindow().SetCursorMode(CursorMode::CursorNormal);
            break;
        }
        default:
            break;

    }
    return true;
}
void EditorLayer::NewScene()
{
    m_ActiveScene = AssetManager::CreateAsset<Scene>();
    m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}
void EditorLayer::OpenScene()
{
    m_ActiveSceneFilepath = FileDialogs::OpenFile("Oil Scene (*.oil)\0*.oil\0");

        if(!m_ActiveSceneFilepath.empty()){
            OpenScene(m_ActiveSceneFilepath);
        }
}
void EditorLayer::OpenScene(const std::filesystem::path &path)
{
    if(std::filesystem::exists(path)){
        UUID id = AssetManager::LoadAsset(path);
        m_ActiveScene = AssetManager::GetAsset<Scene>(id);
        return;
    }
    OIL_CORE_WARN("Scene {0} does not exist. Opening default scene.", path);

}
void EditorLayer::SaveSceneAs()
{
    m_ActiveSceneFilepath = FileDialogs::SaveFile("Oil Scene (*.oil)\0*.oil\0");
                
        if(!m_ActiveSceneFilepath.empty()){
            AssetManager::SaveAssetAs<Scene>(m_ActiveScene, m_ActiveSceneFilepath);
        }
}
void EditorLayer::SaveScene()
{
    if(!m_ActiveSceneFilepath.empty()){
        AssetManager::SaveAsset(m_ActiveScene);
    }
}
void EditorLayer::Import()
{
    std::filesystem::path importSrc = FileDialogs::OpenFile("Import target\0*.png;*.jpg;*.obj;*.gltf;*.fbx\0\0");
    if(!importSrc.empty()){
        std::string extension = importSrc.extension().string();
        if(extension == ".png" || extension == ".jpg")
            AssetImporter::ImportImage(importSrc);
        else if(extension == ".obj" || extension == ".gltf" || extension == ".fbx")
            AssetImporter::ImportModel(importSrc);
    }
}
void EditorLayer::OnScenePlay()
{
    m_SceneState  = SceneState::Play;
}
void EditorLayer::OnSceneStop()
{
    m_SceneState  = SceneState::Edit;
}
void EditorLayer::UI_Toolbar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    auto& colors = ImGui::GetStyle().Colors;
    auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
    auto& buttonActive = colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

    ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    //Play button
    float size = ImGui::GetWindowHeight() - 3.0f;
    Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5));
    if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), {size, size}, {0, 0}, {1, 1}, 0)){
        if (m_SceneState == SceneState::Edit)
            OnScenePlay();
        else if (m_SceneState == SceneState::Play)
            OnSceneStop();

    }
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);

    ImGui::End();

}
void EditorLayer::RenderViewport()
{
    ImGuiIO io = ImGui::GetIO();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0,0});
    ImGui::Begin("Viewport");
    

    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    // Calculate viewport bounds
    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportOffset = ImGui::GetWindowPos();
    m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };


    //TODO: Rewrite this to make Input handling easier to manage
    m_ViewportFocused = ImGui::IsWindowFocused() ? ImGui::IsWindowHovered() || Application::Get().GetWindow().GetCursorMode() == CursorMode::CursorDisabled 
        : ImGui::IsWindowHovered();    
    
    Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y};


    // VIEWPORT IMAGE RENDERING -----------------------------
    uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID(0);
    ImGui::Image((void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

    if (ImGui::BeginDragDropTarget()){
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")){
            Ref<DragDropInfo> info = AssetManager::GetDragDropInfo();

            if (info->contentType == ContentType::Scene);
                OpenScene(std::filesystem::path(g_AssetPath) / info->itemPath);
        }

        ImGui::EndDragDropTarget();
    }
    // ------------------------------------------------------

    //Gizmos
    Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
    if (selectedEntity && m_GizmoType != -1){
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

        // Runtime camera
        //auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
        //const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
        //const glm::mat4& cameraProjection = camera.GetProjection();
        //glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

        // Editor camera
        const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
        glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

        // Entity transform
        auto& tc = selectedEntity.GetComponent<TransformComponent>();
        glm::mat4 transform = tc.GetTransform();

        // Snapping
        bool snap = Input::IsKeyPressed(OIL_KEY_LEFT_CONTROL);
        float snapValue = 0.5f;
        if(m_GizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 5.0f;

        float snapValues[3] = {snapValue, snapValue, snapValue};


        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

        if(ImGuizmo::IsUsing()){
            glm::vec3 translation, rotation, scale;
            Math::DecomposeTransform(transform, translation, rotation, scale);

            glm::vec3 deltaRotation = rotation - tc.Rotation;
            tc.Translation = translation;
            tc.Rotation += deltaRotation;
            tc.Scale = scale;

        }
    }

    ImGui::End();
    ImGui::PopStyleVar();
}
void EditorLayer::RenderStats()
{
    ImGui::Begin("Stats");

    auto stats = Renderer2D::GetStats();

    ImGui::Text("General Stats:");
    ImGui::Text("FPS: %.2f", fps);

    ImGui::Separator();
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

}
}