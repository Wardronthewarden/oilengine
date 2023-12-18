#pragma once

#include "Camera.h"
#include "oil/core/Timestep.h"
#include "oil/events/Event.h"
#include "oil/events/MouseEvent.h"

#include <glm/glm.hpp>

namespace oil{

    class EditorCamera : public Camera{
    public:
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);
        EditorCamera() = default;

        void OnSceneInit(glm::vec3& position);
        void OnUpdate(Timestep dt);
        void OnEvent(Event& e);
        void OnFly(Timestep dt);

        inline float GetDistance() const { return m_Distance; }
        inline void setDistance(float distance) { m_Distance = distance; }

        inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        glm::mat4 GetVPMatrix() const { return m_Projection * m_ViewMatrix; }

        glm::vec3 GetUpDirection() const;
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetForwardDirection() const;
        const glm::vec3& GetPosition() const { return m_Position; }
        glm::quat GetOrientation() const;
        void SetInitialMousePosition(glm::vec2 mousePosition) { m_InitialMousePosition = mousePosition; }

        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }

        void SetFocalPoint(glm::vec3 focalPoint) { m_FocalPoint = focalPoint; }
        void OrbitAroundFocalPoint();
    private:
        void UpdateProjection();
        void UpdateView();

        bool OnMouseScroll(MouseScrolledEvent& e);

        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);

        glm::vec3 CalculateOrbitingPosition() const;

        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const { return 0.4f; }
        float ZoomSpeed() const;
    private:
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

        glm::mat4 m_ViewMatrix;
        glm::vec3 m_Position = { 0.0f, 0.0f, -100.0f };
        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

        glm::vec2 m_InitialMousePosition;

        float m_Distance = 10.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;

        float m_Speed = 10.0f;

        float m_ViewportWidth = 1280, m_ViewportHeight = 720;
    };

}