#include "SceneHierarchyPanel.h"
#include "oil/Scene/Component.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include "utils/UIlib.h"

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
                
                
                if (ImGui::BeginMenu("Add Component")){

                        if (ImGui::MenuItem("Camera")){
                            m_SelectionContext.AddComponent<CameraComponent>();
                        }
                        if (ImGui::MenuItem("Model")){
                            m_SelectionContext.AddComponent<ModelComponent>();
                        }
            
                
                    ImGui::EndMenu();
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
           if (ImGui::MenuItem("Model")){
                m_SelectionContext.AddComponent<ModelComponent>();
                ImGui::CloseCurrentPopup();
            }
                
            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        DrawComponent<TransformComponent>("Transform", entity, [](auto& component){
            UI::DrawVec3Control("Translation", component.Translation);
            glm::vec3 rotation = glm::degrees(component.Rotation);
            UI::DrawVec3Control("Rotation", rotation);
            component.Rotation = glm::radians(rotation);
            UI::DrawVec3Control("Scale", component.Scale, 1.0f);
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
        std::string buttonLabel;
        if(component.model){
            buttonLabel = AssetManager::GetName(component.model);
        }else{
            buttonLabel = "Model";
        }

        if(ImGui::Button("+", ImVec2(50.0f, 50.0f)));

        //drag drop 
        UUID draggedID = UI::AcceptAssetDrop(ContentType::Model);
        if (draggedID){
            component.SetModel(AssetManager::GetAssetReference<Model>(draggedID));  
        }

        ImGui::SameLine();
        ImGui::TextWrapped(buttonLabel.c_str());
        if (!component.model)
            return;
        
        //materials
        for (uint32_t i = 0; i < component.Materials.size(); ++i){


                if(ImGui::Button("+", ImVec2(50.0f, 50.0f)));

                //drag drop 
                UUID draggedID = UI::AcceptAssetDrop(ContentType::Material);
                if (draggedID){
                    component.Materials[i] = draggedID; 
                }

                ImGui::SameLine();
                buttonLabel = AssetManager::GetName(component.Materials[i]);
                ImGui::TextWrapped(buttonLabel.c_str());
        }
    
        });

        
        
    }
}