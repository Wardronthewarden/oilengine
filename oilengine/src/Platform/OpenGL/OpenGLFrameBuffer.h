#pragma once

#include "oil/Renderer/FrameBuffer.h"

namespace oil{

    class OpenGLFrameBuffer : public FrameBuffer{
    public:
        OpenGLFrameBuffer(const FrameBufferSpecification& spec);
        virtual ~OpenGLFrameBuffer();

        void Invalidate();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;
        virtual void ClearAttachment(uint32_t attachmentIndex, float value) override;

        virtual void BindColorAttachments();
        virtual void UnbindColorAttachments();


        virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }
        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { OIL_CORE_ASSERT(index < m_ColorAttachments.size(), "The maximum color attachments possible are 6"); return m_ColorAttachments[index]; }

    private:
        uint32_t m_RendererID = 0;
        FrameBufferSpecification m_Specification;

        std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecs;
        FrameBufferTextureSpecification m_DepthAttachmentSpec = FrameBufferTextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
    };

}