#pragma once

#include <oil/core/core.h>

namespace oil{

    enum class FrameBufferTextureFormat{
        None = 0,

        //Color
        RGB8,
        RGBA8,
        RGB16F,
        RGBA16F,
        R_INT,
        R_FLOAT,

        //Depth/stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
    };

    struct FrameBufferTextureSpecification{
        FrameBufferTextureSpecification() = default;
        FrameBufferTextureSpecification(FrameBufferTextureFormat format)
            : TextureFormat(format){}

        FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
        // TODO: filtering and wrap
    };

    struct FrameBufferAttachmentSpecification{
        FrameBufferAttachmentSpecification() = default;
        FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
            : Attachments(attachments){}

        std::vector<FrameBufferTextureSpecification> Attachments;
    };

    struct FrameBufferSpecification{
        uint32_t Width, Height;
        FrameBufferAttachmentSpecification Attachments;
        uint32_t Samples = 1;

        bool SwapChainTarget = false;
    };

    class FrameBufferTarget{
    public:

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Clear(uint32_t value) = 0;
        virtual void Clear(float value) = 0;
        
        virtual inline uint32_t GetRendererID() = 0;
        virtual inline uint32_t GetWidth() = 0;
        virtual inline uint32_t GetHeight() = 0;
        virtual inline FrameBufferTextureFormat GetTextureFormat() = 0;

        static Ref<FrameBufferTarget> Create(FrameBufferTextureFormat format, uint32_t width, uint32_t height);

    };

    class FrameBuffer{
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
        virtual void ClearAttachment(uint32_t attachmentIndex, float value) = 0;

        //Binds color attachments, startin from texture slot 0
        virtual void BindColorAttachments() = 0;
        virtual void UnbindColorAttachments() = 0;

        //Binds the depth attachment to texture slot == num color attachments
        virtual void BindDepthAttachment() = 0;
        virtual void UnbindDepthAttachment() = 0;

        virtual void SetColorAttachment(Ref<FrameBufferTarget> tgt, uint32_t slotIndex) = 0;
        virtual void SetDepthAttachment(Ref<FrameBufferTarget> tgt) = 0;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
        virtual uint32_t GetDepthAttachmentRendererID() const = 0;

        virtual const FrameBufferSpecification& GetSpecification() const = 0;

        static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
    }; 

    class FrameBufferManager{
    public:
        void Init();

        virtual void Add(std::string bufferName, Ref<FrameBuffer> buffer) = 0;
        virtual Ref<FrameBuffer> Get(std::string bufferName) = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void ShareTexture(std::string src, std::string tgt, uint32_t texIndex) = 0;
    };

}