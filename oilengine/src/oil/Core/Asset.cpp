#include <pch/oilpch.h>
#include "Asset.h"

//File writing
#include <fstream>

//Required for extraction
#include "oil/Scene/Component.h"
#include "oil/Scene/Entity.h"


namespace oil{
    //YAML operator overloading
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v){
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }
    
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v){
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    //Scene

    template <>
    void Asset<Scene>::Save()
    {
        Serializer::SerializeScene(m_AssetReference, m_AssetPath, m_ID);
    }

    template <>
    void Asset<Scene>::Load(YAML::Node file)
    {
        auto [ID, Dependencies] = Serializer::DeserializeScene(m_AssetReference, file);
        m_ID = ID;
        m_Dependencies = Dependencies;
    }

    template <>
    ContentType Asset<Scene>::GetType()
    {
        return ContentType::Scene;
    }
    

    //Model

    template <>
    void Asset<Model>::Save()
    {
        Serializer::SerializeModel(m_AssetReference, m_AssetPath, m_ID);
    }

    template <>
    void Asset<Model>::Load(YAML::Node file)
    {
        auto [ID, Dependencies] = Serializer::DeserializeModel(m_AssetReference, file);
        m_ID = ID;
        m_Dependencies = Dependencies;
    }

    template <>
    ContentType Asset<Model>::GetType()
    {
        return ContentType::Model;
    }

    //------------------------------------------------------------------------------
    //Serializer functions

    void Serializer::SerializeScene(const Ref<Scene> scene, std::filesystem::path path, UUID id)
    {
        YAML::Emitter out;

         out << YAML::BeginMap;
        //Header
        out << YAML::Key << "Header";
        out << YAML::BeginMap;
        out << YAML::Key << "Type";
        out << YAML::Value << (uint32_t)ContentType::Scene;
        out << YAML::Key << "FileVersion";
        out << YAML::Value << 1.00f;
        out << YAML::Key << "Length";
        out << YAML::Value << 0;
        out << YAML::Key << "UUID";
        out << YAML::Value << id;
        out << YAML::EndMap;

        //Body
        out << YAML::Key << "Body";
        out << YAML::BeginMap;
        out << YAML::Key << "Scene";
        out << YAML::Value << "Unnamed scene";
        out << YAML::Key << "Entities";
        out << YAML::Value << YAML::BeginSeq;
        scene->m_Registry.each([&](auto entityID){
            Entity entity = { entityID, scene.get() };
            if (!entity)
                return;
            
            SerializeEntity(out, entity);
        });
        out << YAML::EndSeq;
        out << YAML::EndMap;
        out << YAML::EndMap;


        std::ofstream fout(path);
        fout << out.c_str();
    }

    std::pair<UUID, std::unordered_set<UUID>> Serializer::DeserializeScene(const Ref<Scene> scene, YAML::Node file)
    {
        YAML::Node data;
        UUID id;
        std::unordered_set<UUID> deps;
        if(!file["Body"]){
            if(!file["Scene"])
                return {id, deps};
            data = file;
            id = 0; 
        }else{
            data = file["Body"];
            id = file["Header"]["UUID"].as<uint64_t>();
        }

        std::string sceneName = data["Scene"].as<std::string>();
        OIL_CORE_TRACE("Deserializing scene [{0}]", sceneName);

        auto entities = data["Entities"];
        if(entities){
            for (auto entity : entities){
                uint64_t uuid = entity["Entity"].as<uint64_t>();

                std::string name;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent)
                    name = tagComponent["Tag"].as<std::string>();

                OIL_CORE_TRACE("Deserialized entity with ID [{0}], name [{1}]", uuid, name);

                Entity deserializedEntity = scene->CreateEntityWithID(uuid, name);

                //Primitive components
                auto transformComponent = entity["TransformComponent"];
                if (transformComponent){
                    //Entities currently initialize with transforms
                    //TODO: change to support entities without transform

                    auto& tc = deserializedEntity.GetComponent<TransformComponent>();
                    tc.Translation = transformComponent["Translation"].as<glm::vec3>();
                    tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                    tc.Scale = transformComponent["Scale"].as<glm::vec3>();
                }

                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent){

                    auto& cc = deserializedEntity.AddComponent<CameraComponent>();

                    auto cameraProps = cameraComponent["Camera"];
                    cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

                    cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
                    cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                    cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

                    cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
                    cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
                    cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

                    cc.Primary = cameraComponent["Primary"].as<bool>();
                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();

                }

                auto spriteRendererComponent = entity["SpriteRendererComponent"];
                if(spriteRendererComponent){
                    auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
                    src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
                }

                auto meshComponent = entity["MeshComponent"];
                if(meshComponent){
                    auto& mrc = deserializedEntity.AddComponent<MeshComponent>();
                    mrc.mesh = Mesh::CreateCube();
                }

                //Asset components
                auto modelComponent = entity["ModelComponent"];
                if(modelComponent){
                    auto& mc = deserializedEntity.AddComponent<ModelComponent>();
                    mc.ID = entity["ModelComponent"]["ID"].as<UUID>();
                    deps.emplace(mc.ID);
                }
            }
        }
        
        OIL_CORE_INFO("Scene deserialized successfully!");
        return {id, deps};
    }

    void Serializer::SerializeModel(const Ref<Model> model, std::filesystem::path path, UUID id)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        //Header
        out << YAML::Key << "Header";
        out << YAML::BeginMap;
        out << YAML::Key << "Type";
        out << YAML::Value << (uint32_t)ContentType::Model;
        out << YAML::Key << "FileVersion";
        out << YAML::Value << 1.00f;
        out << YAML::Key << "Length";
        out << YAML::Value << 0;
        out << YAML::Key << "UUID";
        out << YAML::Value << id;
        out << YAML::EndMap;

        //Setup variables
        std::vector<Ref<Mesh>> meshes = model->GetMeshes();
        Ref<Mesh> mesh;
        
        uint32_t bufferOffset = 0, currentBufferSize =0, ptr = 0, modelSize = model->GetBufferSize();
        unsigned char* buffer = new unsigned char[modelSize];
        
        YAML::Node chunk;
       chunk["Nodes"];
       chunk["Meshes"];
       chunk["Accessors"];
       chunk["BufferViews"];
        
        //Processing loop
        uint32_t j = 0;
        for (int i = 0; i < model->GetMeshes().size(); ++i){

            //Build node
            YAML::Node node;
            node["Name"] = i == 0 ? "unnamed_node" : "root_node";
            node["Mesh"] = i;
            chunk["Nodes"].push_back(node);
            
            //Get the next mesh
            mesh = meshes[i];

            //Copy mesh vertex data to output buffer
            currentBufferSize = mesh->GetVertexBuffer()->GetSize();
            if(currentBufferSize > 0)
                memcpy(&buffer[bufferOffset], mesh->GetVertexBuffer()->GetData(), currentBufferSize);

            //Build vertex buffer view
            YAML::Node vertex;
            vertex["Buffer"] = 0;
            vertex["ByteLength"] = currentBufferSize;
            vertex["ByteOffset"] = bufferOffset;
            vertex["Target"] = 34962;
            vertex["Stride"] = baseLayout.GetStride();
            chunk["BufferViews"].push_back(vertex);
            bufferOffset += currentBufferSize;

            //Copy mesh index data to output buffer
            currentBufferSize = mesh->GetIndexBuffer()->GetSize();
            if(currentBufferSize > 0)
                memcpy(&buffer[bufferOffset], mesh->GetIndexBuffer()->GetData(), currentBufferSize);
            
            //Build index bufer view
            YAML::Node index;
            index["Buffer"] = 0;
            index["ByteLength"] = currentBufferSize;
            index["ByteOffset"] = bufferOffset;
            index["Target"] = 34963;
            chunk["BufferViews"].push_back(index);
            bufferOffset += currentBufferSize;
            
            //Build mesh attributes list from buffer layout
            YAML::Node attr;
            for(auto elem : baseLayout.GetElements()){
                YAML::Node accessor;
                accessor["BufferView"] = ptr;
                accessor["ElementType"] = elem.Type;
                accessor["Offset"] = elem.Offset;
                accessor["Normalized"] = elem.Normalized;
                chunk["Accessors"].push_back(accessor);
                attr[elem.Name] = j++;
            }
            //Build index buffer accessor
            YAML::Node accessor;
            accessor["BufferView"] = ++ptr;
            accessor["ElementType"] = ShaderDataType::Int;
            accessor["Offset"] = 0;
            accessor["Normalized"] = false;
            chunk["Accessors"].push_back(accessor);


            //Build mesh node
            YAML::Node meshrep;
            meshrep["Attributes"] = attr;
            meshrep["Indices"] = j++;
            meshrep["Material"] = "path";
            chunk["Meshes"].push_back(meshrep);

            ptr++;
        }

        //Chunk 1
        out << YAML::Key << "Body";
        out << YAML::Value << chunk;

        //Chunk 2
        out << YAML::Key << "Buffers";
        out << YAML::BeginMap;
        out << YAML::Key << "ByteLength";
        out << YAML::Value << modelSize;
        out << YAML::Key << "Data";
        out << YAML::Value << YAML::Binary(buffer, modelSize);
        out << YAML::EndMap;




        //End        
        out << YAML::EndMap;

        if(out.good()){
            OIL_CORE_INFO("Model saved successfully!");
            std::ofstream fout(path);
            fout << out.c_str();
        }else{
            OIL_CORE_ERROR("Failed to build YAML representation for model!");
        }
        delete buffer;

    }

    std::pair<UUID, std::unordered_set<UUID>> Serializer::DeserializeModel(const Ref<Model> model, YAML::Node file)
    {
        std::unordered_set<UUID> deps;
        if(!file["Header"] || !file["Body"] || !file["Buffers"] ){
            OIL_CORE_ERROR("File is not formatted properly!");
            return {0, deps};
        }
        if(!(file["Header"]["Type"].as<uint32_t>() == (uint32_t)ContentType::Model)){
            OIL_CORE_ERROR("File is not a model!");
            return {0, deps};
        }

        UUID id = file["Header"]["UUID"].as<uint64_t>();

        YAML::Binary bin = file["Buffers"]["Data"].as<YAML::Binary>();

        const uint32_t bufferSize = file["Buffers"]["ByteLength"].as<uint32_t>();
        unsigned char* buffer = new unsigned char[bufferSize];
        memcpy(&buffer[0], bin.data(), bufferSize);

        auto nodes = file["Body"]["Nodes"];
        for(auto node : nodes){
            //TODO: add mesh name
            auto meshInfo = file["Body"]["Meshes"][node["Mesh"].as<uint32_t>()];

            BufferLayout layout;
            BufferElement element;
            uint32_t stride = 0, bfview;
            for(auto attribute : meshInfo["Attributes"]){
                element.Name = attribute.first.as<std::string>();
                auto accessor = file["Body"]["Accessors"][attribute.second.as<uint32_t>()];
                element.Type = accessor["ElementType"].as<ShaderDataType>();
                element.Normalized = accessor["Normalized"].as<bool>();
                element.Offset = accessor["Offset"].as<uint32_t>();
                bfview = accessor["BufferView"].as<uint32_t>();

                layout.AddElement(element);
            }

            auto indAccessor = file["Body"]["Accessors"][meshInfo["Indices"].as<uint32_t>()];
            auto indView = file["Body"]["BufferViews"][indAccessor["BufferView"].as<uint32_t>()];

            if (indView["Target"].as<uint32_t>() !=  34963){
                OIL_CORE_ERROR("Incorrect index buffer target!");
                return {0, deps};
            }
            auto vertView = file["Body"]["BufferViews"][bfview];
            if (vertView["Target"].as<uint32_t>() !=  34962){
                OIL_CORE_ERROR("Incorrect vertex buffer target!");
                return {0, deps};
            }
            layout.SetStride(vertView["Stride"].as<uint32_t>());

            Ref<Mesh> ms = CreateRef<Mesh>(&buffer[vertView["ByteOffset"].as<uint32_t>()], vertView["ByteLength"].as<uint32_t>(), 
                                &buffer[indView["ByteOffset"].as<uint32_t>()], indView["ByteLength"].as<uint32_t>(), layout);
            model->AddMesh(ms); 
        }

        delete[bufferSize] buffer;
        return {id, deps};
       
    }

    //Serializer internal
    void Serializer::SerializeEntity(YAML::Emitter &out, Entity &entity)
    {
        OIL_ASSERT(entity.HasComponent<IDComponent>(), "Entity does not have a UUID");

        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity";
        out << YAML::Value << entity.GetUUID(); 

        if (entity.HasComponent<TagComponent>()){
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap; // TagComponent

            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;

            out << YAML::EndMap; // TagComponent
        }

        if (entity.HasComponent<TransformComponent>()){
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            auto& tc = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

            out << YAML::EndMap; // TransformComponent
        }

        if (entity.HasComponent<CameraComponent>()){
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent

            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            auto& camera = cameraComponent.Camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; //Camera
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
            out << YAML::EndMap; //Camera

            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

            out << YAML::EndMap; // CameraComponent
        }

        if (entity.HasComponent<SpriteRendererComponent>()){
            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap; // SpriteRendererComponent

            auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
            out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

            out << YAML::EndMap; // SpriteRendererComponent
        }

        //Asset components

        if (entity.HasComponent<ModelComponent>()){
            out << YAML::Key << "ModelComponent";
            out << YAML::BeginMap; // MeshComponent

            auto& modelComponent = entity.GetComponent<ModelComponent>();
            out << YAML::Key << "ID" << YAML::Value << modelComponent.ID;

            out << YAML::EndMap;
        }

        out << YAML::EndMap; // Entity
    }
}