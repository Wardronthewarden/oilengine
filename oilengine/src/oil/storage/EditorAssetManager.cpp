#include "pch/oilpch.h"
#include "EditorAssetManager.h"
#include "Asset.h"

//Required for extraction
#include "oil/Scene/Component.h"
#include "oil/Scene/Entity.h"
#include "oil/Renderer/Shader.h"

//should only be included in sesrializer.
#include "Serializer.h"


namespace oil{

    //Static member definitions
    template<>
    std::unordered_map<AssetHandle, Ref<Ref<Scene>>> EditorAssetManager::s_AssetLookup<Scene>;
    template<>
    std::unordered_map<AssetHandle, Ref<Ref<Model>>> EditorAssetManager::s_AssetLookup<Model>;
    template<>
    std::unordered_map<AssetHandle, Ref<Ref<Material>>> EditorAssetManager::s_AssetLookup<Material>;
    template<>
    std::unordered_map<AssetHandle, Ref<Ref<Shader>>> EditorAssetManager::s_AssetLookup<Shader>;
    template<>
    std::unordered_map<AssetHandle, Ref<Ref<Texture2D>>> EditorAssetManager::s_AssetLookup<Texture2D>;

    Ref<DragDropInfo> EditorAssetManager::s_DragDropInfo;
    std::filesystem::path EditorAssetManager::s_CurrentAssetPath;
    std::filesystem::path EditorAssetManager::s_RootAssetPath;
    std::filesystem::path EditorAssetManager::s_InternalAssetPath;
    std::filesystem::path EditorAssetManager::s_DataPath;

    std::unordered_map<AssetHandle, AssetMetadata> EditorAssetManager::s_AssetRegistry;
    std::unordered_map<std::string, AssetHandle> EditorAssetManager::s_AssetNameRegistry;
    


    Assimp::Importer AssetImporter::s_Importer;

    void EditorAssetManager::Init(std::string assetRootPath, std::filesystem::path internalPath)
    {
        s_RootAssetPath = assetRootPath;
        s_InternalAssetPath = internalPath/"Assets/";
        s_DataPath = internalPath/"Data/";


        s_CurrentAssetPath = s_RootAssetPath;
        OIL_CORE_INFO("{0}", s_CurrentAssetPath);
        s_DragDropInfo = CreateRef<DragDropInfo>();
        LoadAssetRegistry();
        ReimportInternalAssets();
        SaveAssetRegistry();

    }

    Ref<DragDropInfo> EditorAssetManager::OnDragAsset(const DragDropInfo& asset)
    {
        s_DragDropInfo->contentType = asset.contentType;
        s_DragDropInfo->itemPath = asset.itemPath;
        s_DragDropInfo->Handle = asset.Handle;
        return s_DragDropInfo;
    }

    Ref<DragDropInfo> EditorAssetManager::GetDragDropInfo()
    {
        return s_DragDropInfo;
    }     


    AssetHandle EditorAssetManager::GenerateAssetHandle(uint32_t maxTries)
    {
        if(maxTries == 0){
            return AssetHandle(0);
        }
        AssetHandle handle = AssetHandle();
        if(IsValid(handle))
            handle = GenerateAssetHandle(--maxTries);

        return handle;
    }

    #ifdef OIL_EDITOR
        #ifdef OIL_ASSET_IMPORTER
            void EditorAssetManager::ReimportInternalAssets()
            {
                //reimport internal assets
                std::filesystem::path internalSrc = s_InternalAssetPath/"src";
                AssetHandle handle;
                
                //Import shaders
                for(const auto it : std::filesystem::directory_iterator(internalSrc/"Shaders")){
                    if(s_AssetNameRegistry.find(it.path().stem().string()) == s_AssetNameRegistry.end()){
                        AssetHandle handle = CreateAsset<Shader>(it.path().stem().string(), Shader::Create(it.path().string()), s_InternalAssetPath/"Shaders", it.path());
                    }

                }
                //Import textures
                for(const auto it : std::filesystem::directory_iterator(internalSrc/"Textures")){
                    //Import any new Textures
                    if(s_AssetNameRegistry.find(it.path().stem().string()) == s_AssetNameRegistry.end())
                        AssetImporter::ImportImage(it.path().string(), s_InternalAssetPath/"Textures");
                        
                }
                //Create default materials
                if (s_AssetNameRegistry.find("DefaultMaterial") == s_AssetNameRegistry.end()){
                    AssetHandle shaderHandle = GetHandleByName("DefaultSurfaceShader");
                    CreateAsset<Material>("DefaultMaterial", CreateRef<Material>(GetAssetReference<Shader>(shaderHandle)), s_InternalAssetPath/"Materials");
                }
                for(const auto it : std::filesystem::directory_iterator(s_InternalAssetPath/"Scenes")){
                    //Load default scenes
                    if(s_AssetNameRegistry.find(it.path().stem().string()) == s_AssetNameRegistry.end()){
                        AssetHandle handle = GenerateAssetHandle(10);
                        s_AssetLookup<Scene>[handle] =  CreateRef<Ref<Scene>>(Serializer::DeserializeAsset<Scene>(it.path(), handle));
                        std::string name = it.path().stem().string();
                        RegisterAsset(handle);
                        SetPath(handle, (s_InternalAssetPath/"Scenes") / (name + ".oil"));
                        SetType(handle, ContentType::Scene);           
                    }

                }
            }
        #endif

    #endif

    template <>
    inline void EditorAssetManager::ReimportAsset<Shader>(AssetHandle handle)
    {
        OIL_CORE_ASSERT(s_AssetRegistry.contains(handle), "Trying to reimport asset that does not exist!");
        Ref<Shader> reference = Shader::Create(GetSource(handle).string());
        if (s_AssetLookup<Shader>.contains(handle)){
            s_AssetLookup<Shader>[handle].reset(&reference);
        }else{
            s_AssetLookup<Shader>[handle] = CreateRef<Ref<Shader>>(reference);
        }
}

    std::filesystem::path EditorAssetManager::CreateSource(std::filesystem::path dest, std::filesystem::path src)
    {   
        OIL_CORE_ASSERT(!dest.empty(), "No destination passed to create source file.");
        if (std::filesystem::exists(dest)){
            //generate new filename if it already exists
            std::filesystem::path parent = dest.parent_path();
            std::string filename = dest.stem().string(), extension = dest.extension().string();
            for(int i = 0; i < 1000; ++i){
                std::string temp = filename + std::to_string(i);
                if (!std::filesystem::exists(parent / (temp + extension))){
                    dest = parent / (temp + extension);
                    break;
                }
            }
        }
        if(src.empty()){
            std::ofstream {dest};
            return dest;
        }
        OIL_CORE_ASSERT(dest.extension() == src.extension(), "Source extension mismatch!");
        std::filesystem::copy(src, dest);
        return dest;
    }

    bool EditorAssetManager::IsValid(AssetHandle handle)
    {

        if (s_AssetRegistry.find(handle) != s_AssetRegistry.end())
            return true;
        OIL_CORE_WARN("AssetHandle {0} Is not valid", handle);
        return false;
    }

    AssetHandle EditorAssetManager::GetHandleByName(std::string name)
    {
        if(s_AssetNameRegistry.contains(name))
            return s_AssetNameRegistry[name];
        return 0;
    }

//Editor functions


    AssetMetadata EditorAssetManager::GetMetadata(AssetHandle handle)
    {
        return s_AssetRegistry[handle];
    }

    void EditorAssetManager::SetMetadata(AssetHandle handle, AssetMetadata metadata)
    {   
        //Needed for editor function, named assets and searching
        if (s_AssetRegistry.contains(handle)){
            if (s_AssetRegistry[handle].AssetPath != metadata.AssetPath){
                s_AssetNameRegistry.erase(s_AssetRegistry[handle].AssetPath.stem().string());
                s_AssetNameRegistry[metadata.AssetPath.stem().string()] = handle;
            }
        }
        s_AssetNameRegistry[metadata.AssetPath.stem().string()] = handle;

        //Set metadata here
        s_AssetRegistry[handle] = metadata;
    }

    void EditorAssetManager::RegisterAsset(AssetHandle handle)
    {
        s_AssetRegistry.emplace(handle, AssetMetadata());
    }

    void EditorAssetManager::SetPath(AssetHandle handle, std::filesystem::path path)
    {
        OIL_CORE_ASSERT(IsValid(handle), "Tried to set a path for an asset that does not exist yet!");
        std::string name;
        if(path.has_extension()){
            name = path.stem().string();
            SetName(handle, name);
            path.remove_filename();
        }
        if (s_AssetRegistry[handle].AssetPath.empty()){
            if (name == ""){
                OIL_CORE_ASSERT(false, "Asset path can not be initialized");
            }
            s_AssetRegistry[handle].AssetPath = path / (name + ".oil");
            return;
        }
        
        std::string filename = s_AssetRegistry[handle].AssetPath.filename().string();
        if(std::filesystem::exists(s_AssetRegistry[handle].AssetPath))
            std::filesystem::rename(s_AssetRegistry[handle].AssetPath, path / filename);
        s_AssetRegistry[handle].AssetPath = path / filename;
        SaveAssetRegistry();
        return;
        
        OIL_CORE_ERROR("Asset with Handle: {0} does not exist.", handle);
    }

    std::filesystem::path EditorAssetManager::GetPath(AssetHandle handle)
    {
        OIL_CORE_ASSERT(IsValid(handle), "Tried to get path of an asset that does not exist yet!");
        return s_AssetRegistry[handle].AssetPath;
    }

    void EditorAssetManager::SetType(AssetHandle handle, ContentType type)
    {
        OIL_CORE_ASSERT(IsValid(handle), "Tried to set a type for an asset that does not exist yet!");
        s_AssetRegistry[handle].Type = type;
        SaveAssetRegistry();
    }

    ContentType EditorAssetManager::GetType(AssetHandle handle)
    {
        OIL_CORE_ASSERT(IsValid(handle), "Tried to get type of an asset that does not exist yet!");
        return s_AssetRegistry[handle].Type;
    }

    void EditorAssetManager::SetSource(AssetHandle handle, std::filesystem::path src)
    {
        OIL_CORE_ASSERT(IsValid(handle), "Tried to set a source for an asset that does not exist yet!");
        s_AssetRegistry[handle].SrcPath = src;
        SaveAssetRegistry();
        
    }

    std::filesystem::path EditorAssetManager::GetSource(AssetHandle handle)
    {
        OIL_CORE_ASSERT(IsValid(handle), "Tried to get source of an asset that does not exist yet!");
        return s_AssetRegistry[handle].SrcPath;
    }

    void EditorAssetManager::RenameSource(AssetHandle handle, std::string &name)
    {
        OIL_CORE_ASSERT(IsValid(handle), "Tried to rename source of an asset that does not exist yet!");
        if(std::filesystem::exists(s_AssetRegistry[handle].SrcPath)){
            std::filesystem::path newPath =  s_AssetRegistry[handle].SrcPath.parent_path() / (name + s_AssetRegistry[handle].SrcPath.extension().string());
            std::filesystem::rename(s_AssetRegistry[handle].SrcPath, newPath);
            s_AssetRegistry[handle].SrcPath = newPath;
            SaveAssetRegistry();
        }
    }

    bool EditorAssetManager::SetName(AssetHandle handle, std::string& name)
    {
        OIL_CORE_ASSERT(IsValid(handle), "Tried to set a name for an asset that does not exist yet!");
        bool duplicateName;
        if(s_AssetNameRegistry.contains(name)){
            duplicateName = s_AssetNameRegistry[name] != handle;
            //Return if no name change occured
            if(!duplicateName) 
                return false;

            int i = 0;
            while(duplicateName){
                std::string temp = name + std::to_string(++i);
                if (!s_AssetNameRegistry.contains(temp)){
                    name = temp;
                    break;
                }
            }
        }
        s_AssetNameRegistry[name] = handle;
        SaveAssetRegistry();
        if(s_AssetRegistry[handle].AssetPath.empty())
            return true;

        if (s_AssetRegistry[handle].AssetPath.stem() != name){
            std::filesystem::path old = s_AssetRegistry[handle].AssetPath; 
            std::filesystem::path newPath = s_AssetRegistry[handle].AssetPath.parent_path() / (name + ".oil");
            if (std::filesystem::exists(old))
                std::filesystem::rename(old, newPath);
            s_AssetRegistry[handle].AssetPath = newPath;
            SaveAssetRegistry();
        }

        return true;
        
    }

    std::string EditorAssetManager::GetName(AssetHandle handle)
    {
        OIL_CORE_ASSERT(IsValid(handle), "Tried to get name of an asset that does not exist yet!");
        return s_AssetRegistry[handle].AssetPath.stem().string();
    }

    bool EditorAssetManager::StepIntoDirectory(std::filesystem::path path)
    {
        s_CurrentAssetPath /= path;
        if (std::filesystem::exists(s_CurrentAssetPath))
            return true;
        StepOutOfDirectory();
        return false;
    }

    bool EditorAssetManager::StepOutOfDirectory()
    {
        if (IsCurrentRootDirectory())
            return false;
        s_CurrentAssetPath = s_CurrentAssetPath.parent_path();
        return true;
    }

    void EditorAssetManager::LoadAssetRegistry()
    {
        s_AssetRegistry = Serializer::DeserializeAssetRegistry(s_DataPath/"assets.dat");
        std::string name;
        for(const auto& entry : s_AssetRegistry){
            name = entry.second.AssetPath.stem().string();
            SetName(entry.first, name);
        }
    }

    void EditorAssetManager::SaveAssetRegistry()
    {
        Serializer::SerializeAssetRegistry(s_DataPath/"assets.dat", s_AssetRegistry);
    }

//----------------------------------------------------------------------------------

     //Importer------------------------------------------------------------------------------------------------
    AssetHandle AssetImporter::ImportModel(std::filesystem::path src, std::filesystem::path dest)
    {
        if(dest.empty())
            dest = EditorAssetManager::GetCurrentDirectory();

        Ref<Model> mod = CreateRef<Model>();
        const aiScene *scene = s_Importer.ReadFile(src.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            OIL_CORE_ERROR("ERROR ASSIMP >> {0}", s_Importer.GetErrorString());
        }

        std::string name = src.filename().stem().string();
        ProcessNode(scene->mRootNode, scene, mod);

        return EditorAssetManager::CreateAsset<Model>(name, mod, dest, src);
    }

    AssetHandle AssetImporter::ImportImage(std::filesystem::path src, std::filesystem::path dest)
    {
        if(dest.empty())
            dest = EditorAssetManager::GetCurrentDirectory();

        OIL_CORE_INFO("importing to {0}", dest);

        stbi_set_flip_vertically_on_load(1);

        int width, height, channels;

        uint32_t meta = 0;
        uint32_t bpc = 0;
        std::string name = src.filename().stem().string();

        if(src.extension().string() == ".hdr"){
            meta |= 0x00000012;
            OIL_CORE_INFO("Importing hdr...");
            float* data = stbi_loadf(src.string().c_str(), &width, &height, &channels, 0);
            OIL_CORE_INFO("hdr Imported!");
            bpc = 4;
            OIL_CORE_ASSERT(data, "Failed to load hdr image!");
            TextureFormat form = (TextureFormat)((channels << 16) | meta);
            Ref<Texture2D> tex = Texture2D::Create(width, height, form);
            tex->SetData(data, width*height*channels*bpc);
            return EditorAssetManager::CreateAsset<Texture2D>(name, tex, dest, src);
        }else{
            meta |= 0x00000001;
            stbi_uc* data = stbi_load(src.string().c_str(), &width, &height, &channels, 0);
            bpc = 1;
            OIL_CORE_ASSERT(data, "Failed to load image!");
            TextureFormat form = (TextureFormat)((channels << 16) | meta);
            Ref<Texture2D> tex = Texture2D::Create(width, height, form);
            tex->SetData(data, width*height*channels*bpc);
            return EditorAssetManager::CreateAsset<Texture2D>(name, tex, dest, src);
        }



    }



    //Internal importer functions
    void AssetImporter::ProcessNode(aiNode *node, const aiScene *scene, Ref<Model> model)
    {
        //materials (maybe this is bad for multi model scenes)
        if(scene->HasMaterials())
            model->SetMaterialCount(scene->mNumMaterials);
        else
            model->SetMaterialCount(1);

        model->SetMaterialsToDefault();


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
            CreateRef<DataBuffer<unsigned char>>((unsigned char*)indices.data(), (uint32_t)indices.size() * sizeof(uint32_t)),
            mesh->mMaterialIndex
        );
    }

    
}