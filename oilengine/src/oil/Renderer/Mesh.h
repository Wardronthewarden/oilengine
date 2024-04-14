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

    
    class Mesh{
    public:
        Mesh() = default; //TODO: empty mesh object
        Mesh(const unsigned char* vertexBegin, uint32_t vertexByteSize, const unsigned char* indexBegin, uint32_t indexByteSize, BufferLayout layout);
        Mesh(std::vector<BaseVertex> vertices, std::vector<uint32_t> indices)
            :m_Vertices(vertices), m_Indices(indices) {};

        ~Mesh() = default;

        void SetMesh(Mesh mesh);
        void SetMesh(std::vector<BaseVertex> vertices, std::vector<uint32_t> indices);

        void SetLayout(BufferLayout layout) { m_Layout = layout; }

        std::vector<BaseVertex> GetVertices() const { return m_Vertices; }
        std::vector<uint32_t> GetIndices() const { return m_Indices; }

        uint32_t GetBufferSize() const { return (sizeof(BaseVertex) * m_Vertices.size()) + (sizeof(uint32_t) * m_Indices.size()); }


    public:
        static Mesh CreatePlane();
        static Mesh CreateCube();
        static Mesh CreateSphere();

    private:
        //Use binary memory block to store vertex data, its size, and store layout directly
        std::vector<BaseVertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
        BufferLayout m_Layout;
        
    };

}