#pragma once

#include "oil.h"

class Client2D : public oil::Layer{
public:
    Client2D();
    virtual ~Client2D() = default;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    
    virtual void OnUpdate(oil::Timestep dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(oil::Event& event) override;
private:
    oil::OrthographicCameraController m_CameraController;
    
    //Temp
    oil::Ref<oil::VertexArray> m_SquareVA;
    oil::Ref<oil::Shader> m_FlatColorShader;

    glm::vec4 m_SquareColor = {1.0f, 0.3, 0.8f, 1.0f};

};