#include "pch/oilpch.h"
#include "Mesh.h"

namespace oil{
    Mesh::Mesh()
    {
        OIL_CORE_TRACE("Mesh constructor called");
        m_VertexBuffer = CreateRef<DataBuffer<unsigned char>>();
        m_IndexBuffer = CreateRef<DataBuffer<unsigned char>>();
    }
    Mesh::Mesh(const unsigned char *vertexBegin, uint32_t vertexByteSize, const unsigned char *indexBegin, uint32_t indexByteSize)
    {
        m_VertexBuffer = CreateRef<DataBuffer<unsigned char>>(vertexBegin, vertexByteSize);
        m_IndexBuffer = CreateRef<DataBuffer<unsigned char>>(indexBegin, indexByteSize);
    }

    Mesh::Mesh(const unsigned char *vertexBegin, uint32_t vertexByteSize, const unsigned char *indexBegin, uint32_t indexByteSize, BufferLayout layout)
        : Mesh(vertexBegin, vertexByteSize, indexBegin, indexByteSize)
    {
        SetLayout(layout);
    }

    void Mesh::SetMesh(Ref<DataBuffer<unsigned char>> vertices, Ref<DataBuffer<unsigned char>> indices)
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
            CreateRef<DataBuffer<unsigned char>>((unsigned char*)&planeVertices[0], sizeof(planeVertices)),
            CreateRef<DataBuffer<unsigned char>>((unsigned char*)&planeIndices[0], sizeof(planeIndices))
        );
    }
    Ref<Mesh> Mesh::CreateCube()
    {
        OIL_INFO("Creating cube vertices!");
        return CreateRef<Mesh>(
            CreateRef<DataBuffer<unsigned char>>((unsigned char*)&cubeVertices[0], sizeof(cubeVertices)),
            CreateRef<DataBuffer<unsigned char>>((unsigned char*)&cubeIndices[0], sizeof(cubeIndices))
        );
    }
}