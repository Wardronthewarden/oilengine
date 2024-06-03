#pragma once

//constants


//Mandatory includes
#include <yaml-cpp/yaml.h>
#include "oil/Renderer/Buffer.h"


// engine core
#include <oil/core/core.h>
#include <oil/core/UUID.h>

//Importer includes
#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"



namespace oil{
    enum class ContentType;
    namespace utils{

        struct alignas(32) AssetHeader{
            UUID ID;
            ContentType type;
            uint32_t fileSize;
            uint32_t fileVersionMajor;
            uint32_t fileVersionMinor;

            AssetHeader() = default;
            
            AssetHeader(const DataBuffer<char>& fileHeader){
                OIL_CORE_ASSERT(fileHeader.GetSize() == sizeof(AssetHeader), "Incorrect buffer length received when processing AssetHeader from file");
                memcpy(this, fileHeader.GetData(), fileHeader.GetSize());
            }

            AssetHeader(UUID id, ContentType ty, uint32_t size, uint32_t versionMajor, uint32_t versionMinor)
                : ID(id), type(ty), fileSize(size), fileVersionMajor(versionMajor), fileVersionMinor(versionMinor)
            {}
        };
        
        AssetHeader ReadAssetHeader(std::ifstream& assetFile);
        void WriteAssetHeader(std::ofstream& assetFile, AssetHeader header);

        YAML::Node ReadAssetBody(std::ifstream& assetFile);
        void WriteAssetBody(std::ofstream& assetFile, YAML::Emitter& body);

        size_t GetFileSize(std::ifstream& file);
        size_t GetFileSize(std::ofstream& file);


    }//utils

    #define OIL_FILE_HEADER_SIZE sizeof(utils::AssetHeader)

}