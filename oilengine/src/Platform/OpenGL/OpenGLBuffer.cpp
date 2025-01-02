#include "pch/oilpch.h"
#include "OpenGLBuffer.h"

#include <GLAD/glad.h>
#include "OpenGLValidation.h"


// Vertex buffer ----------------------------------------------------------------------------

oil::OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
{
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    GL_VALIDATE("Vertex buffer creation");
}

oil::OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, uint32_t size)
{
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    GL_VALIDATE("Vertex buffer creation");
}

oil::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void oil::OpenGLVertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void oil::OpenGLVertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void oil::OpenGLVertexBuffer::SetData(const void *data, uint32_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    GL_VALIDATE("Vertex buffer data submission");
}

// Uniform buffer ----------------------------------------------------------------------------

oil::OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size)
{
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
    GL_VALIDATE("Uniform buffer creation");
}

oil::OpenGLUniformBuffer::OpenGLUniformBuffer(float *vertices, uint32_t size)
{
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, vertices, GL_STATIC_DRAW);
    GL_VALIDATE("Uniform buffer creation");
}

oil::OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void oil::OpenGLUniformBuffer::Bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
}

void oil::OpenGLUniformBuffer::Unbind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void oil::OpenGLUniformBuffer::SetData(const void *data, uint32_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
    GL_VALIDATE("Uniform buffer data submission");
}

void oil::OpenGLUniformBuffer::SetBinding(uint32_t binding)
{
    m_Binding = binding;
    glBindBufferBase(GL_UNIFORM_BUFFER, m_Binding, m_RendererID);
}

// Index buffer ----------------------------------------------------------------------------

oil::OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
    : m_Count(count)
{   
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
    GL_VALIDATE("Index buffer creation");

}

oil::OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count)
    : m_Count(count)
{
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    GL_VALIDATE("Index buffer creation");
}

oil::OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void oil::OpenGLIndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void oil::OpenGLIndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void oil::OpenGLIndexBuffer::SetData(const void *data, uint32_t count)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(uint32_t), data);
    GL_VALIDATE("Index buffer data submission");
}
