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

        void Init();

        //Draw folder contents
        Ref<Texture2D> RenderDirectoryEntry(FolderContentInfo& directoryEntry);
        void GetFolderContents();


        void OnImGuiRender();

    private:
        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;

        std::vector<FolderContentInfo> m_CurrentFolderContents;
    };

}