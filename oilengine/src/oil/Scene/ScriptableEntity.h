#pragma once
#include "Entity.h"

namespace oil{

    class ScriptableEntity{
    public:
        virtual ~ScriptableEntity() {}

        template<typename T>
        T& GetComponent(){
            return m_Entity.GetComponent<T>();
        }

        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(Timestep dt) {}
    private:
        Entity m_Entity;
        friend class Scene;
    };
}