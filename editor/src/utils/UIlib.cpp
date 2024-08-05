#include "UIlib.h"
#include "imgui_internal.h"
#include "oil/storage/AssetManager.h"

namespace oil{

bool UI::DrawFloatControl(const std::string &label, float& value, float resetValue, float columnWidth)
{
    bool ret = false;
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());
    
    ImGui::Columns(2);

    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight };
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.25f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.7f, 0.05f, 0.075f, 1.0f});
    ImGui::PushFont(boldFont);
    if(ImGui::Button("X", buttonSize)){
        value = resetValue;
        ret = true;
    }

    ImGui::PopStyleColor(3);
    ImGui::PopFont();
    
    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &value, 0.01f))
        ret = true;
    ImGui::PopItemWidth();

    
    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
    return ret;
}

bool UI::DrawVec2Control(const std::string &label, glm::vec2 &values, float resetValue, float columnWidth)
{
    bool ret = false;
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());
    
    ImGui::Columns(2);

    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight };
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.25f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.7f, 0.05f, 0.075f, 1.0f});
    ImGui::PushFont(boldFont);
    if(ImGui::Button("X", buttonSize)){
        values.x = resetValue;
        ret = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();
    
    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &values.x, 0.01f))
        ret = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.6f, 0.1f, 1.0f});
    ImGui::PushFont(boldFont);
    if(ImGui::Button("Y", buttonSize)){
        values.y = resetValue;
        ret = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &values.y, 0.01f))
        ret = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();

        
    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
    return ret;
}

bool UI::DrawVec3Control(const std::string &label, glm::vec3 &values, float resetValue, float columnWidth)
{
    bool ret = false;
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
    if(ImGui::Button("X", buttonSize)){
        values.x = resetValue;
        ret = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();
    
    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &values.x, 0.01f))
        ret = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.6f, 0.1f, 1.0f});
    ImGui::PushFont(boldFont);
    if(ImGui::Button("Y", buttonSize)){
        values.y = resetValue;
        ret = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &values.y, 0.01f))
        ret = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.05f, 0.15f, 0.7f, 1.0f});
    ImGui::PushFont(boldFont);
    if(ImGui::Button("Z", buttonSize)){
        values.z = resetValue;
        ret = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::SameLine();
    if (ImGui::DragFloat("##Z", &values.z, 0.01f))
        ret = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
    return ret;
}

bool UI::DrawVec4Control(const std::string &label, glm::vec4 &values, float resetValue, float columnWidth)
{
    bool ret = false;
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());
    
    ImGui::Columns(2);

    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight };
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.25f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.7f, 0.05f, 0.075f, 1.0f});
    ImGui::PushFont(boldFont);
    if(ImGui::Button("X", buttonSize)){
        values.x = resetValue;
        ret = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();
    
    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &values.x, 0.01f))
        ret = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.6f, 0.1f, 1.0f});
    ImGui::PushFont(boldFont);
    if(ImGui::Button("Y", buttonSize)){
        values.y = resetValue;
        ret = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &values.y, 0.01f))
        ret = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.05f, 0.15f, 0.7f, 1.0f});
    ImGui::PushFont(boldFont);
    if(ImGui::Button("Z", buttonSize)){
        values.z = resetValue;
        ret = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::SameLine();
    if (ImGui::DragFloat("##Z", &values.z, 0.01f))
        ret = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
   
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.8f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.9f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.7f, 0.7f, 0.7f, 1.0f});
    ImGui::PushFont(boldFont);
    if(ImGui::Button("W", buttonSize)){
        values.z = resetValue;
        ret = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::SameLine();
    if (ImGui::DragFloat("##W", &values.w, 0.01f))
        ret = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
    return ret;
}

bool UI::DrawAssetItemBrowser(Ref<Texture2D> thumbnail, std::string& name, bool nameEditMode, float thumbnailSize, float padding)
{
    bool changed = false;
    ImGui::BeginGroup();


    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::ImageButton((ImTextureID)thumbnail->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});


    ImGui::PopStyleColor();

    if(nameEditMode){
        char buff[32];
        memcpy(buff, name.c_str(), 32);
        changed = ImGui::InputText("##ContentNameInput", buff, 32, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
        name = std::string(buff);
        ImGui::SetKeyboardFocusHere(-1);
    }else{
        ImGui::TextWrapped(name.c_str());
    }


    ImGui::EndGroup();
    return changed;
}

void UI::DrawAssetItemList(Ref<Texture2D> thumbnail, std::string& name,  float thumbnailSize, float padding, float widthLimit)
{
    ImGui::BeginGroup();
    ImGui::ImageButton((ImTextureID)thumbnail->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

    ImGui::SameLine();
    ImGui::TextWrapped(name.c_str());
    ImGui::EndGroup();
}

UUID UI::AcceptAssetDrop(ContentType type)
{
    if (ImGui::BeginDragDropTarget()){
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OIL_ASSET")){
            OIL_ASSERT(payload->DataSize == sizeof(UUID), "Payload size mismatch!");
            UUID draggedID = *(UUID*)payload->Data;

            if (AssetManager::GetType(draggedID) == type){
                OIL_ASSERT(AssetManager::IsValid(draggedID), "Invalid handle dragged!");
                ImGui::EndDragDropTarget();
                return draggedID;
            }
        }

        ImGui::EndDragDropTarget();
    }
    return 0;
}
}