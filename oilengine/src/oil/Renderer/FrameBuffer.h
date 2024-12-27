#pragma once

#include <oil/core/core.h>
#include "Texture.h"

namespace oil{

    struct FrameBufferTextureSpecification{
        FrameBufferTextureSpecification() = default;
        FrameBufferTextureSpecification(TextureFormat format)
            : TextureFormat(format){}

        TextureFormat TextureFormat = TextureFormat::None;
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

        //Binds color attachments, startin from texture slot 0
        virtual void BindColorAttachments() = 0;
        virtual void BindAttachment(uint32_t attachmentIndex, uint32_t slot) = 0;

        //Binds the depth attachment to texture slot == num color attachments
        virtual void BindDepthAttachment() = 0;

        virtual void SetColorAttachment(Ref<Texture> tgt, uint32_t slotIndex) = 0;
        virtual void SetDepthAttachment(Ref<Texture> tgt) = 0;
        virtual void SetAttachmentTextureTarget(TextureTarget target, uint32_t attachmentIndex) = 0;
        virtual void SetAttachmentTextureTargetLayer(uint32_t attachmentIndex, uint32_t layerDepthIndex) = 0;

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