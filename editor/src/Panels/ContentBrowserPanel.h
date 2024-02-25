#pragma once

#include <filesystem>
#include <oil.h>


namespace oil{

    class ContentBrowserPanel{
    public:
        ContentBrowserPanel();
        ContentBrowserPanel(Ref<AssetManager> assetManager)
            : m_AssetManagerRef(assetManager){
                ContentBrowserPanel();
        }

        void SetAssetManagerReference(const Ref<AssetManager>& assetManager) { m_AssetManagerRef = assetManager;}

        ContentType GetContentTypeFromFileExtension(std::string extension);


        void OnImGuiRender();

    private:
        std::filesystem::path m_CurrentDirectory;
        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;

        Ref<AssetManager> m_AssetManagerRef;
    };

}