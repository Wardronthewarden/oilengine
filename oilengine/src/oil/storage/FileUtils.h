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
#include "Serializer.h"

namespace YAML{
    
    //YAML overwrites
    //CPP types
    template<>
    struct convert<std::filesystem::path>{
        static Node encode(const std::filesystem::path& rhs){
            Node node;
            node = rhs.string();
            return node;
        }

        static bool decode(const Node& node, std::filesystem::path& rhs){
            rhs = node.as<std::string>();
            return true;
        }
    };
    
    //Native types
    template<>
    struct convert<oil::UUID>{
        static Node encode(const oil::UUID& rhs){
            Node node;
            node = (uint64_t)rhs;
            return node;
        }

        static bool decode(const Node& node, oil::UUID& rhs){
            rhs = node.as<uint64_t>();
            return true;
        }
    };
}

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


        //Templated functions
        template <typename T>
        Ref<T> LoadAssetObject(std::ifstream& stream)
        {

            YAML::Node node = utils::ReadAssetBody(stream);
            return Serializer::Deserialize<T>(node);
        }

        template <typename T, typename U>
        std::unordered_map<T,U> LoadHashmap(std::filesystem::path path){
            std::ifstream stream(path);
            std::stringstream strStream;
            strStream << stream.rdbuf();
            
            std::unordered_map<T,U> ret;

            const YAML::Node map = YAML::Load(strStream.str());
            
            for (auto it = map.begin(); it != map.end(); ++it){
                ret[it->first.as<T>()] = it->second.as<U>();
            }
            return ret;
        }

        template <typename T, typename U>
        void SaveHashmap(std::filesystem::path path, std::unordered_map<T,U> map){
            std::ofstream stream(path);
            YAML::Node node;
            for(auto& item : map)
                node[item.first] = item.second;

            YAML::Emitter out;
            out << node;
            stream << out.c_str();
        }


    }//utils

    #define OIL_FILE_HEADER_SIZE sizeof(utils::AssetHeader)

}