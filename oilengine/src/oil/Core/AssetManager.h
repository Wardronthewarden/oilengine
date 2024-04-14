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

    class AssetManager{
        public:
            AssetManager() = default;
            AssetManager(std::string assetRootPath, std::filesystem::path internalPath) 
             : m_RootAssetPath(assetRootPath), m_CurrentAssetPath(assetRootPath), m_AssetLookupTable(internalPath/"Scratch/assets.dat") {};

            void Init();

            Ref<DragDropInfo> OnDragAsset(const DragDropInfo& asset);
            Ref<DragDropInfo> GetDragDropInfo();

            template<typename T>
            UUID CreateAsset();

            template<typename T>
            UUID CreateAsset(Ref<T> assetObject);

            template<typename T>
            UUID CreateAsset(std::filesystem::path dir, std::string name = "unnamed_asset");

            template<typename T>
            UUID CreateAsset(std::filesystem::path dir, std::string name, Ref<T> assetObject);


            UUID LoadAsset(UUID id);
            UUID LoadAsset(std::filesystem::path path);

            void UnloadAsset(UUID id);

            Ref<Model> GetModel(UUID id);
            bool IDExists(UUID id);


            std::filesystem::path GetRootDirectory() const { return m_RootAssetPath; }
            std::filesystem::path GetCurrentDirectory() const { return m_CurrentAssetPath; }
            void SetCurrentDirectory(std::filesystem::path path) { m_CurrentAssetPath = path; }
            bool StepIntoDirectory(std::filesystem::path path);
            bool StepOutOfDirectory();
            bool IsCurrentRootDirectory() const { return m_CurrentAssetPath == m_RootAssetPath; }

            UUID GetIDFromPath(std::filesystem::path path); 
            UUID GetIDFromPath(std::filesystem::path path, ContentType& type); 
        private:
            void LoadAssetLookup();
            void SaveAssetLookup();


            UUID GenerateUUID(uint32_t maxTries); //TODO: this is kinda dumb   
            inline bool IsLoaded(UUID id) const { return m_LoadedIDs.contains(id); };
            std::filesystem::path GetPath(UUID id);
            UUID AddEntry(std::filesystem::path assetPath, UUID id = 0);
        private:
            Ref<DragDropInfo> m_DragDropInfo;
            std::filesystem::path m_CurrentAssetPath;
            std::filesystem::path m_RootAssetPath;
            std::filesystem::path m_AssetLookupTablePath;

            YAML::Node m_AssetLookupTable;

            std::unordered_map<UUID, Ref<Asset<Model>>> m_LoadedModels;
            std::unordered_set<UUID> m_LoadedIDs;
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