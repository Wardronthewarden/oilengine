#include "pch/oilpch.h"
#include "Scene.h"

#include "Component.h"
#include "oil/Renderer/Renderer2D.h"
#include "oil/Renderer/Renderer3D.h"
#include "Entity.h"
#include "ScriptableEntity.h"

namespace oil{
    Scene::Scene()
    {

    }
    Scene::~Scene()
    {
        m_Registry.clear();
    }

    Entity Scene::CreateEntity(const std::string &name)
    {
        return CreateEntityWithID(UUID(), name);
    }

    Entity Scene::CreateEntityWithID(UUID uuid, const std::string &name)
    {
        //entt::entity temp = m_Registry.create();
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }


    void Scene::OnUpdate(Timestep dt)
    {
        //Update scripts
        {
            m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc){
            //TODO: move to OnScenePlay
                if (!nsc.Instance){
                    nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->m_Entity = Entity{entity, this};
                    nsc.Instance->OnCreate();
                }

                nsc.Instance->OnUpdate(dt);
            });
        }


        // Render sprites
        Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform;
        auto view = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view){
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

            if (camera.Primary){
                mainCamera = &camera.Camera;
                cameraTransform = transform.GetTransform();
                break;
            }
        }
        

        if (mainCamera){

            if (m_Is2DScene){

                // 2D rendering
                Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

                auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
                for (auto entity : group){
                    auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                    Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
                }

                Renderer2D::EndScene(); 

            //-------------------------------------------------------------
            } else {

                // 3D Rendering

                Renderer3D::BeginScene(mainCamera->GetProjection(), cameraTransform);

                auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
                for (auto entity : group){
                    auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

                    Renderer3D::DrawMesh(transform.GetTransform(), mesh, (int)entity);
                }

                Renderer3D::EndScene();

            }
            
        }
    }
    void Scene::OnUpdateEditor(Timestep dt, EditorCamera &camera)
    {
        if (m_Is2DScene){

            // 2D scene rendering
            Renderer2D::BeginScene(camera);

                auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
                for (auto entity : group){
                    auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                    Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
                }

            Renderer2D::EndScene(); 

        //---------------------------------------------------------------------
        } else {

            //3D Rendering
            Renderer3D::BeginScene(camera);
               /*  {
                    auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
                    for (auto entity : group){
                        auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

                        Renderer3D::DrawMesh(transform.GetTransform(), mesh, (int)entity);
                    }
                } */
                {
                    auto group = m_Registry.group<TransformComponent>(entt::get<ModelComponent>);
                    for (auto entity : group){
                        auto [transform, model] = group.get<TransformComponent, ModelComponent>(entity);
                        if(!model.model)
                            continue;
                        for(auto mesh : model.model->GetMeshes())
                            Renderer3D::DrawMesh(transform.GetTransform(), *mesh.get(), (int)entity);
                    }
                }

            Renderer3D::EndScene();

            Renderer3D::InitLightingInfo();

            //Submit Lights
            Renderer3D::RenderLighting();

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

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view){
            const auto& camera = view.get<CameraComponent>(entity);
            if(camera.Primary)
                return Entity{entity, this};
            
        }
        return {};
    }

    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component){
        static_assert(sizeof(T) == 0, "Invalid component type!");
    }

    template<>
    void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component){
        
    }

    template<>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component){
        
    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component){
        component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
        OIL_CORE_INFO("Camera added");
    }

    template<>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component){

    }
    
    template<>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component){

    }
    
    template<>
    void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component){

    }
    
    template<>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component){

    }

    template<>
    void Scene::OnComponentAdded<ModelComponent>(Entity entity, ModelComponent& component){

    }
    
}