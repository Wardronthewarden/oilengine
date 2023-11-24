#include "pch/oilpch.h"
#include "Scene.h"

#include "Component.h"

namespace oil{
    Scene::Scene()
    {

    }
    Scene::~Scene()
    {
    }
    entt::entity Scene::CreateEntity()
    {
        return m_Registry.create();
    }
    void Scene::OnUpdate(Timestep dt)
    {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group){
            auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

            Renderer2D::DrawQuad();
        }
    }
}