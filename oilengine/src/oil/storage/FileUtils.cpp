#include <pch/oilpch.h>
#include "FileUtils.h"


namespace oil{

    namespace utils{
        AssetHeader ReadAssetHeader(std::ifstream& assetFile)
        {
            assetFile.seekg(0, std::ios::beg);
            DataBuffer buffer = DataBuffer<char>(OIL_FILE_HEADER_SIZE);
            assetFile.read(buffer, OIL_FILE_HEADER_SIZE);


            return AssetHeader(buffer);
        }

        void WriteAssetHeader(std::ofstream& assetFile, AssetHeader header)
        {
            assetFile.seekp(0, std::ios::beg);
            assetFile.write((char*)&header, OIL_FILE_HEADER_SIZE);



        }

        YAML::Node ReadAssetBody(std::ifstream &assetFile)
        {
            assetFile.seekg(OIL_FILE_HEADER_SIZE, std::ios::beg);

            std::stringstream strStream;
            strStream << assetFile.rdbuf();
            YAML::Node node = YAML::Load(strStream.str());

            return node;
        }

        void WriteAssetBody(std::ofstream &assetFile, YAML::Emitter& body)
        {
            assetFile.seekp(OIL_FILE_HEADER_SIZE, std::ios::beg);

            assetFile << body.c_str();
        }

        size_t GetFileSize(std::ifstream &file)
        {
            size_t beg, end;
            file.seekg(0, std::ios::beg);
            beg = file.tellg();

            file.seekg(0, std::ios::end);
            end = file.tellg();
            return end-beg;
        }

        size_t GetFileSize(std::ofstream &file)
        {
            size_t beg, end;
            file.seekp(0, std::ios::beg);
            beg = file.tellp();

            file.seekp(0, std::ios::end);
            end = file.tellp();
            return end-beg;
        }

    }//utils


   
}
