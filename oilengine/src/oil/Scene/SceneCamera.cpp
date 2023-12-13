#include "pch/oilpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace oil{
    SceneCamera::SceneCamera()
    {
        RecalculateProjection();
    }

    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Orthographic;
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;
        RecalculateProjection();
    }
    void SceneCamera::SetPerspective(float verticalFov, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Perspective;
        m_PerspectiveFOV = verticalFov;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;
        RecalculateProjection();
    }
    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = (float)width / (float)height;
        RecalculateProjection();
        
    }
    void SceneCamera::RecalculateProjection()
    {
        float orthoLeft, orthoRight, orthoBottom, orthoTop;
        switch(m_ProjectionType){
            case ProjectionType::Orthographic:
                orthoLeft =  -m_OrthographicSize * 0.5 * m_AspectRatio;
                orthoRight =  m_OrthographicSize * 0.5 * m_AspectRatio;
                orthoBottom =  -m_OrthographicSize * 0.5;
                orthoTop =  m_OrthographicSize * 0.5;

                m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
                break;
            case ProjectionType::Perspective:
                m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
                break;

        }
    }
}