#pragma once

#include "oil/Renderer/Buffer.h"

namespace oil{
    class OpenGLVertexBuffer : public VertexBuffer{
    public:
        OpenGLVertexBuffer(uint32_t size);
        OpenGLVertexBuffer(float* vertices, uint32_t size);
        virtual ~OpenGLVertexBuffer();

        virtual void Bind() const;
        virtual void Unbind() const;

        virtual void SetData(const void* data, uint32_t size) override;

        virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
        virtual const BufferLayout& GetLayout() const override { return m_Layout; }
    private:
        uint32_t m_RendererID;
        BufferLayout m_Layout;
    };

    class OpenGLUniformBuffer : public UniformBuffer{
    public:
        OpenGLUniformBuffer(uint32_t size);
        OpenGLUniformBuffer(float* vertices, uint32_t size); 
        virtual ~OpenGLUniformBuffer();

        virtual void Bind() const;
        virtual void Unbind() const;

        virtual void SetData(const void* data, uint32_t size) override;

        virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
        virtual const BufferLayout& GetLayout() const override { return m_Layout; }

        
        virtual void SetBinding(uint32_t binding);
        virtual const uint32_t GetBinding() {return m_Binding; };
    private:
        uint32_t m_RendererID;
        uint32_t m_Binding;
        BufferLayout m_Layout;
    };

    class OpenGLIndexBuffer : public IndexBuffer{
    public:
        OpenGLIndexBuffer(uint32_t count);
        OpenGLIndexBuffer(uint32_t* vertices, uint32_t count);
        virtual ~OpenGLIndexBuffer();

        virtual void Bind() const;
        virtual void Unbind() const;

        virtual void SetData(const void* data, uint32_t count) override;

        virtual uint32_t GetCount() const { return m_Count; }

    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };
}