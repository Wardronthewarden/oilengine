#pragma once

#include "oil/Renderer/Texture.h"

#include <glad/glad.h>

namespace oil{
    class OpenGLTexture2D : public Texture2D{
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(uint32_t width, uint32_t height, TextureFormat format);
        OpenGLTexture2D(const std::string& path);
        virtual ~OpenGLTexture2D();
        
        virtual uint32_t GetWidth() const override { return m_Width; };
        virtual uint32_t GetHeight() const override { return m_Height; };
        virtual TextureFormat GetFormat() const override { return m_InternalFormat; };
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(DataBuffer<unsigned char> buffer) override;
        virtual void SetData(void* data, uint32_t size) override;
        virtual void ResetMetadata(uint32_t width, uint32_t height, TextureFormat format) override;
        virtual DataBuffer<unsigned char> GetData() override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture& other) const override { 
            return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
        }

    private:
        void SetupTexture();

    private:
        uint32_t m_Width, m_Height;
        TextureFormat m_InternalFormat;
        uint32_t m_RendererID;
        std::string m_Path;
        GLenum m_DataFormat, m_FormatComponentType;
        DataBuffer<unsigned char> m_Data;
    };

    class OpenGLTextureCube : public TextureCube{
    public:
        OpenGLTextureCube(std::vector<Ref<Texture2D>> faces);

        virtual uint32_t GetWidth() const override { return m_Width; };
        virtual uint32_t GetHeight() const override { return m_Height; };
        virtual TextureFormat GetFormat() const override { return m_InternalFormat; };
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(DataBuffer<unsigned char> buffer) override;
        virtual void SetData(void* data, uint32_t size) override;
        virtual void ResetMetadata(uint32_t width, uint32_t height, TextureFormat format) override;
        virtual DataBuffer<unsigned char> GetData() override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture& other) const override { 
            return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
        }

    private:
        uint32_t m_Width, m_Height;
        TextureFormat m_InternalFormat;
        uint32_t m_RendererID;

    };
}