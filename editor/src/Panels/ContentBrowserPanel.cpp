#include "pch/oilpch.h"
#include "ContentBrowserPanel.h"

#include <imgui.h>


namespace oil{
    // Change this when we can
    extern const std::filesystem::path g_AssetPath = "Assets";

    ContentBrowserPanel::ContentBrowserPanel()
        : m_CurrentDirectory(g_AssetPath)
    {
        m_DirectoryIcon = Texture2D::Create("Internal/Assets/Textures/DirectoryIcon.png");
        m_FileIcon = Texture2D::Create("Internal/Assets/Textures/FileIcon.png");
    }

    ContentType ContentBrowserPanel::GetContentTypeFromFileExtension(std::string extension)
    {
        if(extension == ".oil") return ContentType::Scene;
        if(extension == ".jpg") return ContentType::Texture;
        if(extension == ".png") return ContentType::Texture;
        if(extension == ".glsl") return ContentType::Shader;

        OIL_CORE_ERROR("ContentType == {0}", extension);

        OIL_CORE_ASSERT(false, "Invalid content type deduced!");
        return ContentType::None;
    }

    void ContentBrowserPanel::OnImGuiRender()
    {

        ImGui::Begin("Content Browser panel");

        if (m_CurrentDirectory != g_AssetPath){
            if (ImGui::Button("<=")){
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
        }

        static float padding = 16.0f;
        static float thumbnailSize = 96.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if(columnCount < 1) columnCount = 1;

        ImGui::Columns(columnCount, 0, false);


        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)){


            const auto& path = directoryEntry.path();
            auto relativePath = std::filesystem::relative(path, g_AssetPath);
            std::string filenameString = relativePath.filename().string();
            ImGui::PushID(filenameString.c_str());


            Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});


            if (ImGui::BeginDragDropSource()){
                ContentType type;
                if (directoryEntry.is_directory())
                    type = ContentType::Directory;
                else{
                    type = GetContentTypeFromFileExtension(relativePath.extension().string());
                }

                m_AssetManagerRef->OnDragAsset({type, relativePath.c_str()});

                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &m_AssetManagerRef, sizeof(AssetManager), ImGuiCond_Once);
                ImGui::EndDragDropSource();
            }


            ImGui::PopStyleColor();
            if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)){
                if(directoryEntry.is_directory())
                    m_CurrentDirectory /= path.filename();
            }
            ImGui::TextWrapped(filenameString.c_str());

            ImGui::NextColumn();

            ImGui::PopID();
             
        } 
        ImGui::Columns(1);

        ImGui::End();
    }
}