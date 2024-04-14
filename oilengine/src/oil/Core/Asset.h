#pragma once
#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "oil/Scene/Scene.h"
#include "oil/Renderer/Model.h"

//This file contains wrappers around in engine constructs that specifies loading and saving them as assets
//All types of assets will be included in this file going forward, and should be included from here
namespace oil{
    enum class ContentType{
        None        = 0,
        Directory   = 0x00444952, //DIR
        Scene       = 0x53434e45, //SCNE
        Model       = 0x4d4f444c, //MODL
        Material    = 0x004d4154, //MAT
        Texture     = 0x00544558, //TEX
        VFX         = 0x00564658, //VFX
        Sound       = 0x00534658, //SFX
        Widget      = 0x57494447, //WIDG
        Template    = 0x54454d50, //TEMP
        Custom      = 0x43555354  //CUST
    };

    template<typename T>
    class Asset{
    public:
        Asset(){};
        Asset(const Ref<T> ref)
            : m_AssetReference(ref) {};
        Asset(std::filesystem::path path)
            : m_AssetPath(path) {};
        Asset(const Ref<T> ref, std::filesystem::path path)
            : m_AssetPath(path), m_AssetReference(ref) {};
        ~Asset() {};

        void SetPath(std::filesystem::path path) { m_AssetPath = path; }
        std::filesystem::path GetPath() { return m_AssetPath; }
        void SetID(UUID id) { m_ID = id; }
        UUID GetID() const { return m_ID; }

        //Maybe unload asset
        void Load();
        void Save();
        void SaveAs(std::filesystem::path path){
            SetPath(path);
            Save();
        };
        Ref<T> GetContent() {return m_AssetReference; };

    private:
        std::filesystem::path m_AssetPath;
        UUID m_ID;
        Ref<T> m_AssetReference;
    };
    

    //Serialization

    class Serializer{
    public:
        //Scene
        static void SerializeScene(const Ref<Scene> scene, std::filesystem::path path, UUID id);
        static UUID DeserializeScene(const Ref<Scene> scene, std::filesystem::path path);

        //Model
        static void SerializeModel(const Ref<Model> model, std::filesystem::path path, UUID id);
        static UUID DeserializeModel(const Ref<Model> model, std::filesystem::path path);

    private:
        static void SerializeEntity(YAML::Emitter& out, Entity& entity);
    };

}
    //YAML encoding helpers
namespace YAML {
    //GLM
    template<>
    struct convert<glm::vec3>{
        static Node encode(const glm::vec3& rhs){
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs){
            if(!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
    
    template<>
    struct convert<glm::vec4>{
        static Node encode(const glm::vec4& rhs){
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs){
            if(!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    //Data types
    template<>
    struct convert<oil::ShaderDataType>{
        static Node encode(const oil::ShaderDataType& rhs){
            Node node;
            node = (uint32_t)rhs;
            return node;
        }

        static bool decode(const Node& node, oil::ShaderDataType& rhs){
            rhs = (oil::ShaderDataType)node.as<uint32_t>();
            return true;
        }
    };

    template<>
    struct convert<oil::ContentType>{
        static Node encode(const oil::ContentType& rhs){
            Node node;
            node = (uint32_t)rhs;
            return node;
        }

        static bool decode(const Node& node, oil::ContentType& rhs){
            rhs = (oil::ContentType)node.as<uint32_t>();
            return true;
        }
    };

    //Native types
    template<>
    struct convert<oil::UUID>{
        static Node encode(const oil::UUID& rhs){
            Node node;
            node = (uint64_t)rhs;
            return node;
        }

        static bool decode(const Node& node, oil::UUID& rhs){
            rhs = node.as<uint64_t>();
            return true;
        }
    };


    //cpp overwrites
    template<>
    struct convert<std::filesystem::path>{
        static Node encode(const std::filesystem::path& rhs){
            Node node;
            node = rhs.string();
            return node;
        }

        static bool decode(const Node& node, std::filesystem::path& rhs){
            rhs = node.as<std::string>();
            return true;
        }
    };


}

