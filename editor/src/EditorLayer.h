#pragma once

#include <oil.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "oil/Renderer/EditorCamera.h"

namespace oil{

    class EditorLayer : public Layer{
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        
        virtual void OnUpdate(Timestep dt) override;
        virtual void OnImGuiRender() override;
        virtual void OnEvent(Event& event) override;
    private:
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnKeyReleased(KeyReleasedEvent& e);
        bool OnMousePressed(MouseButtonPressedEvent& e);
        bool OnMouseReleased(MouseButtonReleasedEvent& e);

        void NewScene();
        void OpenScene();
        void OpenScene(const std::filesystem::path& path);
        void SaveSceneAs();
        void SaveScene();

        void Import();
        void Reimport();

        void OnScenePlay();
        void OnSceneStop();

        // UI panels
        void UI_Toolbar();
        void RenderViewport();
        void RenderStats();
    private:
        OrthographicCameraController m_CameraController;

        Ref<FrameBuffer> m_FrameBuffer;

        Asset<Scene> m_ActiveScene;

        // Data
        std::string m_ActiveSceneFilepath;

        //Assets
        Ref<AssetManager> m_AssetManager;
        Ref<AssetImporter> m_AssetImporter;

        Ref<Texture2D> m_DefaultTexture;
        Ref<Texture2D> m_IconPlay;
        Ref<Texture2D> m_IconStop;

        //Profiling
        std::vector<ProfileResult> m_ProfileResults;
        float fps = 0;

        //Viewport
        glm::vec2 m_ViewportSize;
        EditorCamera m_EditorCamera;
        glm::vec2 m_ViewportBounds[2];

        bool m_ViewportFocused = false;
        Entity m_HoveredEntity;

        //Panels
        SceneHierarchyPanel m_SceneHierarchyPanel;
        ContentBrowserPanel m_ContentBrowserPanel;

        Asset<Model> m_ModelAssetTest;


        //Controls
        int m_GizmoType = -1;
        bool m_AltPressed = false, m_ControlPressed = false, m_ShiftPressed = false;

        enum class SceneState{
            Edit = 0, Play = 1
        };

        SceneState m_SceneState = SceneState::Edit;

    };
}