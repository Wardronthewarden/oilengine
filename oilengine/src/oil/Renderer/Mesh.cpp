#include "pch/oilpch.h"
#include "Mesh.h"

namespace oil{
    void Mesh::SetMesh(Mesh mesh)
    {
        m_Vertices = mesh.GetVertices();
        m_Indices = mesh.GetIndices();
        OIL_INFO("Assigned new data to mesh!");

    }
    Mesh Mesh::CreatePlane()
    {
        std::vector<BaseVertex> v(planeVertices, planeVertices+sizeof(planeVertices) / sizeof(BaseVertex));
        std::vector<uint32_t> i(planeIndices, planeIndices+sizeof(planeIndices) / sizeof(uint32_t));
        OIL_INFO("Created plane vertices!");
        return Mesh(v, i);
    }
    Mesh Mesh::CreateCube()
    {
        std::vector<BaseVertex> v(cubeVertices, cubeVertices+sizeof(cubeVertices) / sizeof(BaseVertex));
        std::vector<uint32_t> i(cubeIndices, cubeIndices+sizeof(cubeIndices) / sizeof(uint32_t));
        OIL_INFO("Created cube vertices!");
        return Mesh(v, i);
    }
    Mesh Mesh::CreateSphere()
    {
        return Mesh();
    }
}