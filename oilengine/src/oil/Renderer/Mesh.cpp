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
    Ref<Mesh> Mesh::CreateSphere()
    {
        OIL_INFO("Creating sphere vertices!");
        Ref<DataBuffer<BaseVertex>> verts = GenerateSphereVertices(64, 64);
        Ref<DataBuffer<uint32_t>> indices = GenerateSphereIndices(64, 64);
        return CreateRef<Mesh>(
            CreateRef<DataBuffer<unsigned char>>((unsigned char*)verts->GetData(), verts->GetSize()),
            CreateRef<DataBuffer<unsigned char>>((unsigned char*)indices->GetData(), indices->GetSize())
        );
    }

    Ref<DataBuffer<BaseVertex>> oil::GenerateSphereVertices(uint32_t xSegments, uint32_t ySegments) {
        const float PI = 3.14159265359f;

        BaseVertex currentVert;
        std::vector<BaseVertex> verts;
        for (unsigned int x = 0; x <= xSegments; ++x)
        {
            for (unsigned int y = 0; y <= ySegments; ++y)
            {
                float xSegment = (float)x / (float)xSegments;
                float ySegment = (float)y / (float)ySegments;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                currentVert.Position = glm::vec4(xPos, yPos, zPos, 1.0);
                currentVert.Color = glm::vec4(1.0, 1.0, 1.0, 1.0);
                currentVert.TexCoord = glm::vec2(xSegment, ySegment);
                currentVert.EntityID = 0;
                //currentVert.Normal = glm::vec3(xPos, yPos, zPos);

                verts.push_back(currentVert);
            }
        }
        return CreateRef<DataBuffer<BaseVertex>>(verts.data(), verts.size());
    }
    Ref<DataBuffer<uint32_t>> GenerateSphereIndices(uint32_t xSegments, uint32_t ySegments)
    {
        bool oddRow = false;
        std::vector<uint32_t> indices;
        for (uint32_t y = 0; y < ySegments; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (uint32_t x = 0; x <= xSegments; ++x)
                {
                    indices.push_back(y       * (xSegments + 1) + x);
                    indices.push_back((y + 1) * (xSegments + 1) + x);
                }
            }
            else
            {
                for (int x = xSegments; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (xSegments + 1) + x);
                    indices.push_back(y       * (xSegments + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        return CreateRef<DataBuffer<uint32_t>>(indices.data(), indices.size());
    }
}