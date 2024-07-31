#include "MaterialEditorPanel.h"
#include "utils/UIlib.h"

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
        m_OpenedMaterial->UpdateUniforms();

    }
    void MaterialEditorPanel::OnImguiRender()
    {
        if(m_Open){
            ImGui::Begin("Material Editor");
            if(ImGui::Button("</>")){
                std::string command = "code " + AssetManager::GetSource(m_OpenedMaterial->GetShader()).string();
                std::system(command.c_str());
            }
                //open shader file with code

            ImGui::SameLine();

            if(ImGui::Button("Recompile"))
                m_OpenedMaterial->GetShader()->Recompile(AssetManager::GetSource(m_OpenedMaterial).string());

            ImGui::SameLine();    
            if(ImGui::Button("Save"))
                Save();

            ImGui::SameLine();    
            if(ImGui::Button("Close"))
                m_Open = false;

            if (ImGui::BeginPopupContextWindow()){
                if(ImGui::MenuItem("New Float")){
                    m_OpenedMaterial->SetUniform("u_NewFloat", 1.0f);
                }
                ImGui::EndPopup();
            }

            for (auto& uniform : m_OpenedMaterial->GetShader()->GetUniformNames()){
                std::string uniformName = uniform.Name.c_str() + 2; 
                switch (uniform.Type){
                    case UniformType::Float:{
                        float value = m_OpenedMaterial->GetUniform<float>(uniform.Name);
                        if (UI::DrawFloatControl(uniformName, value, 0.0f, 100)){
                            m_OpenedMaterial->SetUniform(uniform.Name, value);
                        }
                        break;
                    }
                    case UniformType::Float2:{
                        glm::vec2 value = m_OpenedMaterial->GetUniform<glm::vec2>(uniform.Name);
                        if (UI::DrawVec2Control(uniformName, value, 0.0f, 100)){
                            m_OpenedMaterial->SetUniform(uniform.Name, value);
                        }
                        break;
                    }
                    case UniformType::Float3:{
                        glm::vec3 value = m_OpenedMaterial->GetUniform<glm::vec3>(uniform.Name);
                        if (UI::DrawVec3Control(uniformName, value, 0.0f, 100)){
                            m_OpenedMaterial->SetUniform(uniform.Name, value);
                        }
                        break;
                    }
                    case UniformType::Float4:{
                        glm::vec4 value = m_OpenedMaterial->GetUniform<glm::vec4>(uniform.Name);
                        if (UI::DrawVec4Control(uniformName, value, 0.0f, 100)){
                            m_OpenedMaterial->SetUniform(uniform.Name, value);
                        }
                        break;
                    }
                    default: continue;
                }
            }

            ImGui::End();
        }
    }
    void MaterialEditorPanel::Save()
    {
        AssetManager::SaveAsset<Material>(m_OpenedMaterial.GetHandle());
    }
}