#include "SceneHierarchyPanel.h"
#include "oil/Scene/Component.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace oil{

    extern const std::filesystem::path g_AssetPath = "Assets";

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &scene)
    {
        SetContext(scene);
    }
    void SceneHierarchyPanel::SetContext(const Ref<Scene> &context)
    {
        m_Context = context;
        m_SelectionContext = {};
    }
    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");

        m_Context->m_Registry.each([&](auto entityID){
            Entity entity {entityID, m_Context.get() };
            DrawEntityNode(entity);
        });

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {};

        ImGuiPopupFlags flags = ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight;
        if (ImGui::BeginPopupContextWindow(0, flags)){
            if (ImGui::MenuItem("Create Empty Entity"))
                m_Context->CreateEntity("Empty Entity");
            
            ImGui::EndPopup();
        }

        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectionContext){
            DrawComponents(m_SelectionContext);
        }
        ImGui::End();
    }
    void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
    {
        m_SelectionContext = entity;
    }
    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            
            ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow; 
            flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
            bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
            if (ImGui::IsItemClicked()){
                m_SelectionContext = entity;
            }

            bool entityDeleted = false;
            ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverItems;
            if (ImGui::BeginPopupContextItem()){
                if (ImGui::MenuItem("Delete entity"))
                    entityDeleted = true;
                
                
                if (ImGui::Button("Add Component"))
                    ImGui::OpenPopup("AddComponent");

                    if(ImGui::BeginPopup("AddComponent")){

                        if (ImGui::MenuItem("Camera")){
                            m_SelectionContext.AddComponent<CameraComponent>();
                            ImGui::ClosePopupsExceptModals();
                        }
                        if (ImGui::MenuItem("Sprite Renderer")){
                            m_SelectionContext.AddComponent<SpriteRendererComponent>();
                            ImGui::ClosePopupsExceptModals();
                        }
                        if (ImGui::MenuItem("Mesh")){
                            m_SelectionContext.AddComponent<MeshComponent>();
                            ImGui::ClosePopupsExceptModals();
                        }
                        if (ImGui::MenuItem("Model")){
                            m_SelectionContext.AddComponent<ModelComponent>();
                            ImGui::ClosePopupsExceptModals();
                        }
                        ImGui::EndPopup();
            
                    }
                
                ImGui::EndPopup();
            }

            if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()){
                m_SelectionContext = {};
            }

            if (opened){
                ImGui::TreePop();
            }
            if (entityDeleted) {
                m_Context->DestroyEntity(entity);
                if (m_SelectionContext == entity)
                    m_SelectionContext = {};
            }

    }

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f){

        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());
        
        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight };
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.25f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.7f, 0.05f, 0.075f, 1.0f});
        ImGui::PushFont(boldFont);
        if(ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);
        ImGui::PopFont();
        
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.6f, 0.1f, 1.0f});
        ImGui::PushFont(boldFont);
        if(ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopStyleColor(3);
        ImGui::PopFont();

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.05f, 0.15f, 0.7f, 1.0f});
        ImGui::PushFont(boldFont);
        if(ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopStyleColor(3);
        ImGui::PopFont();

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PopStyleVar();
        ImGui::Columns(1);
        ImGui::PopID();
    }

    template<typename T, typename UIFunction>
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction){
        if(entity.HasComponent<T>()){
            auto& component = entity.GetComponent<T>();
            ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

            const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
            ImGui::PopStyleVar();
            ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);

            if (ImGui::Button("+", ImVec2{lineHeight, lineHeight})){
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings")){
                if (ImGui::MenuItem("Remove Component"))
                    removeComponent = true;


                ImGui::EndPopup();
            }

            if(open){
                uiFunction(component);
                ImGui::TreePop();
            }

            if (removeComponent)
                entity.RemoveComponent<T>();
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if(entity.HasComponent<TagComponent>()){
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), tag.c_str());
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
                tag = std::string(buffer);
            }
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        
        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if(ImGui::BeginPopup("AddComponent")){
            if (ImGui::MenuItem("Camera")){
                m_SelectionContext.AddComponent<CameraComponent>();
                ImGui::CloseCurrentPopup();
            }
           if (ImGui::MenuItem("Sprite Renderer")){
                m_SelectionContext.AddComponent<SpriteRendererComponent>();
                ImGui::CloseCurrentPopup();
            }
           if (ImGui::MenuItem("Mesh")){
                m_SelectionContext.AddComponent<MeshComponent>();
                ImGui::CloseCurrentPopup();
            }
           if (ImGui::MenuItem("Model")){
                m_SelectionContext.AddComponent<ModelComponent>();
                ImGui::CloseCurrentPopup();
            }
                
            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        DrawComponent<TransformComponent>("Transform", entity, [](auto& component){
            DrawVec3Control("Translation", component.Translation);
                glm::vec3 rotation = glm::degrees(component.Rotation);
                DrawVec3Control("Rotation", rotation);
                component.Rotation = glm::radians(rotation);
                DrawVec3Control("Scale", component.Scale, 1.0f);
        });

        DrawComponent<CameraComponent>("Camera", entity, [](auto& component){
            auto& camera = component.Camera;
               
                ImGui::Checkbox("PrimaryCamera", &component.Primary);

                const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
                const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

                if(ImGui::BeginCombo("Projection", currentProjectionTypeString)){
                    for (int i = 0; i < 2; ++i){
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                        if(ImGui::Selectable(projectionTypeStrings[i], isSelected)){
                            currentProjectionTypeString = projectionTypeStrings[i];
                            camera.SetProjectionType((SceneCamera::ProjectionType)i);
                        }

                        if(isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if(camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective){
                    float perspectiveFOV =  glm::degrees(camera.GetPerspectiveVerticalFOV());
                    if(ImGui::DragFloat("FOV", &perspectiveFOV))
                        camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveFOV));
                    
                    float perspectiveNear =  camera.GetPerspectiveNearClip();
                    if(ImGui::DragFloat("Near", &perspectiveNear))
                        camera.SetPerspectiveNearClip(perspectiveNear);
                    
                    float perspectiveFar =  camera.GetPerspectiveFarClip();
                    if(ImGui::DragFloat("Far", &perspectiveFar))
                        camera.SetPerspectiveFarClip(perspectiveFar);
                }


                if(camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic){
                    float orthoSize =  camera.GetOrthographicSize();
                    if(ImGui::DragFloat("Size", &orthoSize))
                        camera.SetOrthographicSize(orthoSize);
                    
                    float orthoNear =  camera.GetOrthographicNearClip();
                    if(ImGui::DragFloat("Near", &orthoNear))
                        camera.SetOrthographicNearClip(orthoNear);
                    
                    float orthoFar =  camera.GetOrthographicFarClip();
                    if(ImGui::DragFloat("Far", &orthoFar))
                        camera.SetOrthographicFarClip(orthoFar);

                    ImGui::Checkbox("FixedAspectRatio", &component.FixedAspectRatio);
                    
                }
        });

        DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [this](auto& component){
                ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
                
                ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
                if (ImGui::BeginDragDropTarget()){
                    if (ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")){

                        Ref<DragDropInfo> info = m_AssetManagerRef->GetDragDropInfo();
                        if (info->contentType == ContentType::Texture){
                            //TODO: move away from this
                            std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / info->itemPath;
                            component.Texture = Texture2D::Create(texturePath.string());
                        }
                    }

                    ImGui::EndDragDropTarget();
                }

                ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 10.0f);
        });


        //Mesh component is deprecated
        DrawComponent<MeshComponent>("Mesh", entity, [this](auto& component){
                
        if(ImGui::Button("Mesh", ImVec2(100.0f, 0.0f)))
            ImGui::OpenPopup("SetMesh");

        if(ImGui::BeginPopup("SetMesh")){
           if (ImGui::MenuItem("Plane")){
                component.mesh = Mesh::CreatePlane();
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Cube")){
                component.mesh = Mesh::CreateCube();
                ImGui::CloseCurrentPopup();
            }
           if (ImGui::MenuItem("Sphere")){
                component.mesh = Mesh::CreateCube();
                ImGui::CloseCurrentPopup();
            }
                
            ImGui::EndPopup();
        }

        });

        DrawComponent<ModelComponent>("Model", entity, [this](auto& component){

        if(ImGui::Button("Model", ImVec2(100.0f, 0.0f)));

        if (ImGui::BeginDragDropTarget()){
                    if (ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")){

                        Ref<DragDropInfo> info = m_AssetManagerRef->GetDragDropInfo();
                        if (info->contentType == ContentType::Model){
                            if (!m_AssetManagerRef->IDExists(info->ID))
                                info->ID = m_AssetManagerRef->LoadAsset(info->itemPath);
                            if (info->ID){
                                component.ID = info->ID;
                                component.model = m_AssetManagerRef->GetAsset<Model>(info->ID).GetContent();
                            }
                            else
                                OIL_CORE_ERROR("Failed to load asset: {0}", info->itemPath);
                        }
                    }

                    ImGui::EndDragDropTarget();
                }

    
        });
        
    }
}