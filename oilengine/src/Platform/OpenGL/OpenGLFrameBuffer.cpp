#include "pch/oilpch.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"

#include "glad/glad.h"
#include "OpenGLValidation.h"

namespace oil{
    static const uint32_t s_MaxFrameBufferSize = 8192;

    namespace utils{

        static bool isFramebufferValid(){
            switch(glCheckFramebufferStatus(GL_FRAMEBUFFER)){
                case GL_FRAMEBUFFER_COMPLETE: return true;
                case GL_FRAMEBUFFER_UNDEFINED:
                    OIL_CORE_ERROR("OPENGL Framebuffer undefined!");
                    return false;
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    OIL_CORE_ERROR("OPENGL Framebuffer incomplete: attachment!");
                    return false;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    OIL_CORE_ERROR("OPENGL Framebuffer incomplete: missing attachment!");
                    return false;
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                    OIL_CORE_ERROR("OPENGL Framebuffer incomplete: draw buffer!");
                    return false;
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                    OIL_CORE_ERROR("OPENGL Framebuffer incomplete: read buffer!");
                    return false;
                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                    OIL_CORE_ERROR("OPENGL Framebuffer incomplete: multisample!");
                    return false;
                case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                    OIL_CORE_ERROR("OPENGL Framebuffer incomplete: layer targets!");
                    return false;
                case GL_FRAMEBUFFER_UNSUPPORTED:
                    OIL_CORE_ERROR("OPENGL Framebuffer unsupported!");
                    return false;
            }
        }


        static void CreateTextures(TextureTarget target, uint32_t* outID, uint32_t count){
            glCreateTextures(NativeToGLTextureTarget(target), count, outID);
        }

        static void BindTexture(TextureTarget target, uint32_t id){
            glBindTexture(NativeToGLTextureTarget(target), id);
        }

        static bool IsDepthFormat(TextureFormat format){
            switch (format){
                case TextureFormat::DEPTH24STENCIL8 : return true;
            }
            return false;
        }

        static void AttachColorTexture(uint32_t id, TextureTarget target, int index){
            //add support for cube maps
            switch (target){
                case TextureTarget::TextureCube:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_CUBE_MAP_POSITIVE_X, id, 0);
                    break;
                default:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, NativeToGLTextureTarget(target), id, 0);
            }
            
        }

        static void AttachDepthTexture(uint32_t id, TextureTarget target, GLenum attachmentType){
            switch (target){
                case TextureTarget::TextureCube:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_CUBE_MAP_POSITIVE_X, id, 0);
                    break;
                default:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, NativeToGLTextureTarget(target), id, 0);
            }
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
                SetColorAttachment(Texture2D::Create(m_Specification.Width, m_Specification.Height, m_ColorAttachmentSpecs[i].TextureFormat), i);
            }   
        }

        if (m_DepthAttachmentSpec.TextureFormat != TextureFormat::None){
            //SetDepthAttachment(Texture2D::Create(m_Specification.Width, m_Specification.Height, m_DepthAttachmentSpec.TextureFormat));
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

        OIL_CORE_ASSERT(utils::isFramebufferValid(), "Framebuffer is incomplete!");

        glBindBuffer(GL_FRAMEBUFFER, 0);

        GL_VALIDATE("Frame buffer creation");

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
        if (m_DepthAttachmentSpec.TextureFormat != TextureFormat::None){
            m_DepthAttachment->Resize(width, height);
            SetDepthAttachment(m_DepthAttachment);
        }
    }
    int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
    {   
        OIL_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Max attachment index is 10");

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixelData;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

        return pixelData;
    }
    void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
    {
        OIL_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Max attachment index is 10");

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
            m_ColorAttachments[i]->Bind(i);
        }
    }
    void OpenGLFrameBuffer::BindAttachment(uint32_t attachmentIndex, uint32_t slot)
    {
        m_ColorAttachments[attachmentIndex]->Bind(slot);
    }
    void OpenGLFrameBuffer::BindDepthAttachment()
    {
        int num = m_ColorAttachments.size();
        m_DepthAttachment->Bind(num);

    }

    void OpenGLFrameBuffer::SetColorAttachment(Ref<Texture> tgt, uint32_t slotIndex)
    {
        OIL_CORE_ASSERT(tgt->GetFormat() == m_ColorAttachmentSpecs[slotIndex].TextureFormat, "Format mismatch when passing texture to framebuffer!");
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        m_ColorAttachments[slotIndex] = tgt;

        utils::BindTexture(tgt->GetTarget(), tgt->GetRendererID());
        utils::AttachColorTexture(tgt->GetRendererID(), tgt->GetTarget(), slotIndex);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void OpenGLFrameBuffer::SetDepthAttachment(Ref<Texture> tgt)
    {
        if (m_DepthAttachmentSpec.TextureFormat != TextureFormat::None && utils::IsDepthFormat(tgt->GetFormat())){
            glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

            utils::BindTexture(tgt->GetTarget(), tgt->GetRendererID());
            switch (m_DepthAttachmentSpec.TextureFormat){
                case TextureFormat::DEPTH24STENCIL8:
                    utils::AttachDepthTexture(tgt->GetRendererID(), tgt->GetTarget(), GL_DEPTH_STENCIL_ATTACHMENT);
                    break;
            }
            m_DepthAttachment = tgt;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        }
    }

    void OpenGLFrameBuffer::SetAttachmentTextureTarget(TextureTarget target = TextureTarget::Texture2D, uint32_t attachmentIndex = 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        utils::AttachColorTexture(m_ColorAttachments[attachmentIndex]->GetRendererID(), target, attachmentIndex);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::SetAttachmentTextureTargetLayer(uint32_t attachmentIndex, uint32_t layerDepthIndex)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        //utils::AttachColorTexture(m_ColorAttachments[attachmentIndex]->GetRendererID(), target, attachmentIndex);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //Frame buffer target
 
}