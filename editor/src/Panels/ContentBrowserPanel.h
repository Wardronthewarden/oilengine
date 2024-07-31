#pragma once

#include <filesystem>
#include <oil.h>


namespace oil{

    static bool IsHidden(const std::filesystem::path path);

    struct FolderContentInfo{
        UUID ID;
        ContentType type;
        std::string name;
        std::filesystem::path path;
    };

    class MaterialEditorPanel;

    class ContentBrowserPanel{
    public:
        ContentBrowserPanel();

        void Init();

        //load all assets from AssetManager::CurrentAssetPath into m_CurrentFolderContents 
        void LoadCurrentFolderContents();

        //return the folder contents from a path relative to AssetManager::RootPath.
        //Returns directories
        std::vector<FolderContentInfo> GetFolderContents(std::string path);
        //return all assets under relative path. 
        //does not return directories as entries
        std::vector<FolderContentInfo> GetFolderContentsRecursive(std::filesystem::path path);


        void OnImGuiRender();

        //render elements
        Ref<Texture2D> RenderDirectoryEntry(FolderContentInfo& directoryEntry);

        void RenderBrowserContextMenu();
        void OpenBrowserItemActions();

    private:
        int NameEditCompletionCallback(ImGuiInputTextCallbackData* data);

    private:
        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;

        std::vector<FolderContentInfo> m_CurrentFolderContents;
        std::unordered_set<uint32_t> m_SelectedItems; //indices into m_CurrentFolderContents

        //inputs
        bool m_NameEditMode = false;
        std::string m_NameInputBuffer = "";

        Ref<MaterialEditorPanel> m_MaterialEditorPanel;
    };

}