#pragma once

#include "oil/Renderer/FrameBuffer.h"

namespace oil{

    class OpenGLFrameBuffer : public FrameBuffer{
    public:
        OpenGLFrameBuffer(const FrameBufferSpecification& spec);
        virtual ~OpenGLFrameBuffer();

        void Initialize();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;
        virtual void ClearAttachment(uint32_t attachmentIndex, float value) override;

        virtual void BindColorAttachments() override;
        virtual void BindAttachment(uint32_t attachmentIndex, uint32_t slot) override;


        virtual void BindDepthAttachment() override;

        virtual void SetColorAttachment(Ref<Texture> tgt, uint32_t slotIndex) override;
        virtual void SetDepthAttachment(Ref<Texture> tgt) override;
        virtual void SetAttachmentTextureTarget(uint32_t TextureRendererID, TextureTarget target, uint32_t attachmentIndex) override;

        virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }
        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { OIL_CORE_ASSERT(index < m_ColorAttachments.size(), "The maximum color attachments possible are 6"); return m_ColorAttachments[index]->GetRendererID(); }
        virtual uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment->GetRendererID(); }

    private:
        uint32_t m_RendererID = 0;
        FrameBufferSpecification m_Specification;

        std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecs;
        FrameBufferTextureSpecification m_DepthAttachmentSpec = TextureFormat::None;

        std::vector<Ref<Texture>> m_ColorAttachments;
        Ref<Texture> m_DepthAttachment;
    };

}