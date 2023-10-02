#pragma once

#include <glm/glm.hpp>

namespace oil{

class OrthographicCamera{
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        glm::mat4 CalculateVP();

        void SetPosition(const glm::vec3& position) {
            m_Position = position;
            CalculateVP();
        }
        const glm::vec3& GetPosition() const { return m_Position; }

        void SetRotation(float rotation) {
            m_Rotation = rotation;
            CalculateVP();
        }
        const float GetRotation() const { return m_Rotation; }

        const glm::mat4& GetProjectionMatrix() const { return m_ProjMat; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMat; }
        const glm::mat4& GetVPMatrix() const { return m_VP; }
    private:
        void CalculateViewMat();
    private:
        glm::mat4 m_ViewMat;
        glm::mat4 m_ProjMat;

        glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
        float m_Rotation = 0.0f;

        glm::mat4 m_VP;
    } ;

}