#pragma once
#include <oil.h>

namespace oil{

    class MaterialEditorPanel{
    public:
        MaterialEditorPanel();
        MaterialEditorPanel(AssetRef<Material> material);

        void OpenMaterialToEdit(AssetRef<Material> material);

        void OnImguiRender();

        void Save();

        bool IsOpen() { return m_Open; }

    private:
        AssetRef<Material> m_OpenedMaterial;

        bool m_Open;
        Ref<Texture2D> m_EmptyThumbnail;

    };

}