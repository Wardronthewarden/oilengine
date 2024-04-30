#pragma once

#include <filesystem>
#include "core.h"

// Assimp
#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Asset.h"

namespace oil{


    struct DragDropInfo{
        ContentType contentType;
        std::filesystem::path itemPath;
        UUID ID;

        DragDropInfo() = default;
        DragDropInfo(ContentType type, const wchar_t* path, UUID id = 0)
            : contentType(type), itemPath(path), ID(id) {
            }
        DragDropInfo(DragDropInfo& rhs)
            :contentType(rhs.contentType), itemPath(rhs.itemPath){}

        ~DragDropInfo(){
        }

        
    };


    //Make this completely static
    class AssetManager{
        public:
            AssetManager() = default;
            AssetManager(std::string assetRootPath, std::filesystem::path internalPath) 
             : m_RootAssetPath(assetRootPath), m_CurrentAssetPath(assetRootPath), m_AssetLookupTablePath(internalPath/"Scratch/assets.dat") {};

            void Init();
            ~AssetManager(){SaveAssetLookup();}

            //Drag drop functions
            Ref<DragDropInfo> OnDragAsset(const DragDropInfo& asset);
            Ref<DragDropInfo> GetDragDropInfo();

            //New asset creation
            template<typename T>
            UUID CreateAsset();

            template<typename T>
            UUID CreateAsset(Ref<T> assetObject);

            template<typename T>
            UUID CreateAsset(std::filesystem::path dir, std::string name = "unnamed_asset");

            template<typename T>
            UUID CreateAsset(std::filesystem::path dir, Ref<T> assetObject, std::string name = "unnamed_asset");

            //Asset loading and unloadig
            UUID LoadAsset(UUID id);
            UUID LoadAsset(std::filesystem::path path);
            template<typename T>
            void LoadDependencies(UUID id);

            void UnloadAsset(UUID id);

            template<typename T>
            void SaveAsset(UUID id) {
                m_AssetLookup<T>[id]->Save();
            }

            //Getting assets
            template<typename T>
            Ref<T> GetAsset(UUID id)
            {
                UUID asset = LoadAsset(id);
                if (asset)
                    return m_AssetLookup<T>[asset]->GetContent();
                OIL_CORE_ERROR("Failed to get asset!");
                return nullptr;
                
            }
            ContentType GetAssetType(UUID id);

            template<typename T>
            void RefreshAsset(UUID id);

            bool IDExists(UUID id);

            //Project directory queries and functions
            std::filesystem::path GetRootDirectory() const { return m_RootAssetPath; }
            std::filesystem::path GetCurrentDirectory() const { return m_CurrentAssetPath; }
            void SetCurrentDirectory(std::filesystem::path path) { m_CurrentAssetPath = path; }
            bool StepIntoDirectory(std::filesystem::path path);
            bool StepOutOfDirectory();
            bool IsCurrentRootDirectory() const { return m_CurrentAssetPath == m_RootAssetPath; }

            //Get ID of asset at path
            UUID GetIDFromPath(std::filesystem::path path); 
            UUID GetIDFromPath(std::filesystem::path path, ContentType& type); 
        private:
            //Asset lookup table functions
            void LoadAssetLookup();
            void SaveAssetLookup();
            template<typename T>
            UUID AddToLookup(UUID id, Ref<Asset<T>> asset);

            //Util functions
            UUID GenerateUUID(uint32_t maxTries); //TODO: this is kinda dumb   
            inline bool IsLoaded(UUID id) const { return m_LoadedIDs.contains(id); };
            std::filesystem::path GetPath(UUID id);
            UUID AddEntry(std::filesystem::path assetPath, UUID id = 0);
            template<typename T>
            UUID LoadAssetInternal(YAML::Node& node, std::filesystem::path path, ContentType type);

        private:
            Ref<DragDropInfo> m_DragDropInfo;
            std::filesystem::path m_CurrentAssetPath;
            std::filesystem::path m_RootAssetPath;
            std::filesystem::path m_AssetLookupTablePath;

            YAML::Node m_AssetLookupTable;

            template<typename T>
            static std::unordered_map<UUID, Ref<Asset<T>>> m_AssetLookup;

            std::unordered_map<UUID, ContentType> m_LoadedIDs;
    };

    class AssetImporter{
        public:
            AssetImporter() = default;
            AssetImporter(Ref<AssetManager> assetManager) 
                : m_AssetManager(assetManager) {};


            UUID ImportModel(std::filesystem::path src, std::filesystem::path dest = "");

        private:
            void ProcessNode(aiNode *node, const aiScene* scene, Ref<Model> model);
            Ref<Mesh> ProcessMesh(aiMesh * mesh, const aiScene * scene);
            std::vector<Mesh> meshes;
            Assimp::Importer m_Importer;

        private:
            Ref<AssetManager> m_AssetManager;
            
    };


}