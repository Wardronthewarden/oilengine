#pragma once

#include <oil.h>

namespace oil{

    class TextureEditorPanel{
    public:
        TextureEditorPanel();
        TextureEditorPanel(AssetRef<Texture2D> texture);

        ~TextureEditorPanel();

        void OnImguiRender();

        void OpenTexture(AssetRef<Texture2D> texture);

        void Save();

        bool IsOpen() { return m_Open; }


    private:
        AssetRef<Texture2D> m_OpenTexture;
        bool m_Open;
    };

}