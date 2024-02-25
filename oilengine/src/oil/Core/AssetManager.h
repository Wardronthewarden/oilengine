#pragma once

#include <filesystem>
#include "core.h"

namespace oil{

     enum class ContentType{
        None = 0, Directory = 1, Scene = 2, Model = 3, Material = 4, Shader = 5, Texture = 6, Composition = 7, Actor = 8, Component = 9, Script = 10
    };

    struct DragDropInfo{
        ContentType contentType;
        std::filesystem::path itemPath;

        DragDropInfo() = default;
        DragDropInfo(ContentType type, const wchar_t* path)
            : contentType(type), itemPath(path) {
            }
        DragDropInfo(DragDropInfo& rhs)
            :contentType(rhs.contentType), itemPath(rhs.itemPath){}

        ~DragDropInfo(){
        }

        
    };

    class AssetManager{
        public:
            AssetManager() = default;

            void OnDragAsset(DragDropInfo asset);
            DragDropInfo GetDragDropInfo();

            
        private:
            DragDropInfo m_DragDropInfo;
    };
    
}