#pragma once

#include <filesystem>
#include <oil.h>


namespace oil{

    struct FolderContentInfo{
        UUID ID;
        ContentType type;
        std::string name;
        std::filesystem::path path;
    };

    class ContentBrowserPanel{
    public:
        ContentBrowserPanel();
        ContentBrowserPanel(Ref<AssetManager> assetManager)
            : m_AssetManagerRef(assetManager){
                ContentBrowserPanel();
        }

        void Init();

        void SetAssetManagerReference(const Ref<AssetManager>& assetManager) { m_AssetManagerRef = assetManager;}

        //Draw folder contents
        Ref<Texture2D> RenderDirectoryEntry(FolderContentInfo& directoryEntry);
        void GetFolderContents();


        void OnImGuiRender();

    private:
        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;

        Ref<AssetManager> m_AssetManagerRef;

        std::vector<FolderContentInfo> m_CurrentFolderContents;
    };

}