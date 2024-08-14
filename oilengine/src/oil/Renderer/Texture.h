#pragma once

#include <string>
#include "oil/core/core.h"
#include "Buffer.h"
#include <oil/storage/Asset.h>
namespace oil {

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
        RGBA32F     = 0x00040013
    };

    class Texture{
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual TextureFormat GetFormat() const = 0;
        virtual uint32_t GetRendererID() const = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;

        virtual void SetData(DataBuffer<unsigned char> buffer) = 0;
        virtual void SetData(void* data, uint32_t size) = 0;
        virtual void ResetMetadata(uint32_t width, uint32_t height, TextureFormat format) = 0;
        virtual DataBuffer<unsigned char> GetData() = 0;

        virtual bool operator==(const Texture& other) const = 0;
    };

    class Texture2D : public Texture{
    public:
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);
        static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFormat format);
        static Ref<Texture2D> Create(const std::string& path);


    };

    class TextureCube : public Texture{
    public:
        static Ref<TextureCube> Create(std::vector<Ref<Texture2D>> faces);
    };

}