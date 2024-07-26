#pragma once

#include "Asset.h"

namespace oil{

    class RuntimeAssetManager{

        //New asset creation
            template<typename T>
            static AssetHandle CreateAsset() = 0;

            template<typename T>
            static AssetHandle CreateAsset(Ref<T> assetObject);

            template<typename T>
            static AssetHandle CreateAsset(std::filesystem::path dir, std::string name = "unnamed_asset");

            template<typename T>
            static AssetHandle CreateAsset(std::filesystem::path dir, Ref<T> assetObject, std::string name = "unnamed_asset");

            template<typename T>
            static void DeleteAsset(AssetHandle handle);

            //Asset loading and unloadig
            template<typename T>
            static bool LoadAsset(AssetHandle handle);
            template<typename T>
            static int UnloadAsset(AssetHandle handle);

            template<typename T>
            static void SaveAsset(AssetHandle handle);

            template<typename T>
            static Ref<T> GetAsset(AssetHandle handle)

    };

}//oil