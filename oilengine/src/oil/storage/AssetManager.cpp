#include "pch/oilpch.h"
#include "AssetManager.h"

//Required for extraction
#include "oil/Scene/Component.h"
#include "oil/Scene/Entity.h"

namespace oil{

    //Instantiate template for new asset types that do not need to be specialized
    //TODO: there has to be a better way of doing this
    template Asset<Model> AssetManager::CreateAsset<Model>();
    template Asset<Scene> AssetManager::CreateAsset<Scene>();

    template void AssetManager::SaveAsset<Model>(UUID id);
    template void AssetManager::SaveAsset<Scene>(UUID id);
    template void AssetManager::SaveAsset<Texture2D>(UUID id);

    template void AssetManager::SaveAsset<Model>(Asset<Model> asset);
    template void AssetManager::SaveAsset<Scene>(Asset<Scene> asset);
    template void AssetManager::SaveAsset<Texture2D>(Asset<Texture2D> asset);


    //Static member definitions
    template<typename T>
    std::unordered_map<UUID, Ref<T>> AssetManager::s_AssetLookup;
    Ref<DragDropInfo> AssetManager::s_DragDropInfo;
    std::filesystem::path AssetManager::s_CurrentAssetPath;
    std::filesystem::path AssetManager::s_RootAssetPath;
    std::filesystem::path AssetManager::s_AssetLookupTablePath;
    YAML::Node AssetManager::s_AssetLookupTable;
    std::unordered_map<UUID, ContentType> AssetManager::s_LoadedIDs;

    Assimp::Importer AssetImporter::s_Importer;

    template <>
    void AssetManager::RefreshAsset<Scene>(UUID id)
    {
        Asset<Scene> sc = GetAsset<Scene>(id);
        sc->m_Registry.each([&](auto entityID){
            Entity entity = { entityID, sc.GetContent().get() };
            if(entity.HasComponent<ModelComponent>()){
                auto& mc = entity.GetComponent<ModelComponent>();
                if(mc.ID)
                    mc.model = GetAsset<Model>(mc.ID).GetContent();
            }
        });
    }

    template <>
    void AssetManager::RefreshAsset<Model>(UUID id)
    {
      
    }

    void AssetManager::Init(std::string assetRootPath, std::filesystem::path internalPath)
    {
        s_RootAssetPath = assetRootPath;
        s_AssetLookupTablePath = internalPath/"Scratch/assets.dat";


        s_CurrentAssetPath = s_RootAssetPath;
        OIL_CORE_INFO("{0}", s_CurrentAssetPath);
        s_DragDropInfo = CreateRef<DragDropInfo>();
        LoadAssetLookup();
    }

    Ref<DragDropInfo> AssetManager::OnDragAsset(const DragDropInfo& asset)
    {
        s_DragDropInfo->contentType = asset.contentType;
        s_DragDropInfo->itemPath = asset.itemPath;
        s_DragDropInfo->ID = asset.ID;
        return s_DragDropInfo;
    }

    Ref<DragDropInfo> AssetManager::GetDragDropInfo()
    {
        return s_DragDropInfo;
    }

    UUID AssetManager::LoadAsset(UUID id)
    {
        UUID check = 0;
        if(IsLoaded(id)){
            return id;
        }
        if(!IDExists(id)){
            OIL_CORE_ERROR("Tried to load asset with ID {0}, but ID does not exist!", id);
            return 0;
        }
        
        check = LoadAsset(GetPath(id));

        if(check != id)
            OIL_CORE_WARN("Asset held a different id from the library. The new ID is {0}", check);
        

        return check;
        
    }

    template <typename T>
    void AssetManager::SaveAsset(UUID id)
    {
        SaveAssetAs<T>(id, GetPath(id));
    }

    template <typename T>
    void AssetManager::SaveAsset(Asset<T> asset)
    {
        SaveAssetAs<T>(asset, GetPath(asset));
    }

    template <typename T>
    void AssetManager::SaveAssetAs(UUID id, std::filesystem::path path)
    {
        Serializer::Serialize<T>(s_AssetLookup<T>[id], GetPath(id), id);
    }

    template <typename T>
    void AssetManager::SaveAssetAs(Asset<T> asset, std::filesystem::path path)
    {
        Serializer::Serialize<T>(asset, path, asset);
    }

    UUID AssetManager::LoadAsset(std::filesystem::path path)
    {
        if(path.extension().string() != ".oil"){
            OIL_CORE_WARN("Tried loading asset {0}. Only internal files [.oil] are allowed!", path.string());
            return 0;
        }


        std::ifstream stream(path);
        utils::AssetHeader header = utils::ReadAssetHeader(stream);

        YAML::Node node = utils::ReadAssetBody(stream);

    
        switch(header.type){

            case oil::ContentType::Model:{
                LoadAssetInternal<Model>(node, path, header);
                break;
            }
            case oil::ContentType::Scene:{
                LoadAssetInternal<Scene>(node, path, header);
                break;             
            }
            default:
                OIL_CORE_WARN("Asset type does not exist!");
        }
        return header.ID;

    }

    void AssetManager::LoadDependencies(std::unordered_set<UUID> deps)
    {
        if (!deps.empty())
            for(auto dep : deps)
                LoadAsset(dep);
    }


    template <typename T>
    UUID AssetManager::AddToLookup(UUID id, Ref<T> asset)
    {
        s_AssetLookup<T>[id] = asset;
        s_LoadedIDs.emplace(id, Asset<T>::GetType());
        return id;
    }

    template <typename T>
    void AssetManager::LoadAssetInternal(YAML::Node &node, std::filesystem::path path, utils::AssetHeader header)
    {
        Ref<T> reference = CreateRef<T>();
        auto depIDs = Serializer::Deserialize<T>(reference, node);
        if(GetPath(header.ID) != path)
            s_LoadedIDs[AddEntry(path, header.ID)] = header.type;
        else
            s_LoadedIDs[header.ID] = header.type;
        AddToLookup<T>(header.ID, reference);
        LoadDependencies(depIDs);
        RefreshAsset<T>(header.ID);
    }


    bool AssetManager::StepIntoDirectory(std::filesystem::path path)
    {
        s_CurrentAssetPath /= path;
        if (std::filesystem::exists(s_CurrentAssetPath))
            return true;
        StepOutOfDirectory();
        return false;
    }

    bool AssetManager::StepOutOfDirectory()
    {
        if (IsCurrentRootDirectory())
            return false;
        s_CurrentAssetPath = s_CurrentAssetPath.parent_path();
        return true;
    }

    void AssetManager::LoadAssetLookup()
    {
        std::ifstream stream(s_AssetLookupTablePath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        s_AssetLookupTable = YAML::Load(strStream.str());
    }

    void AssetManager::SaveAssetLookup()
    {
        std::ofstream stream(s_AssetLookupTablePath);
        YAML::Emitter out;
        out << s_AssetLookupTable;
        stream << out.c_str();
    }

    UUID AssetManager::GenerateUUID(uint32_t maxTries)
    {
        if(maxTries == 0){
            return UUID(0);
        }
        UUID id = UUID();
        if(IDExists(id))
            id = GenerateUUID(--maxTries);

        return id;
    }

    UUID AssetManager::GetIDFromPath(std::filesystem::path path)
    {
        ContentType type;
        return GetIDFromPath(path, type);
    }

    UUID AssetManager::GetIDFromPath(std::filesystem::path path, ContentType &type)
    {
        if(path.extension().string() != ".oil"){
            OIL_CORE_WARN("Tried loading asset {0}. Only internal files [.oil] are allowed!", path.string());
            return 0;
        }

        UUID assetID;

        std::ifstream stream(path);

        UUID id;
        ContentType ty;
        stream.seekg(0, stream.beg); 
        stream.read(reinterpret_cast<char *>(&id), sizeof(UUID));
        stream.read(reinterpret_cast<char *>(&ty), sizeof(ContentType));
        type = ty;
        //Finish this
        return id;
    }


    ContentType AssetManager::GetAssetType(UUID id)
    {
        if(!IsLoaded(id))
            return ContentType::None;
        return s_LoadedIDs[id];
    }

    bool AssetManager::IDExists(UUID id)
    {

        if(s_AssetLookupTable[id])
            return true;
        OIL_CORE_WARN("Does not exist");
        return false;
    }

    std::filesystem::path AssetManager::GetPath(UUID id)
    {

        if(s_AssetLookupTable[id]){
            return s_AssetLookupTable[id].as<std::filesystem::path>();
        }
        return "";
    }

    UUID AssetManager::AddEntry(std::filesystem::path assetPath, UUID id)
    {
        if(s_AssetLookupTable[id] || !id)
            id = GenerateUUID(10);
        if(id){
            s_AssetLookupTable[id] = assetPath;
            SaveAssetLookup();
        }
        return id;
    }

    //Specialized asset creation
    template <>
    inline Asset<Texture2D> AssetManager::CreateAsset()
    {
        Ref<Texture2D> assetObject = Texture2D::Create(0, 0, TextureFormat::None);
        return CreateAsset<Texture2D>(s_CurrentAssetPath, assetObject);
    }

    template <>
    Asset<Texture2D> AssetManager::CreateAsset(Ref<Texture2D> assetObject)
    {
        return CreateAsset<Texture2D>(s_CurrentAssetPath, assetObject);
    }

    template <>
    Asset<Texture2D> AssetManager::CreateAsset(std::filesystem::path dir, std::string name)
    {
        Ref<Texture2D> assetObject = Texture2D::Create(0, 0, TextureFormat::None);
        return CreateAsset<Texture2D>(dir, assetObject, name);
    }

    //Generic Asset creation template
    template <typename T>
    inline Asset<T> AssetManager::CreateAsset()
    {
        Ref<T> assetObject = CreateRef<T>();
        return CreateAsset<T>(s_CurrentAssetPath, assetObject);
    }

    template <typename T>
    Asset<T> AssetManager::CreateAsset(Ref<T> assetObject)
    {
        return CreateAsset<T>(s_CurrentAssetPath, assetObject);
    }

    template <typename T>
    Asset<T> AssetManager::CreateAsset(std::filesystem::path dir, std::string name)
    {
        Ref<T> assetObject = CreateRef<T>();
        return CreateAsset<T>(dir, assetObject, name);
    }

    template <typename T>
    Asset<T> AssetManager::CreateAsset(std::filesystem::path dir, Ref<T> assetObject, std::string name)
    {
        if(dir.empty())
            return Asset<T>();
        


        std::filesystem::path filepath;
        if(dir.has_extension()){
            filepath = dir;
            filepath.replace_extension(".oil");
            dir.remove_filename();
        }else{
            filepath = dir / (name + ".oil");
        }
        std::string append = "";
        uint32_t i = 0;
        while (i<1000){
            if (std::filesystem::exists(filepath)){
                append = std::to_string(++i);
                filepath = dir / (name + append + ".oil");
            }else{
                break;
            }
        }

        UUID id = AddEntry(filepath);
        if(id == 0){
            OIL_CORE_ERROR("Failed to generate UUID for {0}", name);
            return Asset<T>();
        }

        Asset<T> asset = Asset<T>(id, assetObject);
        AddToLookup(id, assetObject);
        SaveAsset<T>(id);
        return asset;
    }

     //Importer
    UUID AssetImporter::ImportModel(std::filesystem::path src, std::filesystem::path dest)
    {
        if(dest.empty())
            dest = AssetManager::GetCurrentDirectory();

        Ref<Model> mod = CreateRef<Model>();
        const aiScene *scene = s_Importer.ReadFile(src.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            OIL_CORE_ERROR("ERROR ASSIMP >> {0}", s_Importer.GetErrorString());
        }

        std::string name = src.filename().stem().string();
        ProcessNode(scene->mRootNode, scene, mod);

        return AssetManager::CreateAsset<Model>(dest, mod, name);
    }

    UUID AssetImporter::ImportImage(std::filesystem::path src, std::filesystem::path dest)
    {
        if(dest.empty())
            dest = AssetManager::GetCurrentDirectory();

        OIL_CORE_INFO("importin to {0}", dest);

        //TODO
        int width, height, channels;

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(src.string().c_str(), &width, &height, &channels, 0);
        OIL_CORE_ASSERT(data, "Failed to load image!");
        TextureFormat form = (TextureFormat)((channels << 16) | 1);
        Ref<Texture2D> tex = Texture2D::Create(width, height, form);
        tex->SetData(data, width*height*channels);

        std::string name = src.filename().stem().string();

        return AssetManager::CreateAsset<Texture2D>(dest, tex, name);
    }



    //Internal importer functions
    void AssetImporter::ProcessNode(aiNode *node, const aiScene *scene, Ref<Model> model)
    {
        for(uint32_t i = 0; i < node->mNumMeshes; ++i){
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            model->AddMesh(ProcessMesh(mesh, scene));
        }

        for(uint32_t i = 0; i < node->mNumChildren; ++i){
            ProcessNode(node->mChildren[i], scene, model);
        }
    }
    Ref<Mesh> AssetImporter::ProcessMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<BaseVertex> vertices;
        std::vector<uint32_t> indices;


        for(uint32_t i = 0; i < mesh->mNumVertices; ++i){
            BaseVertex vertex;
            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f};
            vertex.Color = {1.0f, 1.0f, 1.0f, 1.0f};
            vertex.TexCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
            vertices.push_back(vertex);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; ++i){
            aiFace face = mesh->mFaces[i];
            for(uint32_t j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        return CreateRef<Mesh>(
            CreateRef<DataBuffer<unsigned char>>((unsigned char*)vertices.data(), (uint32_t)vertices.size() * sizeof(BaseVertex)), 
            CreateRef<DataBuffer<unsigned char>>((unsigned char*)indices.data(), (uint32_t)indices.size() * sizeof(uint32_t))
        );
    }

    
}