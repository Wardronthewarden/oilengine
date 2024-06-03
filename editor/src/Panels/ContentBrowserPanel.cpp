#include "pch/oilpch.h"
#include "ContentBrowserPanel.h"

#include <imgui.h>


namespace oil{

    ContentBrowserPanel::ContentBrowserPanel()
    {
        m_DirectoryIcon = Texture2D::Create("Internal/Assets/Textures/DirectoryIcon.png");
        m_FileIcon = Texture2D::Create("Internal/Assets/Textures/FileIcon.png");
    }

    void ContentBrowserPanel::Init()
    {
        GetFolderContents();
    }

    Ref<Texture2D> ContentBrowserPanel::RenderDirectoryEntry(FolderContentInfo &directoryEntry)
    {
        Ref<Texture2D> tex;
        switch (directoryEntry.type){
            case ContentType::Directory:
                tex = m_DirectoryIcon;
                break;
            default:
                tex = m_FileIcon;
        }
        return tex;
    }

    void ContentBrowserPanel::GetFolderContents()
    {
        m_CurrentFolderContents.clear();
        for (auto& directoryEntry : std::filesystem::directory_iterator(AssetManager::GetCurrentDirectory())){
                FolderContentInfo fi;
                fi.path = directoryEntry.path();
                auto relativePath = std::filesystem::relative(fi.path, AssetManager::GetRootDirectory());
                
                fi.name = relativePath.filename().stem().string();

                if(directoryEntry.is_directory()){
                    fi.type = ContentType::Directory;
                    fi.ID = 0;
                    m_CurrentFolderContents.push_back(fi);
                    continue;
                }
                    
                if(!(relativePath.filename().extension() == ".oil"))
                    continue;
                        
                fi.ID = AssetManager::GetIDFromPath(directoryEntry, fi.type);
                m_CurrentFolderContents.push_back(fi);
        }

    }

    void ContentBrowserPanel::OnImGuiRender()
    {

        ImGui::Begin("Content Browser panel");

        if (!AssetManager::IsCurrentRootDirectory()){
            if (ImGui::Button("<=")){
                AssetManager::StepOutOfDirectory();
                GetFolderContents();
            }
        }


        static float padding = 16.0f;
        static float thumbnailSize = 96.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if(columnCount < 1) columnCount = 1;

        ImGui::Columns(columnCount, 0, false);


        for (FolderContentInfo& directoryEntry : m_CurrentFolderContents){

            Ref<Texture2D> icon = RenderDirectoryEntry(directoryEntry);
            ImGui::PushID(directoryEntry.name.c_str());

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});


            if (ImGui::BeginDragDropSource()){

                Ref<DragDropInfo> idref = AssetManager::OnDragAsset({directoryEntry.type, directoryEntry.path.c_str(), directoryEntry.ID});

                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &idref, sizeof(UUID), ImGuiCond_Once);
                ImGui::EndDragDropSource();
            }


            ImGui::PopStyleColor();
            if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)){
                if(std::filesystem::is_directory(directoryEntry.path))
                    AssetManager::StepIntoDirectory(directoryEntry.path.filename());
                    GetFolderContents();
            }
            ImGui::TextWrapped(directoryEntry.name.c_str());

            ImGui::NextColumn();

            ImGui::PopID();
             
        } 
        ImGui::Columns(1);

        ImGui::End();
    }
}