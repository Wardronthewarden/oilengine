#pragma once

#include "oil/core/UUID.h"
#include "oil/Renderer/EditorCamera.h"
#include "oil/core/Timestep.h"
#include "entt.hpp"
#include "oil/storage/Asset.h"


namespace oil {

    class Entity;

    class Scene{
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithID(UUID uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);

        void OnUpdate(Timestep dt);
        void OnUpdateEditor(Timestep dt, EditorCamera& camera);

        void OnViewportResize(uint32_t width, uint32_t height);

        Entity GetPrimaryCameraEntity();
    private:
        template<typename T>
        void OnComponentAdded(Entity entity, T& component);
    private:
        entt::registry m_Registry;

        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        bool m_Is2DScene = false;

        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
        friend class Serializer;
        friend class AssetManager;

    };

}