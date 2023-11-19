#pragma once

#include "oil.h"

#include "ParticleSystem.h"

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
    oil::Ref<oil::Texture2D> m_SpriteSheet;
    oil::Ref<oil::SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree, m_TextureWater, m_TextureDirt;

    oil::Ref<oil::FrameBuffer> m_FrameBuffer;

    //Tile map
    std::unordered_map<char, oil::Ref<oil::SubTexture2D>> s_TextureMap;
    uint32_t m_MapWidth, m_MapHeight;

    glm::vec4 m_SquareColor = {0.2f, 0.3, 0.8f, 1.0f};
    mutable float rot = 0.0f;
    mutable float rot2 = 0.0f;

    std::vector<ProfileResult> m_ProfileResults;
    float fps = 0;

    ParticleSystem m_ParticleSystem;
    ParticleProps m_Particle;

};