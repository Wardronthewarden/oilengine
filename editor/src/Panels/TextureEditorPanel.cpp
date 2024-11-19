#include "TextureEditorPanel.h"
#include "utils/UIlib.h"
namespace oil{
    TextureEditorPanel::TextureEditorPanel()
        : m_Open(false)
    {

    }

    TextureEditorPanel::TextureEditorPanel(AssetRef<Texture2D> texture)
        : m_Open(true)
    {
        OpenTexture(texture);
    }

    TextureEditorPanel::~TextureEditorPanel()
    {
    }
    void TextureEditorPanel::OnImguiRender()
    {
        if (m_Open){
            ImGui::Begin("Texture Editor");

            ImGui::Image((ImTextureID)AssetManager::GetAsset<Texture2D>(AssetManager::GetHandleByName("Checkerboard"))->GetRendererID(), ImGui::GetContentRegionAvail(), {0, 0}, {1, 1});
            //ImGui::Image((ImTextureID)m_OpenTexture->GetRendererID(), ImGui::GetContentRegionAvail(), {0, 0}, {1, 1});

            ImGui::End();
        }
    }
    void TextureEditorPanel::OpenTexture(AssetRef<Texture2D> texture)
    {
        m_Open = true;
        m_OpenTexture = texture;
    }
    void TextureEditorPanel::Save()
    {
    }
}