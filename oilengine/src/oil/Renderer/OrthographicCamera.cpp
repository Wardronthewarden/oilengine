#include "pch/oilpch.h"
#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>


namespace oil{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : m_ViewMat(1.0f), m_ProjMat(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
    { 
        CalculateVP();
    }

    void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
    {
        m_ProjMat = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        CalculateVP();
    }

    glm::mat4 OrthographicCamera::CalculateVP()
    {
        glm::mat4 transform = glm::rotate(glm::translate(glm::mat4(1.0f), m_Position), glm::radians(m_Rotation), glm::vec3(0,0,1));

        m_ViewMat = glm::inverse(transform);

        m_VP = m_ProjMat * m_ViewMat;
        return m_VP;
    }
    void OrthographicCamera::CalculateViewMat()
    {

        CalculateVP();
    }
}