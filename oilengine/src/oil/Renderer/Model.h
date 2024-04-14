#pragma once

#include "Mesh.h"

namespace oil{

    class Model{
    public:
        Model();
        ~Model();

        const uint32_t GetBufferSize() { return m_BufferSize; }

        void AddMesh(const Ref<Mesh> mesh);
        void SubmitForRendering();

        const std::vector<Ref<Mesh>> GetMeshes() { return m_Meshes; };
    private:
        std::vector<Ref<Mesh>> m_Meshes;
        uint32_t m_BufferSize = 0;

    };

}