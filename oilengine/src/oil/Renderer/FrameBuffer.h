#pragma once

#include <oil/core/core.h>
#include "Texture.h"

namespace oil{

    struct FrameBufferTextureSpecification{
        FrameBufferTextureSpecification() = default;
        FrameBufferTextureSpecification(OILTexenum format)
            : TextureFormat(format){}

        OILTexenum TextureFormat = 0;
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

        virtual void SetColorAttachmentFormat(OILTexenum format, uint32_t attachmentIndex) = 0;

        virtual void WrapTargetTexture(Ref<Texture> tgt, uint32_t miplevel = 0) = 0;

        virtual void SetColorAttachment(Ref<Texture> tgt, uint32_t attachmentIndex) = 0;
        virtual void SetDepthAttachment(Ref<Texture> tgt) = 0;
        virtual void SetAttachmentTargetLayer(uint32_t attachmentIndex, uint32_t layerDepthIndex, uint32_t miplevel = 0) = 0;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
        virtual uint32_t GetDepthAttachmentRendererID() const = 0;

        virtual const FrameBufferSpecification& GetSpecification() const = 0;

        static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
    }; 

}