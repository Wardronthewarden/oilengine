#pragma once

#include "oil/core/UUID.h"
#include "FileUtils.h"
    //Forward declarations
namespace YAML{
    class Node;
    class Emitter;
}
namespace oil{
    class Entity;
    struct AssetMetadata;
    using AssetHandle = UUID;

    //Serialization
    class Serializer{
    public:

        //Asset serialization
        template<typename T>
        static void SerializeAsset(const Ref<T> asset , AssetMetadata metadata, UUID assetHandle);

        template<typename T>
        static Ref<T> DeserializeAssetYAML(YAML::Node file);

        template<typename T>
        static Ref<T> DeserializeAsset(std::filesystem::path path, UUID assetHandle){
            OIL_CORE_ASSERT(std::filesystem::exists(path), "Asset from path has either been moved or deleted.");

            if(path.extension().string() == ".oil"){
                #ifndef OIL_EDITOR
                    OIL_CORE_ASSERT(false, "Can not deserialize .oil files in runtime.");
                #endif
                
                std::ifstream stream(path);
                utils::AssetHeader header = utils::ReadAssetHeader(stream);

                if (header.ID != assetHandle){
                    std::ofstream ostream(path);
                    OIL_CORE_WARN("Asset held a different handle from the library. Overwriting file header from registry with handle [{0}]", assetHandle);
                    header.ID = assetHandle;
                    ostream << stream.rdbuf();
                    utils::WriteAssetHeader(ostream, header);
                }

                return DeserializeAssetYAML<T>(utils::ReadAssetBody(stream));
            }

            OIL_CORE_ASSERT(false, "Can not recognize asset file format!");
            return nullptr;
        }

        //Asset registry serialization
        static void SerializeAssetRegistry(std::filesystem::path path, std::unordered_map<AssetHandle, AssetMetadata> dataTable);
        static std::unordered_map<AssetHandle, AssetMetadata> DeserializeAssetRegistry(std::filesystem::path path);

    
    private:
        static void SerializeEntity(YAML::Emitter& out, Entity& entity);
    };

}