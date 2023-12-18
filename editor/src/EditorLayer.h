#pragma once

#include <oil.h>
#include "Panels/SceneHierarchyPanel.h"
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
        void SaveSceneAs();
    private:
        OrthographicCameraController m_CameraController;
    
        //Temp
        Ref<VertexArray> m_SquareVA;
        Ref<Shader> m_FlatColorShader;

        Ref<Texture2D> m_DefaultTexture;
        Ref<Texture2D> m_SpriteSheet;
        Ref<SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree, m_TextureWater, m_TextureDirt;

        Ref<FrameBuffer> m_FrameBuffer;

        Ref<Scene> m_ActiveScene;
        Entity m_SquareEntity;
        Entity m_CameraEntity;

        //Tile map
        std::unordered_map<char, Ref<SubTexture2D>> s_TextureMap;
        uint32_t m_MapWidth, m_MapHeight;

        glm::vec4 m_SquareColor = {0.2f, 0.3, 0.8f, 1.0f};
        mutable float rot = 0.0f;
        mutable float rot2 = 0.0f;

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

        //Controls
        int m_GizmoType = -1;
        bool m_AltPressed = false, m_ControlPressed = false, m_ShiftPressed = false;

    };
}