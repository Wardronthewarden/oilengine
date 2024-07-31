#pragma once

#include "SceneCamera.h"
#include "oil/core/UUID.h"
#include "oil/Renderer/Texture.h"
#include "oil/Renderer/Model.h"
#include "oil/Renderer/Material.h"
#include "oil/Renderer/Light.h"
#include "oil/storage/AssetManager.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace oil{

    //Data components
    struct IDComponent{
        UUID ID;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
        IDComponent(const UUID& id)
            : ID(id){};
    };

    struct TagComponent{
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) 
            : Tag(tag){};
    };

    //Engine primitive components
    struct TransformComponent{
        glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation) 
            : Translation(translation){};

        glm::mat4 GetTransform() const {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
            
            return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }

    };

    struct SpriteRendererComponent{
        glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
        Ref<Texture2D> Texture;
        float TilingFactor = 1.0f;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        SpriteRendererComponent(const glm::vec4& color) 
            : Color(color){};

        operator glm::vec4& () { return Color; }
        operator const glm::vec4& () const { return Color; }
    };

    struct CameraComponent{
        SceneCamera Camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };


    struct MeshComponent{
        Ref<Mesh> mesh;

        MeshComponent() = default;
        MeshComponent(const MeshComponent&) = default;
    };


    struct PointLightComponent{
        PointLight light;
        
        PointLightComponent() = default;
        PointLightComponent(const PointLightComponent&) = default;
    };

    struct SpotLightComponent{
        SpotLight light;
        
        SpotLightComponent() = default;
        SpotLightComponent(const SpotLightComponent&) = default;
    };

    struct DirecLightComponent{
        DirectionalLight light;
        
        DirecLightComponent() = default;
        DirecLightComponent(const DirecLightComponent&) = default;
    };

    //Asset components
    struct ModelComponent{
        AssetRef<Model> model;
        std::vector<AssetHandle> Materials;

        ModelComponent() = default;
        ModelComponent(const ModelComponent&) = default;

        void ResetMaterial(uint32_t index){
            Materials[index] = model->GetMaterial(index);
        }

        void SetModel(AssetRef<Model> newModel){
            model = newModel;
            Materials.resize(newModel->GetMaterialCount());
            for (int i = 0; i < Materials.size(); ++i)
                ResetMaterial(i);
        }
    };


    //Native scripting
    class ScriptableEntity;

    struct NativeScriptComponent{
        ScriptableEntity* Instance = nullptr;

        

        ScriptableEntity*(*InstantiateScript)() ;
        void (*DestroyScript)(NativeScriptComponent*);


        template<typename T>
        void Bind(){
            InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
            DestroyScript = [](NativeScriptComponent* nsc) { delete (T*)nsc->Instance; nsc->Instance = nullptr; };
        }
    };



}