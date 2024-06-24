#pragma once

#include "oil/core/UUID.h"
    //Forward declarations
namespace YAML{
    class Node;
    class Emitter;
}
namespace oil{
    class std::filesystem::path;
    class Entity;

    //Serialization
    class Serializer{
    public:

        template<typename T>
        static void Serialize(const Ref<T> asset , std::filesystem::path path, UUID id);

        template<typename T>
        static Ref<T> Deserialize(YAML::Node file);

    private:
        static void SerializeEntity(YAML::Emitter& out, Entity& entity);
    };

}