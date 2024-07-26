#include "MaterialEditorPanel.h"

namespace oil{
    MaterialEditorPanel::MaterialEditorPanel()
        : m_Open(false)
    {
    }

    MaterialEditorPanel::MaterialEditorPanel(AssetRef<Material> material)
        : m_Open(true)
    {
        OpenMaterialToEdit(material);
    }

    void MaterialEditorPanel::OpenMaterialToEdit(AssetRef<Material> material)
    {
        m_Open = true;
        m_OpenedMaterial = material;
    }
    void MaterialEditorPanel::OnImguiRender()
    {
        if(m_Open){
            ImGui::Begin("Material Editor");
            if(ImGui::Button("Compile"))
                Compile();

            if (ImGui::BeginPopupContextWindow()){
                if(ImGui::MenuItem("New Float")){
                    m_OpenedMaterial->SetUniform("u_NewFloat", 1.0f);
                }
                ImGui::EndPopup();
            }


            for (auto& uniform : m_OpenedMaterial->GetUniformsOfType<float>()){
                std::string materialName = uniform.first.c_str() + 2; 
                ImGui::Text(materialName.c_str());
                ImGui::SameLine();
                ImGui::DragFloat("", &uniform.second);
            } 

            ImGui::End();
        }
    }
    void MaterialEditorPanel::Compile()
    {
        AssetManager::SaveAsset<Material>(m_OpenedMaterial.GetHandle());
    }
}