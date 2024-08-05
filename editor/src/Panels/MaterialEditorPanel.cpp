#include "MaterialEditorPanel.h"
#include "utils/UIlib.h"

namespace oil{
    MaterialEditorPanel::MaterialEditorPanel()
        : m_Open(false)
    {
        m_EmptyThumbnail = Texture2D::Create("Internal/Assets/src/Textures/FileIcon.png");;
    }

    MaterialEditorPanel::MaterialEditorPanel(AssetRef<Material> material)
        : m_Open(true)
    {
        m_EmptyThumbnail = Texture2D::Create("Internal/Assets/src/Textures/FileIcon.png");;
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

            if(ImGui::Button("Recompile")){
                m_OpenedMaterial->GetShader()->Recompile(AssetManager::GetSource(m_OpenedMaterial->GetShader()).string());
                m_OpenedMaterial->UpdateUniforms();
            }

            ImGui::SameLine();    
            if(ImGui::Button("Save"))
                Save();

            ImGui::SameLine();    
            if(ImGui::Button("Close"))
                m_Open = false;


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
                    case UniformType::Texture2D:{
                        AssetRef<Texture2D> texRef = m_OpenedMaterial->GetTexture<Texture2D>(uniform.Name);
                        if(texRef){
                            UI::DrawAssetItemList(texRef, uniformName, 50.0f, 0.0f, 100.0f);
                        }else{
                            std::string label = "empty texture";
                            UI::DrawAssetItemList(m_EmptyThumbnail, label, 50.0f, 0.0f, 100.0f);
                        }
                        UUID id = UI::AcceptAssetDrop(ContentType::Texture2D);
                        if (id){
                            m_OpenedMaterial->SetTexture<Texture2D>(uniform.Name, AssetManager::GetAssetReference<Texture2D>(id));
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