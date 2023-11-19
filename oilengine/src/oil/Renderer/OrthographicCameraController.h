#pragma once


#include "oil/Renderer/OrthographicCamera.h"
#include "oil/core/Timestep.h"

#include "oil/events/ApplicationEvent.h"
#include "oil/events/MouseEvent.h"

namespace oil{

    struct OrthographicCameraBounds{
        float Left, Right;
        float Bottom, Top;

        float GetWidth() { return Right - Left; };
        float GetHeight() { return Top - Bottom; };
    };

    class OrthographicCameraController{
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = false);
    
        void OnUpdate(Timestep dt);
        void OnEvent(Event& e);

        OrthographicCamera& GetCamera() {return m_Camera; }
        const OrthographicCamera& GetCamera() const {return m_Camera; }

        void SetZoomLevel(float level) { m_ZoomLevel = level; CalculateView(); }
        float GetZoomLevel() const { return m_ZoomLevel; }

        void OnResize(float width, float height);

        void CalculateView();

        const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }
    private:
        bool OnMouseScrolled(MouseScrolledEvent& e);
        bool OnWindowResized(WindowResizeEvent& e);
    private:
        float m_AspectRatio;
        float m_ZoomLevel = 1.0f;
        OrthographicCameraBounds m_Bounds;
        OrthographicCamera m_Camera;

        bool m_Rotation;
        glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
        float m_CameraRotation = 0.0f;
        float m_CameraTransSpeed = 2.0f, m_CameraRotSpeed = 180.0f;
    };
}