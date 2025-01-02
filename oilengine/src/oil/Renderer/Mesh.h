#pragma once

#include "VertexArray.h"
#include "Buffer.h"
#include <vector>

#include <glm/glm.hpp>

namespace oil{

    enum class LayoutType{
        None = 0, Base = 1
    };

    struct BaseVertex{
        glm::vec4 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;

        // Editor
        int EntityID = 0;
        
    };

    static BufferLayout baseLayout({
        {ShaderDataType::Float4, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Float2, "a_Texcoord"},
        {ShaderDataType::Int, "a_EntityID"}
    });

    // PLANE

    static const BaseVertex planeVertices[4] = {
        {{-0.5, -0.5, 0.0, 1.0}, {0.5, 0.5, 0.5, 1.0}, {0.0, 0.0}, 0},
        {{ 0.5, -0.5, 0.0, 1.0}, {0.5, 0.5, 0.5, 1.0}, {1.0, 0.0}, 0},
        {{ 0.5,  0.5, 0.0, 1.0}, {0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}, 0},
        {{-0.5,  0.5, 0.0, 1.0}, {0.5, 0.5, 0.5, 1.0}, {0.0, 1.0}, 0}
    };

    static const uint32_t planeIndices[6] = {0, 1, 2, 0, 2, 3};

    // CUBE

    static const BaseVertex cubeVertices[8] = {
        {{-0.5, -0.5,  0.5, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0}, 0},
        {{ 0.5, -0.5,  0.5, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 0.0}, 0},
        {{-0.5,  0.5,  0.5, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 1.0}, 0},
        {{ 0.5,  0.5,  0.5, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0}, 0},
        {{-0.5, -0.5, -0.5, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0}, 0},
        {{ 0.5, -0.5, -0.5, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 0.0}, 0},
        {{-0.5,  0.5, -0.5, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 1.0}, 0},
        {{ 0.5,  0.5, -0.5, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0}, 0}
    };

    static const uint32_t cubeIndices[36] = {
            2, 7, 6, 2, 3, 7, // Top
            0, 4, 5, 0, 5, 1, // Bottom
            0, 2, 6, 0, 6, 4, // Left
            1, 7, 3, 1, 5, 7, // Right
            0, 3, 2, 0, 1, 3, // Front
            4, 6, 7, 4, 7, 5  // Back
    };

    //Sphere generation
    static Ref<DataBuffer<BaseVertex>> GenerateSphereVertices(uint32_t xSegments, uint32_t ySegments);
    static Ref<DataBuffer<uint32_t>> GenerateSphereIndices(uint32_t xSegments, uint32_t ySegments);
    
    class Mesh{
    public:
        Mesh();
        Mesh(const unsigned char* vertexBegin, uint32_t vertexByteSize, const unsigned char* indexBegin, uint32_t indexByteSize);
        Mesh(const unsigned char* vertexBegin, uint32_t vertexByteSize, const unsigned char* indexBegin, uint32_t indexByteSize, BufferLayout layout);
        Mesh(Ref<DataBuffer<unsigned char>> vertices, Ref<DataBuffer<unsigned char>> indices)
            :m_VertexBuffer(vertices), m_IndexBuffer(indices) {};
        Mesh(Ref<DataBuffer<unsigned char>> vertices, Ref<DataBuffer<unsigned char>> indices, uint32_t materialIndex)
            :m_VertexBuffer(vertices), m_IndexBuffer(indices), m_MaterialIndex(materialIndex) {};

        ~Mesh() = default;

        void SetMesh(Ref<DataBuffer<unsigned char>> vertices, Ref<DataBuffer<unsigned char>> indices);

        void SetLayout(BufferLayout layout);

        Ref<DataBuffer<unsigned char>> GetVertexBuffer() const { return m_VertexBuffer; }
        Ref<DataBuffer<unsigned char>> GetIndexBuffer() const { return m_IndexBuffer; }
        BufferLayout GetLayout() const { return m_Layout; }

        uint32_t GetBufferSize() const { return m_VertexBuffer->GetSize() +  m_IndexBuffer->GetSize(); }

        void SetMaterialIndex(uint32_t index) { m_MaterialIndex = index; }
        uint32_t GetMaterialIndex() { return m_MaterialIndex; }
    public:
        static Ref<Mesh> CreatePlane();
        static Ref<Mesh> CreateCube();
        static Ref<Mesh> CreateSphere();

    private:
        Ref<VertexArray> m_VertexArray;
        Ref<DataBuffer<unsigned char>> m_VertexBuffer;
        Ref<DataBuffer<unsigned char>> m_IndexBuffer;
        BufferLayout m_Layout;
        uint32_t m_MaterialIndex;
    };

}