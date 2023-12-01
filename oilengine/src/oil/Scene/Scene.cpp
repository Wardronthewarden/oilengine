#include "pch/oilpch.h"
#include "Scene.h"

#include "Component.h"
#include "oil/Renderer/Renderer2D.h"
#include "Entity.h"

namespace oil{
    Scene::Scene()
    {

    }
    Scene::~Scene()
    {
    }

    Entity Scene::CreateEntity(const std::string &name)
    {
        entt::entity temp = m_Registry.create();
        Entity entity = {temp, this};
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        
        return entity;
    }

    void Scene::OnUpdate(Timestep dt)
    {
        //Update scripts
        {
            m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc){
                if (!nsc.Instance){
                    nsc.InstantiateFunction();
                    nsc.Instance->m_Entity = Entity{entity, this};
                    nsc.OnCreateFunction(nsc.Instance);
                }

                nsc.OnUpdateFunction(nsc.Instance, dt);
            });
        }


        // Render sprites
        Camera* mainCamera = nullptr;
        glm::mat4* cameraTransform = nullptr;
        auto view = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view){
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

            if (camera.Primary){
                mainCamera = &camera.Camera;
                cameraTransform = &transform.Transform;
                break;
            }
        }
        

        if (mainCamera){

            Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group){
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                Renderer2D::DrawQuad(transform, sprite.Color);
            }

            Renderer2D::EndScene();
        }
    }
    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        auto view = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view){
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio){
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }        
    }
}