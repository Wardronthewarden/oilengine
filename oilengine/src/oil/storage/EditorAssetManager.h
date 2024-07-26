#pragma once

#include <filesystem>
#include "oil/core/core.h"
#include <oil/core/UUID.h>

//Importer includes
// Assimp
#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"
#include "Asset.h"
#include "Serializer.h"

namespace oil{

    //Forward declaration
    class Mesh;
    class Model;

    struct DragDropInfo{
        ContentType contentType;
        std::filesystem::path itemPath;
        AssetHandle Handle;

        DragDropInfo() = default;
        DragDropInfo(ContentType type, const wchar_t* path, AssetHandle handle = 0)
            : contentType(type), itemPath(path), Handle(handle) {
            }
        DragDropInfo(DragDropInfo& rhs)
            :contentType(rhs.contentType), itemPath(rhs.itemPath){}

        ~DragDropInfo(){
        }

        
    };

    struct AssetMetadata{
        ContentType Type;
        std::filesystem::path AssetPath;
        std::filesystem::path SrcPath;
    };


    //Make this completely static
    class EditorAssetManager{
        public:
            EditorAssetManager() = delete;

            ~EditorAssetManager(){
                SaveAssetRegistry();
            }

            //Initialization
            static void Init(std::string assetRootPath, std::filesystem::path internalPath);
            

            //Drag drop functions
            //This should not be here
            static Ref<DragDropInfo> OnDragAsset(const DragDropInfo& asset);
            static Ref<DragDropInfo> GetDragDropInfo();

            //New asset creation
            template<typename T>
            static AssetHandle CreateAsset(std::string name, Ref<T> assetObject, std::filesystem::path dir = s_RootAssetPath){
                OIL_CORE_ASSERT(!dir.has_extension(), "Can not create file at path {0}.", dir.string());
                //Generate metadata for file
                AssetHandle handle = GenerateAssetHandle(10);
                if(!handle){
                    OIL_CORE_ERROR("Failed to generate handle for asset. Exiting asset creation.");
                    return 0;
                }
                RegisterAsset(handle);
                SetName(handle, name);
                SetPath(handle, dir / (name + ".oil"));
                SetType(handle, AssetRef<T>::GetType());
                SaveAssetRegistry();
                //serialize the asset
                Serializer::SerializeAsset<T>(assetObject, dir / (name + ".oil"), handle);
                return handle;

            }

            template<typename T>
            static void DeleteAsset(AssetHandle handle){
                if (UnloadAsset<T>(handle) == 0){
                    std::filesystem::remove(s_AssetRegistry[handle].AssetPath);
                    s_AssetRegistry.erase(handle);
                    SaveAssetRegistry();
                    return;
                }
                OIL_WARN("Failed to delete asset");
            }

            //Asset loading and unloadig
            template<typename T>
            static bool LoadAsset(AssetHandle handle){
        
                if(!IsValid(handle)){
                    OIL_CORE_ERROR("Asset with Handle {0} does not exist in registry!", handle);
                    return false;
                }

                Ref<T> reference = Serializer::DeserializeAsset<T>(s_AssetRegistry[handle].AssetPath, handle);
                if(reference){
                    if (s_AssetLookup<T>.contains(handle)){
                        s_AssetLookup<T>[handle].reset(&reference);
                    }else{
                        s_AssetLookup<T>[handle] = CreateRef<Ref<T>>(reference);
                    }
                    return true;
                }
                
                OIL_CORE_ERROR("Failed to load asset with Handle {0}", handle);
                return false;

                
            }

            template<typename T>
            static int UnloadAsset(AssetHandle handle){
                if(IsLoaded<T>(handle)){
                    int count = (*s_AssetLookup<T>[handle].get()).use_count();
                    if(count <= 1){
                        s_AssetLookup<T>[handle].reset();
                        return 0;
                    }
                    OIL_WARN("Unable to delete {0}; currently referenced by {1} objects.", s_AssetRegistry[handle].AssetPath.filename().stem().string(), count-1);
                    return count-1; //we return the number of objects still using the asset
                }
                return 0;
            }

            template<typename T>
            static void SaveAsset(AssetHandle handle){
                Serializer::SerializeAsset<T>(*s_AssetLookup<T>[handle].get(), s_AssetRegistry[handle].AssetPath, handle);
            }

            //Getting assets
            template<typename T>
            static Ref<T> GetAsset(AssetHandle handle)
            {
                if(!handle) return nullptr;
                
                if (LoadAsset<T>(handle))
                    return *s_AssetLookup<T>[handle].get();
                return nullptr;
                
            }

            template<typename T>
            static AssetRef<T> GetAssetReference(AssetHandle handle){
                if(!handle) return AssetRef<T>();
                
                if (LoadAsset<T>(handle))
                    return AssetRef<T>(handle, s_AssetLookup<T>[handle]);
                return AssetRef<T>(); 
            }

            //Internal asset handling
            static void ReimportInternalAssets();

            //Meta queries
            static ContentType GetType(AssetHandle handle) {
                return s_AssetRegistry[handle].Type;
            }
            static bool IsValid(AssetHandle handle);

            static AssetHandle GetHandleByName(std::string name);

            //Editor asset manager functions -------------------------
            //Metadata functions
            static AssetMetadata GetMetadata(AssetHandle handle);
            static void SetMetadata(AssetHandle handle, AssetMetadata metadata);

            static void RegisterAsset(AssetHandle handle);

            static void SetSource(AssetHandle handle, std::filesystem::path src);
            static void SetPath(AssetHandle handle, std::filesystem::path path);
            static void SetType(AssetHandle handle, ContentType type);
            static bool SetName(AssetHandle handle, std::string& name);

            //Project directory queries and functions
            static std::filesystem::path GetRootDirectory() { return s_RootAssetPath; }
            static std::filesystem::path GetCurrentDirectory() { return s_CurrentAssetPath; }
            static void SetCurrentDirectory(std::filesystem::path path) { s_CurrentAssetPath = path; }
            static bool StepIntoDirectory(std::filesystem::path path);
            static bool StepOutOfDirectory();
            static bool IsCurrentRootDirectory() { return s_CurrentAssetPath == s_RootAssetPath; }
            // --------------------------------------------------

        private:
            //Asset registry functions
            static void LoadAssetRegistry();
            static void SaveAssetRegistry();

            //Util functions
            static AssetHandle GenerateAssetHandle(uint32_t maxTries); //TODO: this is kinda dumb
            template<typename T>
            inline static bool IsLoaded(AssetHandle handle) { return s_AssetLookup<T>.contains(handle); };

        private:
            static Ref<DragDropInfo> s_DragDropInfo;
            static std::filesystem::path s_CurrentAssetPath;
            static std::filesystem::path s_RootAssetPath;
            static std::filesystem::path s_InternalAssetPath;
            static std::filesystem::path s_DataPath;

            static std::unordered_map<AssetHandle, AssetMetadata> s_AssetRegistry;
            static std::unordered_map<std::string, AssetHandle> s_AssetNameRegistry;

            template<typename T>
            static std::unordered_map<AssetHandle, Ref<Ref<T>>> s_AssetLookup;

    };





    class AssetImporter{
        public:
            AssetImporter() = delete;

            static void Init();


            static AssetHandle ImportModel(std::filesystem::path src, std::filesystem::path dest = "");
            static AssetHandle ImportImage(std::filesystem::path src, std::filesystem::path dest = "");

        private:
            static void ProcessNode(aiNode *node, const aiScene* scene, Ref<Model> model);
            static Ref<Mesh> ProcessMesh(aiMesh * mesh, const aiScene * scene);
            static Assimp::Importer s_Importer;
            
    };

}