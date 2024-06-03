#include "pch/oilpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace oil{

    //Util functions
    static GLenum NativeToInternalFormat(TextureFormat format){
        switch (format){
            case TextureFormat::RGB8: return GL_RGB8;
            case TextureFormat::RGB16: return GL_RGB16;
            
            case TextureFormat::RGBA8: return GL_RGBA8;
            case TextureFormat::RGBA16: return GL_RGBA16;
        }

    }

    static GLenum NativeToGLFormat(TextureFormat format){
        switch (format){
            case TextureFormat::RGB8:
            case TextureFormat::RGB16: return GL_RGB;

            case TextureFormat::RGBA8:
            case TextureFormat::RGBA16: return GL_RGBA;
        }

    }

    static GLenum GetFormatComponentType(TextureFormat format){
        switch (format){
            case TextureFormat::RGB8:
            case TextureFormat::RGBA8: return GL_UNSIGNED_BYTE;

            case TextureFormat::RGB16:
            case TextureFormat::RGBA16: return GL_UNSIGNED_SHORT;
        }

    }

    static uint32_t GetNoChannels(TextureFormat format){
        return (uint32_t)format >> 16;
    }


    //Texture2D
    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
    {
        OpenGLTexture2D(width, height, TextureFormat::RGBA8);
    }

    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, TextureFormat format)
        : m_Width(width), m_Height(height), m_InternalFormat(format)
    {
        m_DataFormat = NativeToGLFormat(m_InternalFormat);
        SetupTexture();
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string &path)
        :m_Path(path)
    {
        int width, height, channels;

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        OIL_CORE_ASSERT(data, "Failed to load image!");
        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4){
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
            m_InternalFormat = TextureFormat::RGBA8;
        } else if (channels == 3){
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
            m_InternalFormat = TextureFormat::RGB8;
        }

        m_DataFormat = dataFormat;

        OIL_CORE_ASSERT(internalFormat && dataFormat, "Format not supported");

        SetupTexture();

        glTextureSubImage2D(m_RendererID, 0,0,0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

        m_Data.SetData(data, m_Width * m_Height * channels);
        //stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::SetData(DataBuffer<unsigned char> buffer)
    {
    }

    void OpenGLTexture2D::SetData(void *data, uint32_t size)
    {
        uint32_t bpc = GetNoChannels(m_InternalFormat);
        OIL_CORE_ASSERT(size == m_Width * m_Height * bpc, "Data must be entire texture!");


        m_Data.SetData((unsigned char*)data, size);
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GetFormatComponentType(m_InternalFormat), m_Data);
    }

    void OpenGLTexture2D::ResetMetadata(uint32_t width, uint32_t height, TextureFormat format)
    {
        m_Width = width;
        m_Height = height;
        m_InternalFormat = format;
        m_DataFormat = NativeToGLFormat(m_InternalFormat);
                
        glDeleteTextures(1, &m_RendererID);
        SetupTexture();
    }

    DataBuffer<unsigned char> OpenGLTexture2D::GetData()
    {
        return m_Data;
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }
    void OpenGLTexture2D::SetupTexture()
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, NativeToInternalFormat(m_InternalFormat), m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
}