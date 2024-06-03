#pragma once

#include <filesystem>
#include "oil/core/core.h"

//Importer functions
// Assimp
#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"


//Necessary includes
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
            AssetManager() = delete;
            /* AssetManager() 
             : m_RootAssetPath(assetRootPath), m_CurrentAssetPath(assetRootPath), m_AssetLookupTablePath(internalPath/"Scratch/assets.dat") {}; */

            static void Init(std::string assetRootPath, std::filesystem::path internalPath);
            ~AssetManager(){SaveAssetLookup();}

            //Drag drop functions
            static Ref<DragDropInfo> OnDragAsset(const DragDropInfo& asset);
            static Ref<DragDropInfo> GetDragDropInfo();

            //New asset creation
            template<typename T>
            static Asset<T> CreateAsset();

            template<typename T>
            static Asset<T> CreateAsset(Ref<T> assetObject);

            template<typename T>
            static Asset<T> CreateAsset(std::filesystem::path dir, std::string name = "unnamed_asset");

            template<typename T>
            static Asset<T> CreateAsset(std::filesystem::path dir, Ref<T> assetObject, std::string name = "unnamed_asset");

            //Asset loading and unloadig
            static UUID LoadAsset(UUID id);
            static UUID LoadAsset(std::filesystem::path path);
            static void LoadDependencies(std::unordered_set<UUID> deps);

            static void UnloadAsset(UUID id);

            template<typename T>
            static void SaveAsset(UUID id);

            template<typename T>
            static void SaveAsset(Asset<T> asset);

            template<typename T>
            static void SaveAssetAs(UUID id, std::filesystem::path path);
            
            template<typename T>
            static void SaveAssetAs(Asset<T> asset, std::filesystem::path path);

            //Getting assets
            template<typename T>
            static Asset<T> GetAsset(UUID id)
            {
                UUID assetID = LoadAsset(id);
                if (assetID)
                    return Asset<T>(assetID, s_AssetLookup<T>[assetID]);
                OIL_CORE_ERROR("Failed to get asset!");
                return Asset<T>();
                
            }
            static ContentType GetAssetType(UUID id);

            template<typename T>
            static void RefreshAsset(UUID id);

            static bool IDExists(UUID id);

            //Project directory queries and functions
            static std::filesystem::path GetRootDirectory() { return s_RootAssetPath; }
            static std::filesystem::path GetCurrentDirectory() { return s_CurrentAssetPath; }
            static void SetCurrentDirectory(std::filesystem::path path) { s_CurrentAssetPath = path; }
            static bool StepIntoDirectory(std::filesystem::path path);
            static bool StepOutOfDirectory();
            static bool IsCurrentRootDirectory() { return s_CurrentAssetPath == s_RootAssetPath; }

            //Get ID of asset at path
            static UUID GetIDFromPath(std::filesystem::path path); 
            static UUID GetIDFromPath(std::filesystem::path path, ContentType& type);
        private:
            //Asset lookup table functions
            static void LoadAssetLookup();
            static void SaveAssetLookup();

            template<typename T>
            static UUID AddToLookup(UUID id, Ref<T> asset);

            //Util functions
            static UUID GenerateUUID(uint32_t maxTries); //TODO: this is kinda dumb   
            inline static bool IsLoaded(UUID id) { return s_LoadedIDs.contains(id); };
            static std::filesystem::path GetPath(UUID id);
            static UUID AddEntry(std::filesystem::path assetPath, UUID id = 0);
            template<typename T>
            static void LoadAssetInternal(YAML::Node& node, std::filesystem::path path, utils::AssetHeader header);

        private:
            static Ref<DragDropInfo> s_DragDropInfo;
            static std::filesystem::path s_CurrentAssetPath;
            static std::filesystem::path s_RootAssetPath;
            static std::filesystem::path s_AssetLookupTablePath;

            static YAML::Node s_AssetLookupTable;

            template<typename T>
            static std::unordered_map<UUID, Ref<T>> s_AssetLookup;

            static std::unordered_map<UUID, ContentType> s_LoadedIDs;
    };

    class AssetImporter{
        public:
            AssetImporter() = delete;

            static void Init();


            static UUID ImportModel(std::filesystem::path src, std::filesystem::path dest = "");
            static UUID ImportImage(std::filesystem::path src, std::filesystem::path dest = "");

        private:
            static void ProcessNode(aiNode *node, const aiScene* scene, Ref<Model> model);
            static Ref<Mesh> ProcessMesh(aiMesh * mesh, const aiScene * scene);
            static Assimp::Importer s_Importer;
            
    };

}