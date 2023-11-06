#include "pch/oilpch.h"
#include "OpenGLVertexArray.h"

#include <GLAD/glad.h>


namespace oil{
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type){
    switch (type){
        case ShaderDataType::Float: return GL_FLOAT;
        case ShaderDataType::Float2: return GL_FLOAT;
        case ShaderDataType::Float3: return GL_FLOAT;
        case ShaderDataType::Float4: return GL_FLOAT;
        case ShaderDataType::Mat3:  return GL_FLOAT;
        case ShaderDataType::Mat4:  return GL_FLOAT;
        case ShaderDataType::Int:   return GL_INT;
        case ShaderDataType::Int2:  return GL_INT;
        case ShaderDataType::Int3:  return GL_INT;
        case ShaderDataType::Int4:  return GL_INT;
        case ShaderDataType::Bool:  return GL_BOOL;
    }

    OIL_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
};

OpenGLVertexArray::OpenGLVertexArray()
{
    glCreateVertexArrays(1, &m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &m_RendererID);
}

void OpenGLVertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}

void OpenGLVertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(const oil::Ref<VertexBuffer> &VertexBuffer)
{
    glBindVertexArray(m_RendererID);
    VertexBuffer->Bind();

    OIL_CORE_ASSERT(VertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layouts!");

    uint32_t index = 0;
    const auto& layout = VertexBuffer->GetLayout();
    for (const auto& element : layout){
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(
                index, element.GetComponentCount(), 
                ShaderDataTypeToOpenGLBaseType(element.Type), 
                element.Normalized ? GL_TRUE : GL_FALSE, 
                layout.GetStride(), 
                (const void*)element.Offset
            );
        ++index;
    }
    m_VertexBuffers.push_back(VertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const oil::Ref<IndexBuffer> &IndexBuffer)
{
    glBindVertexArray(m_RendererID);
    IndexBuffer->Bind();

    m_IndexBuffer = IndexBuffer;
}

}