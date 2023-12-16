#pragma once

#include "oil/core/core.h"
#include "oil/core/Log.h"
#include "oil/Scene/Scene.h"
#include "oil/Scene/Entity.h"

namespace oil{

    class SceneHierarchyPanel{
    public:
    SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);

        void SetContext(const Ref<Scene>& context);

        void OnImGuiRender();

        Entity GetSelectedEntity() const { return m_SelectionContext; }
    private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
    private:
        Ref<Scene> m_Context;

        friend class Scene;
        Entity m_SelectionContext;
    };
}