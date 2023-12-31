#include "pch/oilpch.h"
#include "OrthographicCameraController.h"

#include "oil/core/Input.h"
#include "oil/core/KeyCodes.h"

namespace oil{
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
        : m_AspectRatio(aspectRatio), m_Bounds({-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel}), m_Camera(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top), m_Rotation(rotation)
    {
    }
    void OrthographicCameraController::OnUpdate(Timestep dt)
    {
        if (Input::IsKeyPressed(OIL_KEY_A)){
            m_CameraPosition.x -= m_CameraTransSpeed * dt;
        }
        if (Input::IsKeyPressed(OIL_KEY_D)){
            m_CameraPosition.x += m_CameraTransSpeed * dt;
        }
        if (Input::IsKeyPressed(OIL_KEY_S)){
            m_CameraPosition.y -= m_CameraTransSpeed * dt;
        }
        if (Input::IsKeyPressed(OIL_KEY_W)){
            m_CameraPosition.y += m_CameraTransSpeed * dt;
        }

        if (m_Rotation){
            if (Input::IsKeyPressed(OIL_KEY_Q))
                m_CameraRotation += m_CameraRotSpeed * dt;    
            if (Input::IsKeyPressed(OIL_KEY_E))
                m_CameraRotation -= m_CameraRotSpeed * dt;

            m_Camera.SetRotation(m_CameraRotation);
        }

        m_Camera.SetPosition(m_CameraPosition);
    }
    void OrthographicCameraController::OnEvent(Event &e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(OIL_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(OIL_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
    }
   
    void OrthographicCameraController::OnResize(float width, float height)
    {
        m_AspectRatio = width / height;
        CalculateView();
    }
    void OrthographicCameraController::CalculateView()
    {
        m_Bounds = {-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel};
        m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
    }
    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent &e)
    {
        m_ZoomLevel -= e.GetYOffset() * 0.25f;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_CameraTransSpeed = 2.0f * m_ZoomLevel;
        CalculateView();   
        return false;
    }
    bool OrthographicCameraController::OnWindowResized(WindowResizeEvent &e)
    {
        OnResize((float)e.GetWidth(), (float)e.GetHeight());
        return false;
    }
}