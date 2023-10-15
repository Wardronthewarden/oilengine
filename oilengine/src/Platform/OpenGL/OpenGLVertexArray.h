#pragma once

#include "oil/Renderer/VertexArray.h"

namespace oil{
    class OpenGLVertexArray : public VertexArray{
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void AddVertexBuffer(const oil::Ref<VertexBuffer>& VertexBuffer) override;
        virtual void SetIndexBuffer(const oil::Ref<IndexBuffer>& IndexBuffer) override;

        virtual const std::vector<oil::Ref<VertexBuffer>>& GetVertexBuffers() const {return m_VertexBuffers; };
        virtual const oil::Ref<IndexBuffer>& GetIndexBuffer() const {return m_IndexBuffer; };
    private:
        uint32_t m_RendererID;
        std::vector<oil::Ref<VertexBuffer>> m_VertexBuffers;
        oil::Ref<IndexBuffer> m_IndexBuffer;
    };
}