#include "pch/oilpch.h"
#include "OpenGLFrameBuffer.h"

#include "glad/glad.h"

namespace oil{
    static const uint32_t s_MaxFrameBufferSize = 8192;

    namespace utils{

        static GLenum TextureTarget(bool multisampled){
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        }

        static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count){
            glCreateTextures(TextureTarget(multisampled), count, outID);
        }

        static void BindTexture(bool multisampled, uint32_t id){
            glBindTexture(TextureTarget(multisampled), id);
        }

        static bool IsDepthFormat(FrameBufferTextureFormat format){
            switch (format){
                case FrameBufferTextureFormat::DEPTH24STENCIL8 : return true;
            }
            return false;
        }

        static void AttachColorTexture(uint32_t id, int samples, int index){
            bool multisampled = samples > 1;

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
        }

        static void AttachDepthTexture(uint32_t id, int samples, GLenum attachmentType){
            bool multisampled = samples > 1;

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
        }

        static GLenum OiltextureFormatToGL(FrameBufferTextureFormat format){
            switch (format){
                case FrameBufferTextureFormat::RGB8:
                    return GL_RGB8;
                case FrameBufferTextureFormat::RGBA8:
                    return GL_RGBA8;
                case FrameBufferTextureFormat::RGB16F:
                    return GL_RGB16F;
                case FrameBufferTextureFormat::RGBA16F:
                    return GL_RGBA16F;
                case FrameBufferTextureFormat::R_INT:
                    return GL_RED_INTEGER;
                case FrameBufferTextureFormat::R_FLOAT:
                    return GL_RED;
            }

            OIL_CORE_ASSERT(false, "Unknown FrameBufferTextureFormat");
            return 0;
        }


    }
    OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
        :m_Specification(spec)
    {
        for (auto format : m_Specification.Attachments.Attachments){
            if(!utils::IsDepthFormat(format.TextureFormat))
                m_ColorAttachmentSpecs.emplace_back(format);
            else
                m_DepthAttachmentSpec = format;
        }

        Initialize();
    }
    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
    }
    void OpenGLFrameBuffer::Initialize()
    {
        if(!m_RendererID){
            glGenFramebuffers(1, &m_RendererID);
            glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        }


        bool multisample = m_Specification.Samples > 1;

        // Attachments
        if (m_ColorAttachmentSpecs.size()){
            m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());

            for (int i = 0; i < m_ColorAttachmentSpecs.size(); ++i){
                SetColorAttachment(FrameBufferTarget::Create(m_ColorAttachmentSpecs[i].TextureFormat, m_Specification.Width, m_Specification.Height), i);
            }   
        }

        if (m_DepthAttachmentSpec.TextureFormat != FrameBufferTextureFormat::None){
            SetDepthAttachment(FrameBufferTarget::Create(m_DepthAttachmentSpec.TextureFormat, m_Specification.Width, m_Specification.Height));
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        if (m_ColorAttachments.size() > 1){
            OIL_CORE_ASSERT(m_ColorAttachments.size() <= 10, "The maximum color attachments possible are 10");
            const int size = m_ColorAttachments.size();
            GLenum* buffers = new GLenum[size];
            for (int i = 0; i < size; i++)
                buffers[i] =  GL_COLOR_ATTACHMENT0 + i;
            
            glDrawBuffers(m_ColorAttachments.size(), buffers);
            delete buffers;

        }else if(m_ColorAttachments.empty()){
            glDrawBuffer(GL_NONE);
        }


        OIL_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindBuffer(GL_FRAMEBUFFER, 0);
    }
    void OpenGLFrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);
        

    }
    void OpenGLFrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        if( width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize){
            OIL_CORE_WARN("Attempted to resize frame buffer to ({0}, {1})", width, height);
            return;
        }

        m_Specification.Width = width;
        m_Specification.Height = height;
        int i = 0;
        for (auto& target : m_ColorAttachments){
            target->Resize(width, height);
            SetColorAttachment(target, i++);
        }
        if (m_DepthAttachmentSpec.TextureFormat != FrameBufferTextureFormat::None){
            m_DepthAttachment->Resize(width, height);
            SetDepthAttachment(m_DepthAttachment);
        }
    }
    int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
    {   
        OIL_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Max attachment index is 6");

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixelData;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

        return pixelData;
    }
    void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
    {
        OIL_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Max attachment index is 6");

        m_ColorAttachments[attachmentIndex]->Clear(value);
    }
    
    void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, float value)
    {
        OIL_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Max attachment index is 6");

        m_ColorAttachments[attachmentIndex]->Clear(value);
    }
    void OpenGLFrameBuffer::BindColorAttachments()
    {
        int num = m_ColorAttachments.size();
        for(int i = 0; i < num; ++i){
            i, m_ColorAttachments[i]->Bind(i);
        }
    }
    void OpenGLFrameBuffer::BindDepthAttachment()
    {
        int num = m_ColorAttachments.size();
        m_DepthAttachment->Bind(num);

    }

    void OpenGLFrameBuffer::SetColorAttachment(Ref<FrameBufferTarget> tgt, uint32_t slotIndex)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        m_ColorAttachments[slotIndex] = tgt;
        utils::BindTexture(false, tgt->GetRendererID());
        switch (m_ColorAttachmentSpecs[slotIndex].TextureFormat){
            case FrameBufferTextureFormat::RGB8:
                utils::AttachColorTexture(tgt->GetRendererID(), m_Specification.Samples, slotIndex);
                break;
            case FrameBufferTextureFormat::RGBA8:
                utils::AttachColorTexture(tgt->GetRendererID(), m_Specification.Samples, slotIndex);
                break;
            case FrameBufferTextureFormat::RGB16F:
                utils::AttachColorTexture(tgt->GetRendererID(), m_Specification.Samples, slotIndex);
                break;
            case FrameBufferTextureFormat::RGBA16F:
                utils::AttachColorTexture(tgt->GetRendererID(), m_Specification.Samples, slotIndex);
                break;
            case FrameBufferTextureFormat::R_INT:
                utils::AttachColorTexture(tgt->GetRendererID(), m_Specification.Samples, slotIndex);
                break;
            case FrameBufferTextureFormat::R_FLOAT:
                utils::AttachColorTexture(tgt->GetRendererID(), m_Specification.Samples, slotIndex);
                break;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void OpenGLFrameBuffer::SetDepthAttachment(Ref<FrameBufferTarget> tgt)
    {
        if (m_DepthAttachmentSpec.TextureFormat != FrameBufferTextureFormat::None){
            glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
            m_DepthAttachment = tgt;

            utils::BindTexture(false, m_DepthAttachment->GetRendererID());
            switch (m_DepthAttachmentSpec.TextureFormat){
                case FrameBufferTextureFormat::DEPTH24STENCIL8:
                    utils::AttachDepthTexture(m_DepthAttachment->GetRendererID(), m_Specification.Samples, GL_DEPTH_STENCIL_ATTACHMENT);
                    break;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        }
    }

    //Frame buffer target
    OpenGLFrameBufferTarget::OpenGLFrameBufferTarget(FrameBufferTextureFormat format, uint32_t width, uint32_t height)
    {
        m_TextureFormat = format;
        Resize(width, height);
    }
    OpenGLFrameBufferTarget::~OpenGLFrameBufferTarget()
    {
        glDeleteTextures(1, &m_RendererID);
    }
    void OpenGLFrameBufferTarget::Resize(uint32_t width, uint32_t height)
    {
        if( width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize){
            OIL_CORE_WARN("Attempted to resize frame buffer to ({0}, {1})", width, height);
            return;
        }

        if(m_Width == width || m_Height == height){
            OIL_CORE_WARN("Attempted to resize frame buffer to the same dimensions!)");
            return;
        }

        m_Width = width;
        m_Height = height;

        if(m_RendererID){
            glDeleteTextures(1, &m_RendererID);
            m_RendererID = 0;
        }

        bool multisample = false; //TODO: implement multisampling support
        utils::CreateTextures(multisample, &m_RendererID, 1);
        utils::BindTexture(multisample, m_RendererID);

        switch(m_TextureFormat){
            case FrameBufferTextureFormat::RGB8:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
                break;
            case FrameBufferTextureFormat::RGBA8:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                break;
            case FrameBufferTextureFormat::RGB16F:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
                break;
            case FrameBufferTextureFormat::RGBA16F:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
                break;
            case FrameBufferTextureFormat::R_INT:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, nullptr);
                break;
            case FrameBufferTextureFormat::R_FLOAT:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);
                break;
            
            //Depth attachments
            case FrameBufferTextureFormat::DEPTH24STENCIL8:
                glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
                break;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    }
    void OpenGLFrameBufferTarget::Bind(uint32_t slot)
    {
        glBindTextureUnit(slot, m_RendererID);
    }
    void OpenGLFrameBufferTarget::Clear(int value)
    {
        glClearTexImage(m_RendererID, 0, utils::OiltextureFormatToGL(m_TextureFormat), GL_INT, &value);
    }
    void OpenGLFrameBufferTarget::Clear(float value)
    {
        glClearTexImage(m_RendererID, 0, utils::OiltextureFormatToGL(m_TextureFormat), GL_INT, &value);
    }
    inline uint32_t OpenGLFrameBufferTarget::GetRendererID()
    {
        return m_RendererID;
    }
    inline uint32_t OpenGLFrameBufferTarget::GetWidth()
    {
        return m_Width;
    }
    inline uint32_t OpenGLFrameBufferTarget::GetHeight()
    {
        return m_Height;

    }
    inline FrameBufferTextureFormat OpenGLFrameBufferTarget::GetTextureFormat()
    {
        return m_TextureFormat;
    }
}