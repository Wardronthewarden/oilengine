#pragma once

#include "oil/core/core.h"
#include "oil/core/Log.h"
#include "oil/core/AssetManager.h"
#include "oil/Scene/Scene.h"
#include "oil/Scene/Entity.h"

namespace oil{

    class SceneHierarchyPanel{
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);

        void SetContext(const Ref<Scene>& context);
        void SetAssetManagerReference(const Ref<AssetManager>& assetManager) { m_AssetManagerRef = assetManager; }

        void OnImGuiRender();

        Entity GetSelectedEntity() const { return m_SelectionContext; }
        void SetSelectedEntity(Entity entity);
    private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
    private:
        Ref<Scene> m_Context;
        Ref<AssetManager> m_AssetManagerRef;

        friend class Scene;
        Entity m_SelectionContext;
    };
}