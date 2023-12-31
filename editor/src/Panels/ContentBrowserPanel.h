#pragma once

#include <filesystem>
#include <oil.h>


namespace oil{

    class ContentBrowserPanel{
    public:
        ContentBrowserPanel();

        void OnImGuiRender();

    private:
        std::filesystem::path m_CurrentDirectory;
        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;
    };

}