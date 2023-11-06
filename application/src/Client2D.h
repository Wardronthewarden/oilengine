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

    oil::Ref<oil::Texture2D> m_DefaultTexture;

    glm::vec4 m_SquareColor = {0.2f, 0.3, 0.8f, 1.0f};
    mutable float rot = 0.0f;

};