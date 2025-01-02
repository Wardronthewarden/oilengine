#pragma once

#include "oil/Renderer/Texture.h"

#include <glad/glad.h>

namespace oil{
    namespace utils{
        GLenum NativeToGLFormat(OILTexenum format);
        GLenum NativeToGLTextureTarget(OILTexenum target);
    }

    class OpenGLTexture2D : public Texture2D{
    public:
        OpenGLTexture2D(TextureSettings settings, TextureParams params);
        virtual ~OpenGLTexture2D();
        
        virtual uint32_t GetWidth() const override { return m_Settings.Width; };
        virtual uint32_t GetHeight() const override { return m_Settings.Height; };
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(DataBuffer<unsigned char> buffer) override;
        virtual void SetData(void* data, uint32_t size) override;
        virtual DataBuffer<unsigned char> GetData() override;

        virtual void Clear(float value) override;
        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual void GenerateMipmaps() override;
        
        virtual void SetWrapS(OILTexenum wrapMethod) override;
        virtual void SetWrapT(OILTexenum wrapMethod) override;
        virtual void SetWrapR(OILTexenum wrapMethod) override;
        virtual void SetMinFilter(OILTexenum filterMethod) override;
        virtual void SetMagFilter(OILTexenum filterMethod) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture& other) const override { 
            return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
        }

    private:
        void InitTexture();

    private:
        uint32_t m_RendererID;
        GLenum m_TexTarget;
        GLenum m_DataFormat, m_FormatComponentType;
    };

    class OpenGLTextureCube : public TextureCube{
    public:
        OpenGLTextureCube(TextureSettings settings, TextureParams params);
        virtual ~OpenGLTextureCube();

        virtual uint32_t GetWidth() const override { return m_Settings.Width; };
        virtual uint32_t GetHeight() const override { return m_Settings.Height; };
        virtual uint32_t GetRendererID() const override { return m_RendererID; }


        virtual void Clear(float value) override;
        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual void GenerateMipmaps() override;

        virtual void SetData(DataBuffer<unsigned char> buffer) override;
        virtual void SetData(void* data, uint32_t size) override;
        virtual DataBuffer<unsigned char> GetData() override;
                
        virtual void SetWrapS(OILTexenum wrapMethod) override;
        virtual void SetWrapT(OILTexenum wrapMethod) override;
        virtual void SetWrapR(OILTexenum wrapMethod) override;
        virtual void SetMinFilter(OILTexenum filterMethod) override;
        virtual void SetMagFilter(OILTexenum filterMethod) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture& other) const override { 
            return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
        }

    private:
        void InitTexture();

    private:
        uint32_t m_RendererID;
        GLenum m_TexTarget;
        GLenum m_DataFormat, m_FormatComponentType;
    };
}