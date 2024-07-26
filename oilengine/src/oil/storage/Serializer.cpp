#include "pch/oilpch.h"
#include "Serializer.h"

//File writing
#include <fstream>

//Required for extraction
#include "oil/Scene/Component.h"
#include "oil/Scene/Entity.h"
#include "AssetManager.h"


#include "oil/Scene/Scene.h"
#include "oil/Renderer/Model.h"
#include "oil/Renderer/Texture.h"
#include "oil/Renderer/Shader.h"
#include "oil/Renderer/Material.h"
#include "oil/Renderer/Shader.h"

//YAML Overwrites
    namespace YAML{

    //Emitter operator overloading
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

    //GLM
    template<>
    struct convert<glm::vec2>{
        static Node encode(const glm::vec2& rhs){
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs){
            if(!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };


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

       
    template<>
    struct convert<glm::mat3>{
        static Node encode(const glm::mat3& rhs){
            Node node;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    node.push_back(rhs[i][j]);
            return node;
        }

        static bool decode(const Node& node, glm::mat3& rhs){
            if(!node.IsSequence() || node.size() != rhs.length())
                return false;

            int k = 0;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    rhs[i][j] = node[k++].as<float>();

            return true;
        }
    };
    
    template<>
    struct convert<glm::mat4>{
        static Node encode(const glm::mat4& rhs){
            Node node;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    node.push_back(rhs[i][j]);
            return node;
        }

        static bool decode(const Node& node, glm::mat4& rhs){
            if(!node.IsSequence() || node.size() != rhs.length())
                return false;

            int k = 0;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    rhs[i][j] = node[k++].as<float>();

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

    template<>
    struct convert<oil::TextureFormat>{
        static Node encode(const oil::TextureFormat& rhs){
            Node node;
            node = (uint32_t)rhs;
            return node;
        }

        static bool decode(const Node& node, oil::TextureFormat& rhs){
            rhs = (oil::TextureFormat)node.as<uint32_t>();
            return true;
        }
    };

    //Data structures
    template<>
    struct convert<oil::AssetMetadata>{
        static Node encode(const oil::AssetMetadata& rhs){
            Node node;
            node.push_back(rhs.Type);
            node.push_back(rhs.AssetPath);
            node.push_back(rhs.SrcPath);
            return node;
        }

        static bool decode(const Node& node, oil::AssetMetadata& rhs){
            rhs.Type = (oil::ContentType)node[0].as<uint32_t>();
            rhs.AssetPath = node[1].as<std::string>();
            rhs.SrcPath = node[2].as<std::string>();
            return true;
        }
    };


}


namespace oil{

    //------------------------------------------------------------------------------
    //Serializer functions

    template<>
    void Serializer::SerializeAsset<Scene>(const Ref<Scene> scene, AssetMetadata metadata, UUID id)
    {
        YAML::Emitter out;
        std::ofstream fout(metadata.AssetPath);

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
    Ref<Scene> Serializer::DeserializeAssetYAML<Scene>(YAML::Node file)
    {
        Ref<Scene> scene = CreateRef<Scene>();

        YAML::Node data;
        if(!file["Body"]){
            if(!file["Scene"])
                return scene;
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
                    mc.model = AssetManager::GetAsset<Model>(mc.ID);
                }
            }
        }
        
        OIL_CORE_INFO("Scene deserialized successfully!");
        return scene;
    }

    template<>
    void Serializer::SerializeAsset<Model>(const Ref<Model> model, AssetMetadata metadata, UUID id)
    {

        std::ofstream fout(metadata.AssetPath);
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
       chunk["Materials"];
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
            meshrep["Material"] = mesh->GetMaterialIndex();
            chunk["Meshes"].push_back(meshrep);

            ptr++;
        }

        //Materials
        for (auto mat : model->GetMaterials()){
            chunk["Materials"].push_back(0u);
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
    Ref<Model> Serializer::DeserializeAssetYAML<Model>(YAML::Node file)
    {
        Ref<Model> model = CreateRef<Model>();
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
                return model;
            }
            auto vertView = file["Body"]["BufferViews"][bfview];
            if (vertView["Target"].as<uint32_t>() !=  34962){
                OIL_CORE_ERROR("Incorrect vertex buffer target!");
                return model;
            }
            layout.SetStride(vertView["Stride"].as<uint32_t>());

            Ref<Mesh> ms = CreateRef<Mesh>(&buffer[vertView["ByteOffset"].as<uint32_t>()], vertView["ByteLength"].as<uint32_t>(), 
                                &buffer[indView["ByteOffset"].as<uint32_t>()], indView["ByteLength"].as<uint32_t>(), layout);
            ms->SetMaterialIndex(meshInfo["Material"].as<uint32_t>());
            model->AddMesh(ms); 
        }
        //get materials
        YAML::Node materials = file["Body"]["Materials"];
        model->SetMaterialCount(materials.size());
        model->SetMaterialsToDefault(); //can't load materials just yet
        for (int i = 0; i < materials.size(); ++i){
            if(materials[i])
                model->SetMaterial(AssetRef<Material>(materials[i].as<UUID>()), i);
        }

        delete[bufferSize] buffer;
        return model;
       
    }

    template<>
    void Serializer::SerializeAsset<Texture2D>(const Ref<Texture2D> texture, AssetMetadata metadata, UUID id)
    {
        std::ofstream fout(metadata.AssetPath);
        
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
            OIL_CORE_ERROR("Failed to build YAML representation for texture!");
        }
        header.fileSize = utils::GetFileSize(fout);
        utils::WriteAssetHeader(fout, header);

        //Cleanup
        fout.close();

    }

    template<>
    Ref<Texture2D> Serializer::DeserializeAssetYAML<Texture2D>(YAML::Node file)
    {
        //Image metadata chunk
        uint32_t width, height;
        TextureFormat format;
        width = file["Body"]["Width"].as<uint32_t>();
        height = file["Body"]["Height"].as<uint32_t>();
        format = file["Body"]["Format"].as<TextureFormat>();

        Ref<Texture2D> texture = Texture2D::Create(width, height, format);
     
        texture->ResetMetadata(width, height, format);

        //Image data chunk

        YAML::Binary bin = file["Buffers"]["Data"].as<YAML::Binary>();

        const uint32_t bufferSize = file["Buffers"]["ByteLength"].as<uint32_t>();

        DataBuffer<unsigned char> imgData(bin.data(), bufferSize);

        texture->SetData(imgData);

        return texture;
       
    }

 template<>
    void Serializer::SerializeAsset<Shader>(const Ref<Shader> shader, AssetMetadata metadata, UUID id)
    {
        std::ofstream fout(metadata.AssetPath);
        
        YAML::Emitter out;
        out << YAML::BeginMap;
        //Header
        utils::AssetHeader header(id, ContentType::Shader, 0, OILENGINE_VERSION_MAJOR, OILENGINE_VERSION_MINOR);

        YAML::Node chunk;
        chunk["ShaderType"] = 1;
        chunk["ShaderLang"] = "OGL";
        chunk["Compiled"] = false;
        chunk["SRCPath"] = metadata.SrcPath;
       

        //Chunk 1
        out << YAML::Key << "Body";
        out << YAML::Value << chunk;

        //End
        out << YAML::EndMap;

        if(out.good()){
            OIL_CORE_INFO("Shader saved successfully!");
            utils::WriteAssetBody(fout, out);
        }else{
            OIL_CORE_ERROR("Failed to build YAML representation for shader!");
        }
        header.fileSize = utils::GetFileSize(fout);
        utils::WriteAssetHeader(fout, header);

        //Cleanup
        fout.close();
    }

    template<>
    Ref<Shader> Serializer::DeserializeAssetYAML<Shader>(YAML::Node file)
    {
        return Shader::Create(file["Body"]["SRCPath"].as<std::string>());
    }

 template<>
    void Serializer::SerializeAsset<Material>(const Ref<Material> material, AssetMetadata metadata, UUID id)
    {
        std::ofstream fout(metadata.AssetPath);
        
        YAML::Emitter out;
        out << YAML::BeginMap;
        //Header
        utils::AssetHeader header(id, ContentType::Material, 0, OILENGINE_VERSION_MAJOR, OILENGINE_VERSION_MINOR);

        YAML::Node chunk;

        YAML::Node uniforms;
        //build asset here
        for (auto uniform : material->GetUniformsOfType<int>())
            uniforms["int"][uniform.first] = uniform.second;
        for (auto uniform : material->GetUniformsOfType<float>())
            uniforms["float"][uniform.first] = uniform.second;
        for (auto uniform : material->GetUniformsOfType<glm::vec2>())
            uniforms["vec2"][uniform.first] = uniform.second;
        for (auto uniform : material->GetUniformsOfType<glm::vec3>())
            uniforms["vec3"][uniform.first] = uniform.second;
        for (auto uniform : material->GetUniformsOfType<glm::vec4>())
            uniforms["vec4"][uniform.first] = uniform.second;
        for (auto uniform : material->GetUniformsOfType<glm::mat3>())
            uniforms["mat3"][uniform.first] = uniform.second;
        for (auto uniform : material->GetUniformsOfType<glm::mat4>())
            uniforms["mat4"][uniform.first] = uniform.second;

        chunk["uniforms"] = uniforms;
        chunk["Shader"] = material->GetShader().GetHandle();

        //Chunk 1
        out << YAML::Key << "Body";
        out << YAML::Value << chunk;

        //End
        out << YAML::EndMap;

        if(out.good()){
            OIL_CORE_INFO("Material saved successfully!");
            utils::WriteAssetBody(fout, out);
        }else{
            OIL_CORE_ERROR("Failed to build YAML representation for material!");
        }
        header.fileSize = utils::GetFileSize(fout);
        utils::WriteAssetHeader(fout, header);

        //Cleanup
        fout.close();
    }

    template<>
    Ref<Material> Serializer::DeserializeAssetYAML<Material>(YAML::Node file)
    {
        Ref<Material> material = CreateRef<Material>();

        YAML::Node uniforms = file["Body"]["uniforms"];

        for (auto uniform : uniforms["int"])
            material->SetUniform<int>(uniform.first.as<std::string>(), uniform.second.as<int>());
        for (auto uniform : uniforms["float"])
            material->SetUniform<float>(uniform.first.as<std::string>(), uniform.second.as<float>());
        for (auto uniform : uniforms["vec2"])
            material->SetUniform<glm::vec2>(uniform.first.as<std::string>(), uniform.second.as<glm::vec2>());
        for (auto uniform : uniforms["vec3"])
            material->SetUniform<glm::vec3>(uniform.first.as<std::string>(), uniform.second.as<glm::vec3>());
        for (auto uniform : uniforms["vec4"])
            material->SetUniform<glm::vec4>(uniform.first.as<std::string>(), uniform.second.as<glm::vec4>());
        for (auto uniform : uniforms["mat3"])
            material->SetUniform<glm::mat3>(uniform.first.as<std::string>(), uniform.second.as<glm::mat3>());
        for (auto uniform : uniforms["mat4"])
            material->SetUniform<glm::mat4>(uniform.first.as<std::string>(), uniform.second.as<glm::mat4>());

        material->SetShader(AssetRef<Shader>(file["Body"]["Shader"].as<UUID>()));
        return material;
    }

    void Serializer::SerializeAssetRegistry(std::filesystem::path path, std::unordered_map<AssetHandle, AssetMetadata> dataTable)
    {
        utils::SaveHashmap<AssetHandle, AssetMetadata>(path, dataTable);
    }

    std::unordered_map<AssetHandle, AssetMetadata> Serializer::DeserializeAssetRegistry(std::filesystem::path path)
    {
        return utils::LoadHashmap<AssetHandle, AssetMetadata>(path);
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