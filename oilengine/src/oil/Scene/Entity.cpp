#include "pch/oilpch.h"
#include "Entity.h"

namespace oil
{
    Entity::Entity(entt::entity handle, Scene *scene)
        : m_EntityHandle(handle), m_Scene(scene){

    }
} 
