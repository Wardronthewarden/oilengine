#pragma once

#include "oil/Renderer/FrameBuffer.h"

namespace oil{

    class OpenGLFrameBufferTarget : public FrameBufferTarget{
    public:
        OpenGLFrameBufferTarget(FrameBufferTextureFormat format, uint32_t width, uint32_t height);

        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Clear(uint32_t value) override;
        virtual void Clear(float value) override;
        
        virtual inline uint32_t GetRendererID() override;
        virtual inline uint32_t GetWidth() override;
        virtual inline uint32_t GetHeight() override;
        virtual inline FrameBufferTextureFormat GetTextureFormat() override;
    private:
        uint32_t m_Width, m_Height;
        FrameBufferTextureFormat m_TextureFormat = FrameBufferTextureFormat::None;
        uint32_t m_RendererID;
    };

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

        virtual void BindColorAttachments() override;
        virtual void UnbindColorAttachments() override;

        virtual void BindDepthAttachment() override;
        virtual void UnbindDepthAttachment() override;

        virtual void CopyDepthAttachment(Ref<FrameBuffer> other) override;

        virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }
        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { OIL_CORE_ASSERT(index < m_ColorAttachments.size(), "The maximum color attachments possible are 6"); return m_ColorAttachments[index]; }
        virtual uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

    private:
        uint32_t m_RendererID = 0;
        FrameBufferSpecification m_Specification;

        std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecs;
        FrameBufferTextureSpecification m_DepthAttachmentSpec = FrameBufferTextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
    };

}