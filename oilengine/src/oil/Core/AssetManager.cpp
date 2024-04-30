#include "pch/oilpch.h"
#include "AssetManager.h"

//Required for extraction
#include "oil/Scene/Component.h"
#include "oil/Scene/Entity.h"


namespace oil{
    template<typename T>
    std::unordered_map<UUID, Ref<Asset<T>>> AssetManager::m_AssetLookup;

    //Instantiate template for new asset types
    template UUID AssetManager::CreateAsset<Model>(std::filesystem::path dir, Ref<Model> assetObject, std::string name);
    template UUID AssetManager::CreateAsset<Scene>(std::filesystem::path dir, Ref<Scene> assetObject, std::string name);

    template <>
    void AssetManager::RefreshAsset<Scene>(UUID id)
    {
        Ref<Scene> sc = GetAsset<Scene>(id);
        sc->m_Registry.each([&](auto entityID){
            Entity entity = { entityID, sc.get() };
            if(entity.HasComponent<ModelComponent>()){
                auto& mc = entity.GetComponent<ModelComponent>();
                if(mc.ID)
                    mc.model = GetAsset<Model>(mc.ID);
            }
        });
    }

    template <>
    void AssetManager::RefreshAsset<Model>(UUID id)
    {
        
    }

    void AssetManager::Init()
    {
        m_CurrentAssetPath = m_RootAssetPath;
        OIL_CORE_INFO("{0}", m_CurrentAssetPath);
        m_DragDropInfo = CreateRef<DragDropInfo>();
        LoadAssetLookup();
    }

    Ref<DragDropInfo> AssetManager::OnDragAsset(const DragDropInfo& asset)
    {
        m_DragDropInfo->contentType = asset.contentType;
        m_DragDropInfo->itemPath = asset.itemPath;
        m_DragDropInfo->ID = asset.ID;
        return m_DragDropInfo;
    }

    Ref<DragDropInfo> AssetManager::GetDragDropInfo()
    {
        return m_DragDropInfo;
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


    //Add new asset types here
    UUID AssetManager::LoadAsset(std::filesystem::path path)
    {
        if(path.extension().string() != ".oil"){
            OIL_CORE_WARN("Tried loading asset {0}. Only internal files [.oil] are allowed!", path.string());
            return 0;
        }

        UUID assetID;

        std::ifstream stream(path);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node node = YAML::Load(strStream.str());

        if(!node["Header"]["Type"]){
            OIL_CORE_ERROR("File {0} formatted incorrectly [no type information]", path.string());
            return 0;
        }

        ContentType type = node["Header"]["Type"].as<ContentType>();
        switch(type){
            case oil::ContentType::Model:{
                assetID = LoadAssetInternal<Model>(node, path, ContentType::Model);
                RefreshAsset<Model>(assetID);
                break;
            }
            case oil::ContentType::Scene:{
                assetID = LoadAssetInternal<Scene>(node, path, ContentType::Scene);
                RefreshAsset<Scene>(assetID);
                break;             
            }
            default:
                OIL_CORE_WARN("Asset type does not exist!");
        }
        return assetID;

    }

    template<typename T>
    void AssetManager::LoadDependencies(UUID id)
    {
        std::unordered_set<UUID> deps = m_AssetLookup<T>[id]->GetDependencies();
        if (!deps.empty())
            for(auto dep : deps)
                LoadAsset(dep);
    }


    template <typename T>
    UUID AssetManager::AddToLookup(UUID id, Ref<Asset<T>> asset)
    {
        m_AssetLookup<T>[id] = asset;
        m_LoadedIDs.emplace(id, asset->GetType());
        return id;
    }

    template <typename T>
    UUID AssetManager::LoadAssetInternal(YAML::Node &node, std::filesystem::path path, ContentType type)
    {
        Ref<T> ass = CreateRef<T>();
        Ref<Asset<T>> asset = CreateRef<Asset<T>>(ass, path);
        asset->Load(node);
        UUID assetID = asset->GetID();
        if(GetPath(assetID) != path)
            m_LoadedIDs[AddEntry(path, assetID)] = type;
        else
            m_LoadedIDs[assetID] = type;
        AddToLookup<T>(assetID, asset);
        LoadDependencies<T>(assetID);
        return assetID;
    }


    bool AssetManager::StepIntoDirectory(std::filesystem::path path)
    {
        m_CurrentAssetPath /= path;
        if (std::filesystem::exists(m_CurrentAssetPath))
            return true;
        StepOutOfDirectory();
        return false;
    }

    bool AssetManager::StepOutOfDirectory()
    {
        if (IsCurrentRootDirectory())
            return false;
        m_CurrentAssetPath = m_CurrentAssetPath.parent_path();
        return true;
    }

    void AssetManager::LoadAssetLookup()
    {
        std::ifstream stream(m_AssetLookupTablePath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        m_AssetLookupTable = YAML::Load(strStream.str());
    }

    void AssetManager::SaveAssetLookup()
    {
        std::ofstream stream(m_AssetLookupTablePath);
        YAML::Emitter out;
        out << m_AssetLookupTable;
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
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node node = YAML::Load(strStream.str());

        if(!node["Header"]["Type"]){
            OIL_CORE_ERROR("File {0} formatted incorrectly [no type information]", path.string());
            return 0;
        }

        if(!node["Header"]["UUID"]){
            OIL_CORE_ERROR("File {0} formatted incorrectly [no UUID]", path.string());
            return 0;
        }

        type = node["Header"]["Type"].as<ContentType>();
        return node["Header"]["UUID"].as<UUID>();
    }


    ContentType AssetManager::GetAssetType(UUID id)
    {
        if(!IsLoaded(id))
            return ContentType::None;
        return m_LoadedIDs[id];
    }

    bool AssetManager::IDExists(UUID id)
    {

        if(m_AssetLookupTable[id])
            return true;
        OIL_CORE_WARN("Does not exist");
        return false;
    }

    std::filesystem::path AssetManager::GetPath(UUID id)
    {

        if(m_AssetLookupTable[id]){
            return m_AssetLookupTable[id].as<std::filesystem::path>();
        }
        return "";
    }

    UUID AssetManager::AddEntry(std::filesystem::path assetPath, UUID id)
    {
        if(m_AssetLookupTable[id] || !id)
            id = GenerateUUID(10);
        if(id){
            m_AssetLookupTable[id] = assetPath;
            SaveAssetLookup();
        }
        return id;
    }

    template <>
    inline UUID AssetManager::CreateAsset<Model>()
    {
        Ref<Model> assetObject = CreateRef<Model>();
        return CreateAsset<Model>(m_CurrentAssetPath, assetObject);
    }

    template <>
    inline UUID AssetManager::CreateAsset<Scene>()
    {
        Ref<Scene> assetObject = CreateRef<Scene>();
        return CreateAsset<Scene>(m_CurrentAssetPath, assetObject);
    }

    template <typename T>
    UUID AssetManager::CreateAsset(Ref<T> assetObject)
    {
        return CreateAsset<T>(m_CurrentAssetPath, assetObject);
    }

    template <typename T>
    UUID AssetManager::CreateAsset(std::filesystem::path dir, std::string name)
    {
        Ref<T> assetObject = CreateRef<T>();
        return CreateAsset<T>(dir, assetObject, name);
    }

    template <typename T>
    UUID AssetManager::CreateAsset(std::filesystem::path dir, Ref<T> assetObject, std::string name)
    {
        if(dir.empty())
            return 0;
        
        Ref<Asset<T>> asset = CreateRef<Asset<T>>(assetObject);

        std::filesystem::path filepath;
        if(dir.has_filename()){
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
            return 0;
        }

        asset->SetPath(filepath);
        asset->SetID(id);
        asset->Save();

        AddToLookup(id, asset);
        return id;
    }

    UUID AssetImporter::ImportModel(std::filesystem::path src, std::filesystem::path dest)
    {
        if(dest.empty())
            dest = m_AssetManager->GetCurrentDirectory();

        Ref<Model> mod = CreateRef<Model>();
        const aiScene *scene = m_Importer.ReadFile(src.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            OIL_CORE_ERROR("ERROR ASSIMP >> {0}", m_Importer.GetErrorString());
        }

        std::string directory = src.parent_path().string(), name = src.filename().stem().string();
        ProcessNode(scene->mRootNode, scene, mod);

        return m_AssetManager->CreateAsset<Model>(directory, mod, name);
    }
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
            CreateRef<CharBuffer>((unsigned char*)vertices.data(), (uint32_t)vertices.size() * sizeof(BaseVertex)), 
            CreateRef<CharBuffer>((unsigned char*)indices.data(), (uint32_t)indices.size() * sizeof(uint32_t))
        );
    }
}