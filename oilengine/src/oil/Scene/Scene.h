#pragma once

#include "entt.hpp"
#include "oil/core/Timestep.h"

namespace oil {

    class Scene{
    public:
        Scene();
        ~Scene();

        //temp
        entt::entity CreateEntity();

        //TEMP
        entt::registry& Reg() { return m_Registry; }

        void OnUpdate(Timestep dt);
    private:
        entt::registry m_Registry;
    };

}