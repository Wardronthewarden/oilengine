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

    template <>
    ContentType Asset<Scene>::GetType()
    {
        return ContentType::Scene;
    }


    template <>
    ContentType Asset<Model>::GetType()
    {
        return ContentType::Model;
    }

    template <>
    ContentType Asset<Texture2D>::GetType()
    {
        return ContentType::Texture2D;
    }

    template <>
    ContentType Asset<Material>::GetType()
    {
        return ContentType::Material;
    }

    //------------------------------------------------------------------------------
    //Serializer functions

    template<>
    void Serializer::Serialize<Scene>(const Ref<Scene> scene, std::filesystem::path path, UUID id)
    {
        YAML::Emitter out;
        std::ofstream fout(path);

         out << YAML::BeginMap;
        //Header
        utils::AssetHeader header(id, ContentType::Scene, 0, OILENGINE_VERSION_MAJOR, OILENGINE_VERSION_MINOR);


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

        //WriteFile
        if(out.good()){
            OIL_CORE_INFO("Scene saved successfully!");
            utils::WriteAssetBody(fout, out);
        }else{
            OIL_CORE_ERROR("Failed to build YAML representation for scene!");
        }
        header.fileSize = utils::GetFileSize(fout);
        utils::WriteAssetHeader(fout, header);


        //Cleanup
        fout.close();
    }

    template<>
    std::unordered_set<UUID> Serializer::Deserialize<Scene>(const Ref<Scene> scene, YAML::Node file)
    {
        YAML::Node data;
        std::unordered_set<UUID> deps;
        if(!file["Body"]){
            if(!file["Scene"])
                return deps;
            data = file;
        }else{
            data = file["Body"];
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
        return deps;
    }

    template<>
    void Serializer::Serialize<Model>(const Ref<Model> model, std::filesystem::path path, UUID id)
    {

        std::ofstream fout(path);
        //Header
        utils::AssetHeader header(id, ContentType::Model, 0, OILENGINE_VERSION_MAJOR, OILENGINE_VERSION_MINOR);


        //YAML
        YAML::Emitter out;
        out << YAML::BeginMap;


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

        //WriteFile
        if(out.good()){
            OIL_CORE_INFO("Model saved successfully!");
            utils::WriteAssetBody(fout, out);
        }else{
            OIL_CORE_ERROR("Failed to build YAML representation for model!");
        }
        header.fileSize = utils::GetFileSize(fout);
        utils::WriteAssetHeader(fout, header);


        //Cleanup
        fout.close();
        delete buffer;

    }

    template<>
    std::unordered_set<UUID> Serializer::Deserialize<Model>(const Ref<Model> model, YAML::Node file)
    {
        std::unordered_set<UUID> deps;

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
                return deps;
            }
            auto vertView = file["Body"]["BufferViews"][bfview];
            if (vertView["Target"].as<uint32_t>() !=  34962){
                OIL_CORE_ERROR("Incorrect vertex buffer target!");
                return deps;
            }
            layout.SetStride(vertView["Stride"].as<uint32_t>());

            Ref<Mesh> ms = CreateRef<Mesh>(&buffer[vertView["ByteOffset"].as<uint32_t>()], vertView["ByteLength"].as<uint32_t>(), 
                                &buffer[indView["ByteOffset"].as<uint32_t>()], indView["ByteLength"].as<uint32_t>(), layout);
            model->AddMesh(ms); 
        }

        delete[bufferSize] buffer;
        return deps;
       
    }

    template<>
    void Serializer::Serialize<Texture2D>(const Ref<Texture2D> texture, std::filesystem::path path, UUID id)
    {
        std::ofstream fout(path);
        
        YAML::Emitter out;
        out << YAML::BeginMap;
        //Header
        utils::AssetHeader header(id, ContentType::Texture2D, 0, OILENGINE_VERSION_MAJOR, OILENGINE_VERSION_MINOR);

        //Setup variables

        DataBuffer<unsigned char> buff = texture->GetData();
        
        YAML::Node chunk;
        chunk["Width"] = texture->GetWidth();
        chunk["Height"] = texture->GetHeight();
        chunk["Format"] = texture->GetFormat();

        //Chunk 1
        out << YAML::Key << "Body";
        out << YAML::Value << chunk;

        //Chunk 2
        out << YAML::Key << "Buffers";
        out << YAML::BeginMap;
        out << YAML::Key << "ByteLength";
        out << YAML::Value << buff.GetSize();
        out << YAML::Key << "Data";
        out << YAML::Value << YAML::Binary(buff.GetData(), buff.GetSize());
        out << YAML::EndMap;




        //End
        out << YAML::EndMap;

        if(out.good()){
            OIL_CORE_INFO("Texture saved successfully!");
            utils::WriteAssetBody(fout, out);
        }else{
            OIL_CORE_ERROR("Failed to build YAML representation for model!");
        }
        header.fileSize = utils::GetFileSize(fout);
        utils::WriteAssetHeader(fout, header);

        //Cleanup
        fout.close();

    }

    template<>
    std::unordered_set<UUID> Serializer::Deserialize<Texture2D>(const Ref<Texture2D> texture, YAML::Node file)
    {
        std::unordered_set<UUID> deps;

        //Image metadata chunk
        uint32_t width, height;
        TextureFormat format;
        width = file["Body"]["Width"].as<uint32_t>();
        height = file["Body"]["Height"].as<uint32_t>();
        format = file["Body"]["Format"].as<TextureFormat>();

        texture->ResetMetadata(width, height, format);

        //Image data chunk

        YAML::Binary bin = file["Buffers"]["Data"].as<YAML::Binary>();

        const uint32_t bufferSize = file["Buffers"]["ByteLength"].as<uint32_t>();

        DataBuffer<unsigned char> imgData(bin.data(), bufferSize);

        texture->SetData(imgData);

        return deps;
       
    }




    //Serializer internal
    //--------------------------------------------------------------------------------------------------------------------------------------

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