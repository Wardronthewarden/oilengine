#include "pch/oilpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>
#include "OpenGLValidation.h"

namespace oil{

    //Util functions
    namespace utils{

        GLenum NativeToInternalFormat(TextureFormat format){
            switch (format){
                case TextureFormat::RGB8: return GL_RGB;
                case TextureFormat::RGB16: return GL_RGB16;
                
                case TextureFormat::RGBA8: return GL_RGBA8;
                case TextureFormat::RGBA16: return GL_RGBA16;


                case TextureFormat::RGB16F: return GL_RGB16F;
                case TextureFormat::RGBA16F: return GL_RGBA16F;

                case TextureFormat::RED16F: return GL_R16F;
                case TextureFormat::RED16: return GL_R16I;

                case TextureFormat::DEPTH: return GL_DEPTH;
                case TextureFormat::STENCIL: return GL_STENCIL;
                case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
                default: OIL_CORE_ASSERT(false, "Texture format not implemented in [NativeToInternalFormat]!");
                    return GL_NONE;
            }

        }

        GLenum NativeToGLFormat(TextureFormat format){
            switch (format){
                case TextureFormat::RGB8:
                case TextureFormat::RGB16:
                case TextureFormat::RGB16F: return GL_RGB;

                case TextureFormat::RGBA8:
                case TextureFormat::RGBA16:
                case TextureFormat::RGBA16F: return GL_RGBA;

                case TextureFormat::RED16F: return GL_RED;
                case TextureFormat::RED16: return GL_RED_INTEGER;

                case TextureFormat::DEPTH: return GL_DEPTH;
                case TextureFormat::STENCIL: return GL_STENCIL;
                case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL;
                default: OIL_CORE_ASSERT(false, "Texture format not implemented in [NativeToGLFormat]!");
                    return GL_NONE;
            }
            

        }
        
        GLenum NativeToGLTextureTarget(TextureTarget target){
            switch (target){
                case TextureTarget::Texture1D: return GL_TEXTURE_1D;
                case TextureTarget::Texture2D: return GL_TEXTURE_2D;
                case TextureTarget::Texture3D: return GL_TEXTURE_3D;

                case TextureTarget::TextureCube: return GL_TEXTURE_CUBE_MAP;
                case TextureTarget::TextureCube_Xpos: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                case TextureTarget::TextureCube_Xneg: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
                case TextureTarget::TextureCube_Ypos: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
                case TextureTarget::TextureCube_Yneg: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
                case TextureTarget::TextureCube_Zpos: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
                case TextureTarget::TextureCube_Zneg: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
                default: return GL_TEXTURE_2D;
            }
        }

        static GLenum GetFormatComponentType(TextureFormat format){
            switch (format){
                case TextureFormat::RGB8:
                case TextureFormat::RGBA8: return GL_UNSIGNED_BYTE;

                case TextureFormat::RED16: return GL_INT;
                case TextureFormat::RGB16:
                case TextureFormat::RGBA16: return GL_UNSIGNED_SHORT;

                case TextureFormat::RED16F:
                case TextureFormat::RGB16F:
                case TextureFormat::RGBA16F: return GL_HALF_FLOAT;


                case TextureFormat::DEPTH: return GL_DEPTH;
                case TextureFormat::STENCIL: return GL_STENCIL;
                case TextureFormat::DEPTH24STENCIL8: return GL_UNSIGNED_INT_24_8;
                default: OIL_CORE_ASSERT(false, "Texture format not implemented in [GetFormatComponentType]!");
                    return GL_NONE;
            }

        }

        static uint32_t GetNoChannels(TextureFormat format){
            return (uint32_t)format >> 16;
        }

        static uint32_t GetBytesPerChannels(TextureFormat format){
            switch (format){
                case TextureFormat::DEPTH24STENCIL8:
                case TextureFormat::RGB8:
                case TextureFormat::RGBA8: return 1;

                case TextureFormat::RED16:    
                case TextureFormat::RGB16:
                case TextureFormat::RGBA16: return 2;

                case TextureFormat::RGB16F:
                case TextureFormat::RGBA16F: return 4;

                default: OIL_CORE_ASSERT(false, "Texture format not implemented in [GetBytesPerChannels]!");
                    return 0;
            }
        }

    }

    //Texture2D
    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
        : OpenGLTexture2D(width, height, TextureFormat::RGBA8)
    {
    }

    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, TextureFormat format)
        : m_Width(width), m_Height(height)
    {
        m_TextureFormat = format;
        m_DataFormat = utils::NativeToGLFormat(m_TextureFormat);
        SetupTexture();
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::SetData(DataBuffer<unsigned char> buffer)
    {
        uint32_t bpc = utils::GetNoChannels(m_TextureFormat);
        OIL_CORE_ASSERT(buffer.GetSize() == m_Width * m_Height * bpc, "Data must be entire texture!");

        m_Data.SetData(buffer);
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, utils::GetFormatComponentType(m_TextureFormat), m_Data);
    }

    void OpenGLTexture2D::SetData(void *data, uint32_t size)
    {
        uint32_t ch = utils::GetNoChannels(m_TextureFormat);
        uint32_t bpc = utils::GetBytesPerChannels(m_TextureFormat);
        OIL_CORE_ASSERT(size == m_Width * m_Height * ch * bpc, "Data must be entire texture!");

        //TODO: this is not always a correct assumption
        m_Data.SetData((unsigned char*)data, size);
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, utils::GetFormatComponentType(m_TextureFormat), m_Data);
    }

    void OpenGLTexture2D::Clear(float value)
    {
        glClearTexImage(m_RendererID, 0, utils::NativeToInternalFormat(m_TextureFormat), GL_FLOAT, &value);
    }

    void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
    {
        if (m_Width == width && m_Height == height)
            return;
        m_Width = width;
        m_Height = height;
                
        glDeleteTextures(1, &m_RendererID);
        SetupTexture();
    }

    void OpenGLTexture2D::ResetMetadata(uint32_t width, uint32_t height, TextureFormat format)
    {
        m_Width = width;
        m_Height = height;
        m_TextureFormat = format;
        m_DataFormat = utils::NativeToGLFormat(m_TextureFormat);
                
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
        m_TextureTarget = TextureTarget::Texture2D;
        GLenum texTarget = utils::NativeToGLTextureTarget(m_TextureTarget);
        glGenTextures(1, &m_RendererID);
        GL_VALIDATE("Texture generation");
        glBindTexture(texTarget, m_RendererID);
        glTexImage2D(texTarget, 0, utils::NativeToInternalFormat(m_TextureFormat), m_Width, m_Height, 0, m_DataFormat, utils::GetFormatComponentType(m_TextureFormat), NULL);
        GL_VALIDATE("Texture image creation");

        glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, GL_REPEAT); 
        glBindTexture(texTarget, 0);
                
        GL_VALIDATE("Texture setup");
    }

    //CubeMap
    OpenGLTextureCube::OpenGLTextureCube(std::vector<Ref<Texture2D>> faces)
    {
        m_TextureTarget = TextureTarget::TextureCube;
        OIL_CORE_ASSERT(faces.size() == 6, "It is only possible to create a cube map with 6 sides!");
        GLenum texTarget = utils::NativeToGLTextureTarget(m_TextureTarget);
        m_TextureFormat = faces[0]->GetFormat();
        m_DataFormat = utils::NativeToGLFormat(m_TextureFormat);
        m_Width = faces[0]->GetWidth();
        m_Height = faces[0]->GetHeight();

        glGenTextures(1, &m_RendererID);
        glBindTexture(texTarget, m_RendererID);
        GL_VALIDATE("Texture generation");

        for(int i = 0; i< 6; i++){
            OIL_CORE_ASSERT(m_TextureFormat == faces[i]->GetFormat(), "Texture format inconsistent within Cubemap initializer faces!");
            glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, utils::NativeToInternalFormat(m_TextureFormat), m_Width, m_Height, 0, m_DataFormat, utils::GetFormatComponentType(m_TextureFormat), faces[i]->GetData());
            GL_VALIDATE("Texture image creation");
        }

        
        glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
        glBindTexture(texTarget, 0);
        GL_VALIDATE("Texture setup");
    }
    
    OpenGLTextureCube::OpenGLTextureCube(uint32_t width, uint32_t height)
        : OpenGLTextureCube(width, height, TextureFormat::RGBA8)
    {
    }
    OpenGLTextureCube::OpenGLTextureCube(uint32_t width, uint32_t height, TextureFormat format)
        : m_Width(width), m_Height(height)
    {
        m_TextureFormat = format;
        m_DataFormat = utils::NativeToGLFormat(m_TextureFormat);
        SetupTexture();
    }
    OpenGLTextureCube::~OpenGLTextureCube()
    {
        glDeleteTextures(1, &m_RendererID);
    }
    void OpenGLTextureCube::Clear(float value)
    {
        glClearTexImage(m_RendererID, 0, utils::NativeToInternalFormat(m_TextureFormat), GL_FLOAT, &value);
    }
    void OpenGLTextureCube::SetData(DataBuffer<unsigned char> buffer)
    {
        OIL_CORE_ASSERT(false, "Can not set data directly on cube map!");
    }
    void OpenGLTextureCube::SetData(void *data, uint32_t size)
    {

    }
    void OpenGLTextureCube::Resize(uint32_t width, uint32_t height)
    {
        if (m_Width == width && m_Height == height)
            return;
        m_Width = width;
        m_Height = height;
                
        glDeleteTextures(1, &m_RendererID);
        SetupTexture();
    }
    void OpenGLTextureCube::ResetMetadata(uint32_t width, uint32_t height, TextureFormat format)
    {
        m_Width = width;
        m_Height = height;
        m_TextureFormat = format;
        m_DataFormat = utils::NativeToGLFormat(m_TextureFormat);
                
        glDeleteTextures(1, &m_RendererID);
        SetupTexture();
    }
    DataBuffer<unsigned char> OpenGLTextureCube::GetData()
    {
        return DataBuffer<unsigned char>();
    }
    void OpenGLTextureCube::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }
    void OpenGLTextureCube::SetupTexture()
    {
        m_TextureTarget = TextureTarget::TextureCube;
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
        GL_VALIDATE("Texture generation");

        for(int i = 0; i< 6; i++){
            glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, utils::NativeToInternalFormat(m_TextureFormat), m_Width, m_Height, 0, m_DataFormat, utils::GetFormatComponentType(m_TextureFormat), NULL);
            GL_VALIDATE("Texture image creation");
        }

        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
        GL_VALIDATE("Texture setup");
    }
}