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

    class FrameBuffer{
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
        virtual void ClearAttachment(uint32_t attachmentIndex, float value) = 0;

        //bind attachments to  texture slots, incremented from startIndex.
        //the startIndex returns increased by the number of slots bound as color attachments
        virtual void BindColorAttachments() = 0;
        virtual void UnbindColorAttachments() = 0;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

        virtual const FrameBufferSpecification& GetSpecification() const = 0;

        static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
    }; 

}