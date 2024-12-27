#include "pch/oilpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
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

        GLenum GetInternalFormat(OILTexenum format){
            switch (format){
                case OIL_TEXTURE_FORMAT_RGB8:       return GL_RGB;
                case OIL_TEXTURE_FORMAT_RGB16:      return GL_RGB16;
                
                case OIL_TEXTURE_FORMAT_RGBA8:      return GL_RGBA8;
                case OIL_TEXTURE_FORMAT_RGBA16:     return GL_RGBA16;


                case OIL_TEXTURE_FORMAT_RGB16F:     return GL_RGB16F;
                case OIL_TEXTURE_FORMAT_RGBA16F:    return GL_RGBA16F;

                case OIL_TEXTURE_FORMAT_RED16F:     return GL_R16F;
                case OIL_TEXTURE_FORMAT_RED16:      return GL_R16I;

                case OIL_TEXTURE_FORMAT_DEPTH:      return GL_DEPTH;
                case OIL_TEXTURE_FORMAT_STENCIL:    return GL_STENCIL;
                case OIL_TEXTURE_FORMAT_DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
                default: OIL_CORE_ASSERT(false, "Texture format not implemented in [GetInternalFormat]!");
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

        GLenum GetGLBaseFormat(OILTexenum format){
            switch (format){
                case OIL_TEXTURE_FORMAT_RGB8:
                case OIL_TEXTURE_FORMAT_RGB16:
                case OIL_TEXTURE_FORMAT_RGB16F: return GL_RGB;

                case OIL_TEXTURE_FORMAT_RGBA8:
                case OIL_TEXTURE_FORMAT_RGBA16:
                case OIL_TEXTURE_FORMAT_RGBA16F: return GL_RGBA;

                case OIL_TEXTURE_FORMAT_RED16F: return GL_RED;
                case OIL_TEXTURE_FORMAT_RED16: return GL_RED_INTEGER;

                case OIL_TEXTURE_FORMAT_DEPTH: return GL_DEPTH;
                case OIL_TEXTURE_FORMAT_STENCIL: return GL_STENCIL;
                case OIL_TEXTURE_FORMAT_DEPTH24STENCIL8: return GL_DEPTH_STENCIL;
                default: OIL_CORE_ASSERT(false, "Texture format not implemented in [GetGLBaseFormat]!");
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

        GLenum NativeToGLTextureTarget(OILTexenum target){
            if (target & OIL_TEXTURE_ARRAY){
                switch (target & 0x0000000f){
                    case OIL_TEXTURE_1D: return GL_TEXTURE_1D_ARRAY;
                    case OIL_TEXTURE_2D: return GL_TEXTURE_2D_ARRAY;

                    case OIL_TEXTURE_CUBE: return GL_TEXTURE_CUBE_MAP_ARRAY;

                    default: OIL_CORE_ASSERT(false, "Illegal texture target definition!");
                }
            }
            
            if (target & OIL_TEXTURE_MULTISAMPLE){
                switch (target & 0x0000000f){
                    case OIL_TEXTURE_2D: return GL_TEXTURE_2D_MULTISAMPLE;
                    case OIL_TEXTURE_3D: return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;

                    default: OIL_CORE_ASSERT(false, "Illegal texture target definition!");
                }
            }
            
            
            switch (target & 0x0000000f){
                case OIL_TEXTURE_1D: return GL_TEXTURE_1D;
                case OIL_TEXTURE_2D: return GL_TEXTURE_2D;
                case OIL_TEXTURE_3D: return GL_TEXTURE_3D;

                case OIL_TEXTURE_CUBE: return GL_TEXTURE_CUBE_MAP;

                default: OIL_CORE_ASSERT(false, "Illegal texture target definition!");
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
                case TextureFormat::RGBA16F: return GL_FLOAT;


                case TextureFormat::DEPTH: return GL_DEPTH;
                case TextureFormat::STENCIL: return GL_STENCIL;
                case TextureFormat::DEPTH24STENCIL8: return GL_UNSIGNED_INT_24_8;
                default: OIL_CORE_ASSERT(false, "Texture format not implemented in [GetFormatComponentType]!");
                    return GL_NONE;
            }

        }

        static GLenum GetFormatComponentType(OILTexenum texFormat){
            if (texFormat > 0x0004ffff){
                switch (texFormat){
                    case OIL_TEXTURE_FORMAT_DEPTH: return GL_DEPTH;
                    case OIL_TEXTURE_FORMAT_STENCIL: return GL_STENCIL;
                    case OIL_TEXTURE_FORMAT_DEPTH24STENCIL8: return GL_UNSIGNED_INT_24_8;

                    default: OIL_CORE_ASSERT(false, "Texture format not implemented in [GetFormatComponentType]!");
                        return GL_NONE;
                }
            }else{    
                switch (texFormat & 0x000000ff){
                    case 0x00000001: return GL_UNSIGNED_BYTE;
                    case 0x00000002: return GL_UNSIGNED_SHORT;
                    case 0x00000003: return GL_INT;

                    case 0x00000012: return GL_HALF_FLOAT;
                    case 0x00000013: return GL_FLOAT;

                    default: OIL_CORE_ASSERT(false, "Texture format not implemented in [GetFormatComponentType]!");
                        return GL_NONE;
                }

            }

        }

        static uint32_t GetNoChannels(TextureFormat format){
            return (uint32_t)format >> 16;
        }
        static uint32_t GetNoChannels(OILTexenum format){
            return format >> 16;
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

        static uint32_t GetBytesPerChannels(OILTexenum format){
            switch (format){
                case OIL_TEXTURE_FORMAT_DEPTH24STENCIL8:
                case OIL_TEXTURE_FORMAT_RGB8:
                case OIL_TEXTURE_FORMAT_RGBA8: return 1;

                case OIL_TEXTURE_FORMAT_RED16:    
                case OIL_TEXTURE_FORMAT_RGB16:
                case OIL_TEXTURE_FORMAT_RGBA16: return 2;

                case OIL_TEXTURE_FORMAT_RGB16F:
                case OIL_TEXTURE_FORMAT_RGBA16F: return 4;

                default: OIL_CORE_ASSERT(false, "Texture format not implemented in [GetBytesPerChannels]!");
                    return 0;
            }
        }

    }

    OpenGLTexture2D::OpenGLTexture2D(TextureSettings settings, TextureParams params)
    {
        m_Settings = settings;
        m_Params = params;
        InitTexture();
    }

    // Texture2D
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
        uint32_t ch = utils::GetNoChannels(m_TextureFormat);
        uint32_t bpc = utils::GetBytesPerChannels(m_TextureFormat);
        OIL_CORE_ASSERT(buffer.GetSize() == m_Width * m_Height * bpc * ch, "Data must be entire texture!");

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, utils::GetFormatComponentType(m_TextureFormat), buffer);
    }

    void OpenGLTexture2D::SetData(void *data, uint32_t size)
    {
        uint32_t ch = utils::GetNoChannels(m_TextureFormat);
        uint32_t bpc = utils::GetBytesPerChannels(m_TextureFormat);
        OIL_CORE_ASSERT(size == m_Width * m_Height * ch * bpc, "Data must be entire texture!");

        //TODO: this is not always a correct assumption
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, utils::GetFormatComponentType(m_TextureFormat), data);
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
        glBindTexture(m_TexTarget, m_RendererID);
        uint32_t ch = utils::GetNoChannels(m_TextureFormat);
        uint32_t bpc = utils::GetBytesPerChannels(m_TextureFormat);
        uint32_t size = m_Width * m_Height * ch * bpc;
        unsigned char* imgData = new unsigned char[size];
        glGetTextureImage(m_RendererID, 0, m_DataFormat, utils::GetFormatComponentType(m_TextureFormat), size, (void*)imgData);
        return DataBuffer<unsigned char>((unsigned char*)imgData,size);
    }

    void OpenGLTexture2D::SetWrapS(OILTexenum wrapMethod)
    {
        switch(wrapMethod){
            case OIL_TEXTURE_WRAP_CLAMP_TO_EDGE:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                break;
            case OIL_TEXTURE_WRAP_CLAMP_TO_BORDER:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                break;
            case OIL_TEXTURE_WRAP_REPEAT:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_REPEAT:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_CLAMP_EDGE:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
                break;
            default:
                OIL_CORE_ASSERT(false, "Invalid wrapping method passed to texture 2D");
        }
    }

    void OpenGLTexture2D::SetWrapT(OILTexenum wrapMethod)
    {
        switch(wrapMethod){
            case OIL_TEXTURE_WRAP_CLAMP_TO_EDGE:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                break;
            case OIL_TEXTURE_WRAP_CLAMP_TO_BORDER:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                break;
            case OIL_TEXTURE_WRAP_REPEAT:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_REPEAT:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_CLAMP_EDGE:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
                break;
            default:
                OIL_CORE_ASSERT(false, "Invalid wrapping method passed to texture 2D");
        }
    }

    void OpenGLTexture2D::SetWrapR(OILTexenum wrapMethod)
    {
        switch(wrapMethod){
            case OIL_TEXTURE_WRAP_CLAMP_TO_EDGE:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                break;
            case OIL_TEXTURE_WRAP_CLAMP_TO_BORDER:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
                break;
            case OIL_TEXTURE_WRAP_REPEAT:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_R, GL_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_REPEAT:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_CLAMP_EDGE:
                glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_R, GL_MIRROR_CLAMP_TO_EDGE);
                break;
            default:
                OIL_CORE_ASSERT(false, "Invalid wrapping method passed to texture 2D");
        }
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }
    void OpenGLTexture2D::SetupTexture()
    {
        m_TextureTarget = TextureTarget::Texture2D;
        m_TexTarget = utils::NativeToGLTextureTarget(m_TextureTarget);
        glGenTextures(1, &m_RendererID);
        GL_VALIDATE("Texture generation");
        glBindTexture(m_TexTarget, m_RendererID);
        glTexImage2D(m_TexTarget, 0, utils::NativeToInternalFormat(m_TextureFormat), m_Width, m_Height, 0, m_DataFormat, utils::GetFormatComponentType(m_TextureFormat), NULL);
        GL_VALIDATE("Texture image creation");

        glTexParameteri(m_TexTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(m_TexTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(m_TexTarget, GL_TEXTURE_WRAP_R, GL_REPEAT); 
        glBindTexture(m_TexTarget, 0);
                
        GL_VALIDATE("Texture setup");
    }

    void OpenGLTexture2D::InitTexture()
    {
        //Get texture target
        bool multisample = m_Settings.StorageType & OIL_TEXTURE_MULTISAMPLE;
        m_TexTarget = multisample  ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        glGenTextures(1, &m_RendererID);
        glBindTexture(m_TexTarget, m_RendererID);

        //Get storage allocation arguments
        GLint internalFormat = utils::GetInternalFormat(m_Settings.TextureFormat);

        //Allocate the proper type of storage
        //Note that we mip level is also used to define sample count in multisample textures
        if (m_Settings.StorageType & OIL_TEXTURE_STORAGE_MUTABLE){
            if (multisample){
                glTexImage2DMultisample(m_TexTarget, m_Settings.MipLevels, internalFormat, m_Settings.Width, m_Settings.Height, GL_TRUE);
            }else{
                glTexImage2D(m_TexTarget, m_Settings.MipLevels, internalFormat, m_Settings.Width, m_Settings.Height, 0, GL_RGB8, GL_UNSIGNED_BYTE, NULL);
            }
        }else{
            if (multisample){
                glTexStorage2DMultisample(m_TexTarget, m_Settings.MipLevels, internalFormat, m_Settings.Width, m_Settings.Height, GL_TRUE);
            }else{
                glTexStorage2D(m_TexTarget, m_Settings.MipLevels, internalFormat, m_Settings.Width, m_Settings.Height);
            }
        }

        GL_VALIDATE("Texture image creation");

        //set up texture params
        //base and max levels
        glTexParameterfv(m_TexTarget, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(m_Params.BorderColor));
        glTexParameteri(m_TexTarget, GL_TEXTURE_BASE_LEVEL, m_Params.TexBaseLevel);
        glTexParameteri(m_TexTarget, GL_TEXTURE_MAX_LEVEL, m_Params.TexMaxLevel);
        //min filter
        if (!multisample && m_Settings.MipLevels){
            switch(m_Params.MinFilter){
                case OIL_TEXTURE_FILTER_NEAREST | OIL_TEXTURE_MIPFILTER_NEAREST :
                    glTexParameteri(m_TexTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                    break;
                case OIL_TEXTURE_FILTER_NEAREST | OIL_TEXTURE_MIPFILTER_LINEAR :
                    glTexParameteri(m_TexTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                    break;
                case OIL_TEXTURE_FILTER_LINEAR | OIL_TEXTURE_MIPFILTER_NEAREST :
                    glTexParameteri(m_TexTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                    break;
                case OIL_TEXTURE_FILTER_LINEAR | OIL_TEXTURE_MIPFILTER_LINEAR :
                    glTexParameteri(m_TexTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    break;
                default:
                    OIL_CORE_ASSERT(false, "Invalid minification filter method passed to mipmap texture2D!");
            }
        }else{
            switch(m_Params.MinFilter){
                case OIL_TEXTURE_FILTER_NEAREST:
                    glTexParameteri(m_TexTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    break;
                case OIL_TEXTURE_FILTER_LINEAR:
                    glTexParameteri(m_TexTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    break;
                default:
                    OIL_CORE_ASSERT(false, "Invalid minification filter method passed to texture2D!");
            }
        }

        //mag filter
        switch(m_Params.MagFilter){
            case OIL_TEXTURE_FILTER_NEAREST:
                glTexParameteri(m_TexTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case OIL_TEXTURE_FILTER_LINEAR:
                glTexParameteri(m_TexTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            default:
                OIL_CORE_ASSERT(false, "Invalid magnification filter method passed to texture2D!");
        }

        //wrapping
        GLenum paramName;
        uint32_t wrapParams = m_Params.TexWrap;
        for(int i = 0; i < 3; ++i){
            switch (i){
                case 0: paramName = GL_TEXTURE_WRAP_S; break;
                case 1: paramName = GL_TEXTURE_WRAP_T; break;
                case 2: paramName = GL_TEXTURE_WRAP_R; break;
            }
            switch(wrapParams & 0x000000ff){
                case OIL_TEXTURE_WRAP_S_CLAMP_TO_EDGE:
                    glTexParameteri(m_TexTarget, paramName, GL_CLAMP_TO_EDGE);
                    break;
                case OIL_TEXTURE_WRAP_S_CLAMP_TO_BORDER:
                    glTexParameteri(m_TexTarget, paramName, GL_CLAMP_TO_BORDER);
                    break;
                case OIL_TEXTURE_WRAP_S_REPEAT:
                    glTexParameteri(m_TexTarget, paramName, GL_REPEAT);
                    break;
                case OIL_TEXTURE_WRAP_S_MIRROR_REPEAT:
                    glTexParameteri(m_TexTarget, paramName, GL_MIRRORED_REPEAT);
                    break;
                case OIL_TEXTURE_WRAP_S_MIRROR_CLAMP_EDGE:
                    glTexParameteri(m_TexTarget, paramName, GL_MIRROR_CLAMP_TO_EDGE);
                    break;
            }
            wrapParams = wrapParams << 8;
        }
        glBindTexture(m_TexTarget, 0);
                
        GL_VALIDATE("Texture setup");
    }

    //CubeMap
    OpenGLTextureCube::OpenGLTextureCube(std::vector<Ref<Texture2D>> faces)
    {
        m_TextureTarget = TextureTarget::TextureCube;
        OIL_CORE_ASSERT(faces.size() == 6, "It is only possible to create a cube map with 6 sides!");
        //Setup internal data
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
        m_TextureTarget = TextureTarget::TextureCube;
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
        OIL_CORE_ASSERT(false, "Can not set data directly on cube map!");
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
    void OpenGLTextureCube::SetWrapS(OILTexenum wrapMethod)
    {
        bool multisample = m_Settings.StorageType & OIL_TEXTURE_MULTISAMPLE;
        GLenum texTarget = multisample  ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        switch(wrapMethod){
            case OIL_TEXTURE_WRAP_CLAMP_TO_EDGE:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                break;
            case OIL_TEXTURE_WRAP_CLAMP_TO_BORDER:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                break;
            case OIL_TEXTURE_WRAP_REPEAT:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_REPEAT:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_CLAMP_EDGE:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
                break;
            default:
                OIL_CORE_ASSERT(false, "Invalid wrapping method passed to texture 2D");
        }
    }
    void OpenGLTextureCube::SetWrapT(OILTexenum wrapMethod)
    {
        bool multisample = m_Settings.StorageType & OIL_TEXTURE_MULTISAMPLE;
        GLenum texTarget = multisample  ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        switch(wrapMethod){
            case OIL_TEXTURE_WRAP_CLAMP_TO_EDGE:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                break;
            case OIL_TEXTURE_WRAP_CLAMP_TO_BORDER:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                break;
            case OIL_TEXTURE_WRAP_REPEAT:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_REPEAT:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_CLAMP_EDGE:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
                break;
            default:
                OIL_CORE_ASSERT(false, "Invalid wrapping method passed to texture 2D");
        }
    }
    void OpenGLTextureCube::SetWrapR(OILTexenum wrapMethod)
    {
        bool multisample = m_Settings.StorageType & OIL_TEXTURE_MULTISAMPLE;
        GLenum texTarget = multisample  ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        switch(wrapMethod){
            case OIL_TEXTURE_WRAP_CLAMP_TO_EDGE:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                break;
            case OIL_TEXTURE_WRAP_CLAMP_TO_BORDER:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
                break;
            case OIL_TEXTURE_WRAP_REPEAT:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, GL_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_REPEAT:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
                break;
            case OIL_TEXTURE_WRAP_MIRROR_CLAMP_EDGE:
                glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, GL_MIRROR_CLAMP_TO_EDGE);
                break;
            default:
                OIL_CORE_ASSERT(false, "Invalid wrapping method passed to texture 2D");
        }
    }
    void OpenGLTextureCube::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }
    void OpenGLTextureCube::SetupTexture()
    {
        GLenum texTarget = utils::NativeToGLTextureTarget(m_TextureTarget);
        
        glGenTextures(1, &m_RendererID);
        glBindTexture(texTarget, m_RendererID);
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