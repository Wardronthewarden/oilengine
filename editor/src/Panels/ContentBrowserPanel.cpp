#include "pch/oilpch.h"
#include "ContentBrowserPanel.h"
#include "MaterialEditorPanel.h"

#include "utils/UIlib.h"


namespace oil{

    ContentBrowserPanel::ContentBrowserPanel()
    {
        m_DirectoryIcon = Texture2D::Create("Internal/Assets/src/Textures/DirectoryIcon.png");
        m_FileIcon = Texture2D::Create("Internal/Assets/src/Textures/FileIcon.png");
        m_MaterialEditorPanel = CreateRef<MaterialEditorPanel>();
    }

    void ContentBrowserPanel::Init()
    {
        LoadCurrentFolderContents();
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

    int ContentBrowserPanel::NameEditCompletionCallback(ImGuiInputTextCallbackData *data)
    {

        FolderContentInfo& activeItem = m_CurrentFolderContents[*m_SelectedItems.begin()];
        if(data->EventFlag == ImGuiInputTextFlags_CallbackEdit){
                
        }
        return 0;
    }

    void ContentBrowserPanel::LoadCurrentFolderContents()
    {
        m_CurrentFolderContents.clear();
        m_SelectedItems.clear();
        for (auto& directoryEntry : std::filesystem::directory_iterator(AssetManager::GetCurrentDirectory())){
                FolderContentInfo fi;
                fi.path = directoryEntry.path();
                fi.name = directoryEntry.path().stem().string();
                //Check if the entry is a directory
                if(!(directoryEntry.path().filename().extension() == ".oil")){
                    if(directoryEntry.is_directory() && !IsHidden(directoryEntry.path())){
                        fi.type = ContentType::Directory;
                        fi.ID = 0;
                        m_CurrentFolderContents.push_back(fi);
                    }
                    continue;
                }
               
                fi.ID = AssetManager::GetHandleByName(fi.name);
                fi.type = AssetManager::GetType(fi.ID);
                m_CurrentFolderContents.push_back(fi);
        }

    }

    std::vector<FolderContentInfo> ContentBrowserPanel::GetFolderContents(std::string path)
    {
        std::vector<FolderContentInfo> contents;
        for (auto& directoryEntry : std::filesystem::directory_iterator(std::filesystem::relative(path, AssetManager::GetRootDirectory()))){
                FolderContentInfo fi;
                fi.path = directoryEntry.path();
                fi.name = directoryEntry.path().stem().string();
                //Check if the entry is a directory
                if(!(directoryEntry.path().filename().extension() == ".oil")){
                    if(directoryEntry.is_directory() && !IsHidden(directoryEntry.path())){
                        fi.type = ContentType::Directory;
                        fi.ID = 0;
                        contents.push_back(fi);
                    }
                    continue;
                }
               
                fi.ID = AssetManager::GetHandleByName(fi.name);
                fi.type = AssetManager::GetType(fi.ID);
                contents.push_back(fi);
        }
        return contents;
    }

    std::vector<FolderContentInfo> ContentBrowserPanel::GetFolderContentsRecursive(std::filesystem::path path)
    {
        std::vector<FolderContentInfo> contents;
        for (auto& directoryEntry : std::filesystem::directory_iterator(std::filesystem::relative(path))){
            
            FolderContentInfo fi;
            fi.path = directoryEntry.path();
            fi.name = directoryEntry.path().stem().string();
            //Check if the entry is a directory
            if(!(directoryEntry.path().filename().extension() == ".oil")){
                if(directoryEntry.is_directory() && !IsHidden(directoryEntry.path())){
                    std::vector<FolderContentInfo> subfolder = GetFolderContentsRecursive(fi.path); 
                    contents.insert(contents.end(), subfolder.begin(), subfolder.end());
                }
                continue;
            }
            
            fi.ID = AssetManager::GetHandleByName(fi.name);
            fi.type = AssetManager::GetType(fi.ID);
            contents.push_back(fi);
        }
        return contents;
    }

    void OpenBrowserItemActions(){

    }

    void ContentBrowserPanel::OnImGuiRender()
    {

        ImGui::Begin("Content Browser panel");

        if (!AssetManager::IsCurrentRootDirectory()){
            if (ImGui::Button("<=")){
                AssetManager::StepOutOfDirectory();
                LoadCurrentFolderContents();
                //TODO: drag asset one folder back
            }
        }


        static float padding = 16.0f;
        static float thumbnailSize = 96.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if(columnCount < 1) columnCount = 1;

        //Draw browser context menu
        if(ImGui::BeginPopupContextWindow()){
            if(ImGui::BeginMenu("Create Asset")){
                if (ImGui::MenuItem("Scene")){
                    AssetManager::CreateAsset<Scene>("unnamed_scene", CreateRef<Scene>(), AssetManager::GetCurrentDirectory());
                    LoadCurrentFolderContents();
                }
                if (ImGui::MenuItem("Shader")){
                    std::filesystem::path src = AssetManager::CreateSource(AssetManager::GetRootDirectory() / (".shaderSources/unnamed_shader.glsl"),AssetManager::GetSource(AssetManager::GetHandleByName("DefaultSurfaceShader")));
                    AssetManager::CreateAsset<Shader>("unnamed_shader", Shader::Create(src.string()), AssetManager::GetCurrentDirectory(), src);
                    LoadCurrentFolderContents();
                }

                ImGui::EndMenu();
            }

            if(ImGui::MenuItem("New Folder")){
                    std::filesystem::create_directory(AssetManager::GetCurrentDirectory() / "NewFolder");
                    LoadCurrentFolderContents();
            }

            ImGui::EndPopup();
        }

        //Draw all folder contents
        ImGui::Columns(columnCount, 0, false);

        uint32_t i = 0;
        for (FolderContentInfo& directoryEntry : m_CurrentFolderContents){
            Ref<Texture2D> thumbnail = RenderDirectoryEntry(directoryEntry);
            bool isEdited = m_NameEditMode && m_SelectedItems.contains(i);
            
            ImGui::PushID(directoryEntry.name.c_str());

            if (UI::DrawAssetItemBrowser(thumbnail, isEdited ? m_NameInputBuffer : directoryEntry.name, isEdited)){

              
                std::filesystem::path newName;
                if (directoryEntry.type == ContentType::Directory){
                    newName = directoryEntry.path.parent_path() / m_NameInputBuffer;
                    std::filesystem::rename(directoryEntry.path, newName);
                    //update all asset paths under a folder 
                    for(auto& content : GetFolderContentsRecursive(newName)){
                        AssetManager::SetPath(content.ID, content.path);
                    }

                }else{
                    newName = (directoryEntry.path.parent_path() / m_NameInputBuffer).replace_extension(".oil");
                    std::filesystem::rename(directoryEntry.path,  newName);
                    AssetManager::SetPath(directoryEntry.ID, newName);
                    if(directoryEntry.type == ContentType::Shader){
                        std::string n = newName.filename().stem().string();
                        AssetManager::RenameSource(directoryEntry.ID, n);
                        AssetManager::SaveAsset<Shader>(directoryEntry.ID);
                    }
                }

                
                m_NameEditMode = false;
                LoadCurrentFolderContents();
            }

            if (directoryEntry.type == ContentType::Directory){
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)){

                    ImGui::SetDragDropPayload("DIR", &directoryEntry.path, sizeof(std::filesystem::path), ImGuiCond_Once);
                    ImGui::EndDragDropSource();
                }

                 if (ImGui::BeginDragDropTarget()){
                    //Accept asset drop
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OIL_ASSET")){
                        OIL_ASSERT(payload->DataSize == sizeof(UUID), "Payload size mismatch!");
                        UUID draggedID = *(UUID*)payload->Data;
                        AssetManager::SetPath(draggedID, directoryEntry.path);
                    }
                    //Accept directory drop
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DIR")){
                        OIL_ASSERT(payload->DataSize == sizeof(std::filesystem::path), "Payload size mismatch!");
                        std::filesystem::path draggedPath = *(std::filesystem::path*)payload->Data;
                        //TODO: set new directory path here
                    }

                    ImGui::EndDragDropTarget();
                }
            }else{
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)){

                    ImGui::SetDragDropPayload("OIL_ASSET", &directoryEntry.ID, sizeof(UUID), ImGuiCond_Once);
                    ImGui::EndDragDropSource();
                }
            }
            
            if (isEdited && (Input::IsMouseButtonPressed(OIL_MOUSE_BUTTON_1) && !ImGui::IsItemHovered())){
                m_NameEditMode = false;
            }

            //Item opening
            if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)){
                if(std::filesystem::is_directory(directoryEntry.path))
                    AssetManager::StepIntoDirectory(directoryEntry.path.filename());
                    LoadCurrentFolderContents();

                switch(directoryEntry.type){
                    case ContentType::Material:{
                        m_MaterialEditorPanel->OpenMaterialToEdit(AssetManager::GetAssetReference<Material>(directoryEntry.ID));
                        break;
                    } 
                    case ContentType::Shader:{
                        std::string command = "code " + AssetManager::GetSource(directoryEntry.ID).string();
                        std::system(command.c_str());
                        break;
                    } 
                }
            }

            //Selection
            if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
                if(Input::IsKeyPressed(OIL_KEY_LEFT_SHIFT) || Input::IsKeyPressed(OIL_KEY_RIGHT_SHIFT)){
                    m_SelectedItems.emplace(i);
                }else{
                    m_SelectedItems.clear();
                    m_SelectedItems.emplace(i);
                }
            }

            ImGui::OpenPopupOnItemClick("BrowserItemActions");
                

            if(ImGui::BeginPopup("BrowserItemActions")){
                if(!m_SelectedItems.contains(i)){
                    m_SelectedItems.clear();
                    m_SelectedItems.emplace(i);
                }

                if(std::filesystem::is_directory(directoryEntry.path)){
                    if(ImGui::MenuItem("Remove Directory")){
                        std::filesystem::remove(directoryEntry.path);
                        LoadCurrentFolderContents();
                    }
                }else{
                    if(ImGui::MenuItem("Delete Asset")){
                        switch (directoryEntry.type){
                            case ContentType::Model:{
                                AssetManager::DeleteAsset<Model>(directoryEntry.ID);
                                break;
                            } 
                            case ContentType::Shader:{
                                AssetManager::DeleteAsset<Shader>(directoryEntry.ID);
                                break;
                            } 
                            case ContentType::Material:{
                                AssetManager::DeleteAsset<Material>(directoryEntry.ID);
                                break;
                            } 
                            case ContentType::Scene:{
                                AssetManager::DeleteAsset<Scene>(directoryEntry.ID);
                                break;
                            } 
                            case ContentType::Texture2D:{
                                AssetManager::DeleteAsset<Texture2D>(directoryEntry.ID);
                                break;
                            } 
                            default: OIL_ERROR("ContentType deletion of {0} is not yet implemented!");
                        }
                        LoadCurrentFolderContents();
                    }

                    switch (directoryEntry.type){
                        case ContentType::Shader:{
                            if(ImGui::MenuItem("Create Material")){
                                AssetManager::CreateAsset<Material>("unnamed_material", CreateRef<Material>(AssetManager::GetAssetReference<Shader>(directoryEntry.ID)), AssetManager::GetCurrentDirectory());
                                LoadCurrentFolderContents();
                                break;
                            }
                            if(ImGui::MenuItem("Recompile")){
                                AssetManager::GetAsset<Shader>(directoryEntry.ID)->Recompile(AssetManager::GetSource(directoryEntry.ID).string());
                                break;
                            }
                        }
                    }
                    
                }

                //general actions
                if(m_SelectedItems.contains(i) && (m_SelectedItems.size() == 1)){
                    if(ImGui::MenuItem("Rename")){
                        m_NameEditMode = true;
                        m_NameInputBuffer = directoryEntry.name.c_str();
                    }
                }
                ImGui::EndPopup();
            }
                
            ImGui::PopID();
            
            ImGui::NextColumn();
            i++;
        }

             
        
        ImGui::Columns(1);

        ImGui::End();

        m_MaterialEditorPanel->OnImguiRender();
    }
    bool IsHidden(const std::filesystem::path path)
    {
        std::string name = path.filename().string();
        if(name [0] == '.')
            return true;
        return false;
    }
}