#pragma once

#include "oil/Renderer/Texture.h"

#include <glad/glad.h>

namespace oil{
    namespace utils{
        GLenum NativeToGLFormat(TextureFormat format);
        GLenum NativeToGLTextureTarget(TextureTarget target);
    }

    class OpenGLTexture2D : public Texture2D{
    public:
        OpenGLTexture2D(TextureSettings settings, TextureParams params);
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(uint32_t width, uint32_t height, TextureFormat format);
        virtual ~OpenGLTexture2D();
        
        virtual uint32_t GetWidth() const override { return m_Width; };
        virtual uint32_t GetHeight() const override { return m_Height; };
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(DataBuffer<unsigned char> buffer) override;
        virtual void SetData(void* data, uint32_t size) override;

        virtual void Clear(float value) override;
        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual void ResetMetadata(uint32_t width, uint32_t height, TextureFormat format) override;
        virtual DataBuffer<unsigned char> GetData() override;

        
        virtual void SetWrapS(OILTexenum wrapMethod) override;
        virtual void SetWrapT(OILTexenum wrapMethod) override;
        virtual void SetWrapR(OILTexenum wrapMethod) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture& other) const override { 
            return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
        }

    private:
        void SetupTexture();
        void InitTexture();

    private:
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_TexTarget;
        GLenum m_DataFormat, m_FormatComponentType;
    };

    class OpenGLTextureCube : public TextureCube{
    public:
        OpenGLTextureCube(std::vector<Ref<Texture2D>> faces);
        OpenGLTextureCube(uint32_t width, uint32_t height);
        OpenGLTextureCube(uint32_t width, uint32_t height, TextureFormat format);
        virtual ~OpenGLTextureCube();

        virtual uint32_t GetWidth() const override { return m_Width; };
        virtual uint32_t GetHeight() const override { return m_Height; };
        virtual void Clear(float value) override;
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(DataBuffer<unsigned char> buffer) override;
        virtual void SetData(void* data, uint32_t size) override;
        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual void ResetMetadata(uint32_t width, uint32_t height, TextureFormat format) override;
        virtual DataBuffer<unsigned char> GetData() override;
                
        virtual void SetWrapS(OILTexenum wrapMethod) override;
        virtual void SetWrapT(OILTexenum wrapMethod) override;
        virtual void SetWrapR(OILTexenum wrapMethod) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture& other) const override { 
            return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
        }

    private:
        void SetupTexture();

    private:
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_TexTarget;
        GLenum m_DataFormat, m_FormatComponentType;
    };
}