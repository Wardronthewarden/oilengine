#include "pch/oilpch.h"
#include "Mesh.h"

namespace oil{
    Mesh::Mesh()
    {
        OIL_CORE_TRACE("Mesh constructor called");
        m_VertexBuffer = CreateRef<CharBuffer>();
        m_IndexBuffer = CreateRef<CharBuffer>();
    }
    Mesh::Mesh(const unsigned char *vertexBegin, uint32_t vertexByteSize, const unsigned char *indexBegin, uint32_t indexByteSize)
    {
        m_VertexBuffer = CreateRef<CharBuffer>(vertexBegin, vertexByteSize);
        m_IndexBuffer = CreateRef<CharBuffer>(indexBegin, indexByteSize);
    }

    Mesh::Mesh(const unsigned char *vertexBegin, uint32_t vertexByteSize, const unsigned char *indexBegin, uint32_t indexByteSize, BufferLayout layout)
        : Mesh(vertexBegin, vertexByteSize, indexBegin, indexByteSize)
    {
        SetLayout(layout);
    }

    void Mesh::SetMesh(Ref<CharBuffer> vertices, Ref<CharBuffer> indices)
    {
        m_VertexBuffer = vertices;
        m_IndexBuffer = indices;
    }
    void Mesh::SetLayout(BufferLayout layout)
    {
        m_Layout = layout;
    }
    Ref<Mesh> Mesh::CreatePlane()
    {
        OIL_INFO("Creating plane vertices!");
        return CreateRef<Mesh>(
            CreateRef<CharBuffer>((unsigned char*)&planeVertices[0], sizeof(planeVertices)),
            CreateRef<CharBuffer>((unsigned char*)&planeIndices[0], sizeof(planeIndices))
        );
    }
    Ref<Mesh> Mesh::CreateCube()
    {
        OIL_INFO("Creating cube vertices!");
        return CreateRef<Mesh>(
            CreateRef<CharBuffer>((unsigned char*)&cubeVertices[0], sizeof(cubeVertices)),
            CreateRef<CharBuffer>((unsigned char*)&cubeIndices[0], sizeof(cubeIndices))
        );
    }
}