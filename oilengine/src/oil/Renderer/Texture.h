#pragma once

#include <string>
#include "oil/core/core.h"
#include "Buffer.h"
#include <oil/storage/Asset.h>
#include <glm/glm.hpp>
namespace oil {

    typedef uint32_t OILTexenum;
    //Texture storage settings definitions
    //Storage type
    #define OIL_TEXTURE_STORAGE_IMMUTABLE       0
    #define OIL_TEXTURE_STORAGE_MUTABLE         1 << 16

    //storage dimensions
    #define OIL_TEXTURE_1D                      0x00000001
    #define OIL_TEXTURE_2D                      0x00000002
    #define OIL_TEXTURE_3D                      0x00000003
    #define OIL_TEXTURE_CUBE                    0x00000006

    #define OIL_TEXTURE_ARRAY                   1 << 6
    #define OIL_TEXTURE_MULTISAMPLE             1 << 8

    //Texture formats
    #define OIL_TEXTURE_FORMAT_RED8             0x00010001
    #define OIL_TEXTURE_FORMAT_RG8              0x00020001
    #define OIL_TEXTURE_FORMAT_RGB8             0x00030001
    #define OIL_TEXTURE_FORMAT_RGBA8            0x00040001
    #define OIL_TEXTURE_FORMAT_RED16            0x00010002
    #define OIL_TEXTURE_FORMAT_RG16             0x00020002
    #define OIL_TEXTURE_FORMAT_RGB16            0x00030002
    #define OIL_TEXTURE_FORMAT_RGBA16           0x00040002
    #define OIL_TEXTURE_FORMAT_RED32            0x00010003
    #define OIL_TEXTURE_FORMAT_RG32             0x00020003
    #define OIL_TEXTURE_FORMAT_RGB32            0x00030003
    #define OIL_TEXTURE_FORMAT_RGBA32           0x00040003
    
    #define OIL_TEXTURE_FORMAT_RED16F           0x00010012
    #define OIL_TEXTURE_FORMAT_RG16F            0x00020012
    #define OIL_TEXTURE_FORMAT_RGB16F           0x00030012
    #define OIL_TEXTURE_FORMAT_RGBA16F          0x00040012
    #define OIL_TEXTURE_FORMAT_RED32F           0x00010013
    #define OIL_TEXTURE_FORMAT_RG32F            0x00020013
    #define OIL_TEXTURE_FORMAT_RGB32F           0x00030013
    #define OIL_TEXTURE_FORMAT_RGBA32F          0x00040013

    #define OIL_TEXTURE_FORMAT_DEPTH            0x00050001
    #define OIL_TEXTURE_FORMAT_STENCIL          0x00060001
    #define OIL_TEXTURE_FORMAT_DEPTH24STENCIL8  0x00070001

    //Texture parameters
    //Texture filtering
    #define OIL_TEXTURE_FILTER_NEAREST          0x00000000
    #define OIL_TEXTURE_FILTER_LINEAR           0x00000001
    #define OIL_TEXTURE_MIPFILTER_NEAREST       0x00000000
    #define OIL_TEXTURE_MIPFILTER_LINEAR        0x00000002

    //Texture wrapping
    #define OIL_TEXTURE_WRAP_CLAMP_TO_EDGE          0x00000000
    #define OIL_TEXTURE_WRAP_CLAMP_TO_BORDER        0x00000001
    #define OIL_TEXTURE_WRAP_REPEAT                 0x00000002
    #define OIL_TEXTURE_WRAP_MIRROR_REPEAT          0x00000003
    #define OIL_TEXTURE_WRAP_MIRROR_CLAMP_EDGE      0x00000004
    
    #define OIL_TEXTURE_WRAP_S_CLAMP_TO_EDGE        0x00000000
    #define OIL_TEXTURE_WRAP_S_CLAMP_TO_BORDER      0x00000001
    #define OIL_TEXTURE_WRAP_S_REPEAT               0x00000002
    #define OIL_TEXTURE_WRAP_S_MIRROR_REPEAT        0x00000003
    #define OIL_TEXTURE_WRAP_S_MIRROR_CLAMP_EDGE    0x00000004
    
    #define OIL_TEXTURE_WRAP_T_CLAMP_TO_EDGE        0x00000000
    #define OIL_TEXTURE_WRAP_T_CLAMP_TO_BORDER      0x00000001 << 8
    #define OIL_TEXTURE_WRAP_T_REPEAT               0x00000002 << 8
    #define OIL_TEXTURE_WRAP_T_MIRROR_REPEAT        0x00000003 << 8
    #define OIL_TEXTURE_WRAP_T_MIRROR_CLAMP_EDGE    0x00000004 << 8
    
    #define OIL_TEXTURE_WRAP_R_CLAMP_TO_EDGE        0x00000000
    #define OIL_TEXTURE_WRAP_R_CLAMP_TO_BORDER      0x00000001 << 16
    #define OIL_TEXTURE_WRAP_R_REPEAT               0x00000002 << 16
    #define OIL_TEXTURE_WRAP_R_MIRROR_REPEAT        0x00000003 << 16
    #define OIL_TEXTURE_WRAP_R_MIRROR_CLAMP_EDGE    0x00000004 << 16


    enum class TextureFormat{
        //Channels
        None = 0,

        RED8        = 0x00010001,
        RG8         = 0x00020001,
        RGB8        = 0x00030001,
        RGBA8       = 0x00040001,
        RED16       = 0x00010002,
        RG16        = 0x00020002,
        RGB16       = 0x00030002,
        RGBA16      = 0x00040002,
        RED32       = 0x00010003,
        RG32        = 0x00020003,
        RGB32       = 0x00030003,
        RGBA32      = 0x00040003,
        
        RED16F      = 0x00010012,
        RG16F       = 0x00020012,
        RGB16F      = 0x00030012,
        RGBA16F     = 0x00040012,
        RED32F      = 0x00010013,
        RG32F       = 0x00020013,
        RGB32F      = 0x00030013,
        RGBA32F     = 0x00040013,

        DEPTH           = 0x00050001,
        STENCIL         = 0x00060001,
        DEPTH24STENCIL8 = 0x00070001
    };

    enum class TextureTarget{
        None = 0,

        Texture1D               = 0x00000001,
        Texture2D               = 0x00000002,
        Texture2D_Multisample   = 0x00000012,
        Texture3D               = 0x00000003,
        Texture3D_Multisample   = 0x00000013,

        TextureCube             = 0x00010000,
        TextureCube_Xpos        = 0x00010001,
        TextureCube_Xneg        = 0x00010002,
        TextureCube_Ypos        = 0x00010003,
        TextureCube_Yneg        = 0x00010004,
        TextureCube_Zpos        = 0x00010005,
        TextureCube_Zneg        = 0x00010006
    };

    TextureTarget operator +(TextureTarget t, int i);


    typedef struct TextureSettings{
        OILTexenum StorageType;
        OILTexenum TextureFormat;
        uint32_t Width      {0};
        uint32_t Height     {0};
        uint32_t Depth      {0};
        uint32_t MipLevels  {0};
    } TextureSettings;
    
    typedef struct TextureParams{
        glm::vec4 BorderColor   {0.0,0.0,0.0,0.0};
        uint32_t TexBaseLevel   {0};
        uint32_t TexMaxLevel    {1000};
        OILTexenum MinFilter    {OIL_TEXTURE_FILTER_NEAREST};
        OILTexenum MagFilter    {OIL_TEXTURE_FILTER_NEAREST};
        OILTexenum TexWrap      {OIL_TEXTURE_WRAP_S_CLAMP_TO_EDGE};
    } TextureParams;

    typedef struct SamplerSettings{

    } SamplerSettings;

    class Texture{
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;

        virtual void SetData(DataBuffer<unsigned char> buffer) = 0;
        virtual void SetData(void* data, uint32_t size) = 0;
        virtual void Clear(float value) = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void ResetMetadata(uint32_t width, uint32_t height, TextureFormat format) = 0;
        virtual DataBuffer<unsigned char> GetData() = 0;

        virtual void SetWrapS(OILTexenum wrapMethod) = 0;
        virtual void SetWrapT(OILTexenum wrapMethod) = 0;
        virtual void SetWrapR(OILTexenum wrapMethod) = 0;

        virtual bool operator==(const Texture& other) const = 0;

        inline TextureTarget GetTarget() const { return m_TextureTarget; }
        inline TextureFormat GetFormat() const { return m_TextureFormat; }

    protected:
        TextureSettings m_Settings;
        TextureParams m_Params;
        TextureTarget m_TextureTarget = TextureTarget::None;
        TextureFormat m_TextureFormat = TextureFormat::None;
    };

    class Texture2D : public Texture{
    public:
        static Ref<Texture2D> Create(TextureSettings settings, TextureParams params);
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);
        static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFormat format);
    };

    class TextureCube : public Texture{
    public:
        static Ref<TextureCube> Create(std::vector<Ref<Texture2D>> faces);
        static Ref<TextureCube> Create(uint32_t width, uint32_t height);
        static Ref<TextureCube> Create(uint32_t width, uint32_t height, TextureFormat format);
    };

}