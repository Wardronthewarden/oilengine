#include "pch/oilpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLValidation.h"

namespace oil{

    //Util functions
    namespace utils{

        GLenum GetInternalFormat(OILTexenum format){
            switch (format){
                case OIL_TEXTURE_FORMAT_RG8:       return GL_RG;
                case OIL_TEXTURE_FORMAT_RG16:      return GL_RG16;

                case OIL_TEXTURE_FORMAT_RGB8:       return GL_RGB;
                case OIL_TEXTURE_FORMAT_RGB16:      return GL_RGB16;
                
                case OIL_TEXTURE_FORMAT_RGBA8:      return GL_RGBA8;
                case OIL_TEXTURE_FORMAT_RGBA16:     return GL_RGBA16;



                case OIL_TEXTURE_FORMAT_RG16F:      return GL_RG16F;
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

        GLenum GetGLBaseFormat(OILTexenum format){
            switch (format){
                case OIL_TEXTURE_FORMAT_RG8:
                case OIL_TEXTURE_FORMAT_RG16:
                case OIL_TEXTURE_FORMAT_RG16F: return GL_RG;
                
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

                    case 0x00000012: return GL_FLOAT;
                    case 0x00000013: return GL_FLOAT;

                    default: OIL_CORE_ASSERT(false, "Texture format not implemented in [GetFormatComponentType]!");
                        return GL_NONE;
                }

            }

        }

        static uint32_t GetNoChannels(OILTexenum format){
            return format >> 16;
        }

        static uint32_t GetBytesPerChannels(OILTexenum format){
            switch (format){
                case OIL_TEXTURE_FORMAT_DEPTH24STENCIL8:
                case OIL_TEXTURE_FORMAT_RED8:
                case OIL_TEXTURE_FORMAT_RG8:
                case OIL_TEXTURE_FORMAT_RGB8:
                case OIL_TEXTURE_FORMAT_RGBA8: return 1;

                case OIL_TEXTURE_FORMAT_RED16:    
                case OIL_TEXTURE_FORMAT_RG16:
                case OIL_TEXTURE_FORMAT_RGB16:
                case OIL_TEXTURE_FORMAT_RGBA16: return 2;

                case OIL_TEXTURE_FORMAT_RG16F:
                case OIL_TEXTURE_FORMAT_RGB16F:
                case OIL_TEXTURE_FORMAT_RGBA16F: return 4;

                default: OIL_CORE_ASSERT(false, "Texture format not implemented in [GetBytesPerChannels]!");
                    return 0;
            }
        }

    }


    // Texture2D
    OpenGLTexture2D::OpenGLTexture2D(TextureSettings settings, TextureParams params)
    {
        m_Settings = settings;
        m_Params = params;
        InitTexture();
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::SetData(DataBuffer<unsigned char> buffer)
    {
        uint32_t ch = utils::GetNoChannels(m_Settings.TextureFormat);
        uint32_t bpc = utils::GetBytesPerChannels(m_Settings.TextureFormat);
        OIL_CORE_ASSERT(buffer.GetSize() == m_Settings.Width * m_Settings.Height * bpc * ch, "Data must be entire texture!");

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Settings.Width, m_Settings.Height, m_DataFormat, m_FormatComponentType, buffer);
    }

    void OpenGLTexture2D::SetData(void *data, uint32_t size)
    {
        uint32_t ch = utils::GetNoChannels(m_Settings.TextureFormat);
        uint32_t bpc = utils::GetBytesPerChannels(m_Settings.TextureFormat);
        OIL_CORE_ASSERT(size == m_Settings.Width * m_Settings.Height * ch * bpc, "Data must be entire texture!");

        //TODO: this is not always a correct assumption
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Settings.Width, m_Settings.Height, m_DataFormat, m_FormatComponentType, data);
    }

    void OpenGLTexture2D::Clear(float value)
    {
        glClearTexImage(m_RendererID, 0, utils::GetInternalFormat(m_Settings.TextureFormat), GL_FLOAT, &value);
    }

    void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
    {
        if (m_Settings.Width == width && m_Settings.Height == height)
            return;
        m_Settings.Width = width;           
        m_Settings.Height = height;

        glDeleteTextures(1, &m_RendererID);
        InitTexture();
    }

    void OpenGLTexture2D::GenerateMipmaps()
    {
        glBindTexture(m_TexTarget, m_RendererID);
        glGenerateMipmap(m_TexTarget);

        float mipW = std::log2(m_Settings.Width);
        float mipH = std::log2(m_Settings.Height);
        m_Settings.MipLevels = mipW > mipH ? mipH : mipW;
    }

    DataBuffer<unsigned char> OpenGLTexture2D::GetData()
    {
        glBindTexture(m_TexTarget, m_RendererID);
        uint32_t ch = utils::GetNoChannels(m_Settings.TextureFormat);
        uint32_t bpc = utils::GetBytesPerChannels(m_Settings.TextureFormat);
        uint32_t size = m_Settings.Width * m_Settings.Height * ch * bpc;
        unsigned char* imgData = new unsigned char[size];
        glGetTextureImage(m_RendererID, 0, m_DataFormat, m_FormatComponentType, size, (void*)imgData);
        return DataBuffer<unsigned char>((unsigned char*)imgData,size);
    }

    void OpenGLTexture2D::SetWrapS(OILTexenum wrapMethod)
    {
        glBindTexture(m_TexTarget, m_RendererID);
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
        glBindTexture(m_TexTarget, m_RendererID);
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
        glBindTexture(m_TexTarget, m_RendererID);
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

    void OpenGLTexture2D::SetMinFilter(OILTexenum filterMethod)
    {
        m_Params.MinFilter = filterMethod;
        if (!( m_Settings.StorageType & OIL_TEXTURE_MULTISAMPLE) && m_Settings.MipLevels){
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
    }

    void OpenGLTexture2D::SetMagFilter(OILTexenum filterMethod)
    {
        m_Params.MinFilter = filterMethod;
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
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }

    void OpenGLTexture2D::InitTexture()
    {
        //Get texture target
        bool multisample = m_Settings.StorageType & OIL_TEXTURE_MULTISAMPLE;
        m_TexTarget = multisample  ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

        //Set utility members
        m_DataFormat = utils::GetGLBaseFormat(m_Settings.TextureFormat);
        m_FormatComponentType = utils::GetFormatComponentType(m_Settings.TextureFormat);

        glGenTextures(1, &m_RendererID);
        glBindTexture(m_TexTarget, m_RendererID);

        //Get storage allocation arguments
        GLint internalFormat = utils::GetInternalFormat(m_Settings.TextureFormat);

        //Allocate the proper type of storage
        //Note that we mip level is also used to define sample count in multisample textures
        int mip = 0, mipWidth, mipHeight;
        mipWidth = m_Settings.Width;
        mipHeight = m_Settings.Height;
        while (mip <= m_Settings.MipLevels){
            if (m_Settings.StorageType & OIL_TEXTURE_STORAGE_MUTABLE){
                if (multisample){
                    glTexImage2DMultisample(m_TexTarget, mip, internalFormat, mipWidth, mipHeight, GL_TRUE);
                }else{
                    glTexImage2D(m_TexTarget, mip, internalFormat, mipWidth, mipHeight, 0, m_DataFormat, utils::GetFormatComponentType(m_Settings.TextureFormat), NULL);
                }
            }else{
                if (multisample){
                    glTexStorage2DMultisample(m_TexTarget, mip, internalFormat, mipWidth, mipHeight, GL_TRUE);
                }else{
                    glTexStorage2D(m_TexTarget, mip, internalFormat, mipWidth, mipHeight);
                }
            }

            if (mipWidth == 1 || mipHeight == 1){
                m_Settings.MipLevels = mip;
                break;
            }
            mipWidth /=2;
            mipHeight /=2;
            ++mip;
        }
        GL_VALIDATE("Texture image creation");

        m_Params.TexMaxLevel = m_Settings.MipLevels;

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
    OpenGLTextureCube::OpenGLTextureCube(TextureSettings settings, TextureParams params)
    {
        m_Settings = settings;
        m_Params = params;
        InitTexture();
    }
    OpenGLTextureCube::~OpenGLTextureCube()
    {
        glDeleteTextures(1, &m_RendererID);
    }
    void OpenGLTextureCube::Clear(float value)
    {
        glClearTexImage(m_RendererID, 0, utils::GetInternalFormat(m_Settings.TextureFormat), GL_FLOAT, &value);
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
        if (m_Settings.Width == width && m_Settings.Height == height)
            return;
        m_Settings.Width = width;           
        m_Settings.Height = height;

        glDeleteTextures(1, &m_RendererID);
        InitTexture();
    }
    void OpenGLTextureCube::GenerateMipmaps()
    {
        glBindTexture(m_TexTarget, m_RendererID);
        glGenerateMipmap(m_TexTarget);

        float mipW = std::log2(m_Settings.Width);
        float mipH = std::log2(m_Settings.Height);
        m_Settings.MipLevels = mipW > mipH ? mipH : mipW;

    }
    DataBuffer<unsigned char> OpenGLTextureCube::GetData()
    {
        return DataBuffer<unsigned char>();
    }
    void OpenGLTextureCube::SetWrapS(OILTexenum wrapMethod)
    {
        glBindTexture(m_TexTarget, m_RendererID);
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
        glBindTexture(m_TexTarget, m_RendererID);
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
        glBindTexture(m_TexTarget, m_RendererID);
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
    void OpenGLTextureCube::SetMinFilter(OILTexenum filterMethod)
    {
        m_Params.MinFilter = filterMethod;
        if (!( m_Settings.StorageType & OIL_TEXTURE_MULTISAMPLE) && m_Settings.MipLevels){
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
                    OIL_CORE_ASSERT(false, "Invalid minification filter method passed to mipmap texture cube!");
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
                    OIL_CORE_ASSERT(false, "Invalid minification filter method passed to texture cube!");
            }
        }
    }
    void OpenGLTextureCube::SetMagFilter(OILTexenum filterMethod)
    {
        m_Params.MinFilter = filterMethod;
        switch(m_Params.MagFilter){
            case OIL_TEXTURE_FILTER_NEAREST:
                glTexParameteri(m_TexTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case OIL_TEXTURE_FILTER_LINEAR:
                glTexParameteri(m_TexTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            default:
                OIL_CORE_ASSERT(false, "Invalid magnification filter method passed to texture cube!");
        }
    }
    void OpenGLTextureCube::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }
    void OpenGLTextureCube::InitTexture()
    {
        //Get texture target
        bool multisample = m_Settings.StorageType & OIL_TEXTURE_MULTISAMPLE;
        if (multisample){
            OIL_CORE_WARN("Ignored multisampling request; not supported on cube maps!");
            m_Settings.StorageType &= ~OIL_TEXTURE_MULTISAMPLE;
        }
        bool arrayTexture = m_Settings.StorageType & OIL_TEXTURE_ARRAY;
        m_TexTarget = arrayTexture  ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;

        //Set utility members
        m_DataFormat = utils::GetGLBaseFormat(m_Settings.TextureFormat);
        m_FormatComponentType = utils::GetFormatComponentType(m_Settings.TextureFormat);

        glGenTextures(1, &m_RendererID);
        glBindTexture(m_TexTarget, m_RendererID);

        //Get storage allocation arguments
        GLint internalFormat = utils::GetInternalFormat(m_Settings.TextureFormat);
        
        //Allocate the proper type of storage
        //Note that for cubemap arrays, Depth is the number of cubes, not the number of total faces!
        int mip = 0, mipWidth, mipHeight;
        mipWidth = m_Settings.Width;
        mipHeight = m_Settings.Height;
        while (mip <= m_Settings.MipLevels){

            if (m_Settings.StorageType & OIL_TEXTURE_STORAGE_MUTABLE){
                if (arrayTexture){
                    glTexImage3D(m_TexTarget, mip, internalFormat, mipWidth, mipHeight, m_Settings.Depth * 6, 0, m_DataFormat, utils::GetFormatComponentType(m_Settings.TextureFormat), NULL);
                    GL_VALIDATE("Texture image creation");
                }else{
                    for (int i = 0; i < 6; ++i){
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, internalFormat, mipWidth, mipHeight, 0, m_DataFormat, utils::GetFormatComponentType(m_Settings.TextureFormat), NULL);
                        GL_VALIDATE("Texture image creation");
                    }
                }
            }else{
                if (arrayTexture){
                    glTexStorage3D(m_TexTarget, mip, internalFormat, mipWidth, mipHeight, m_Settings.Depth * 6);
                    GL_VALIDATE("Texture image creation");
                }else{
                    for (int i = 0; i < 6; ++i){
                        glTexStorage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, internalFormat, mipWidth, mipHeight);
                        GL_VALIDATE("Texture image creation");
                    }
                }
            }

            if (mipWidth == 1 || mipHeight == 1){
                m_Settings.MipLevels = mip;
                break;
            }
            mipWidth /=2;
            mipHeight /=2;
            ++mip;
        }

        m_Params.TexMaxLevel = m_Settings.MipLevels;
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
                    OIL_CORE_ASSERT(false, "Invalid minification filter method passed to mipmap texture cube!");
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
                    OIL_CORE_ASSERT(false, "Invalid minification filter method passed to texture cube!");
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
                OIL_CORE_ASSERT(false, "Invalid magnification filter method passed to texture cube!");
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
}