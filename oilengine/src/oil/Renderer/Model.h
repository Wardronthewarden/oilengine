#pragma once
#include <oil/core/UUID.h>
#include "Mesh.h"

namespace oil{

    using AssetHandle = UUID;

    class Model{
    public:
        Model();
        ~Model();

        const uint32_t GetBufferSize() { return m_BufferSize; }

        void AddMesh(const Ref<Mesh> mesh);

        const std::vector<Ref<Mesh>> GetMeshes() { return m_Meshes; };
        const std::vector<AssetHandle> GetMaterials() { return m_DefaultMaterials; };
        AssetHandle GetMaterial(uint32_t index);
        const uint32_t GetMaterialCount() { return m_DefaultMaterials.size(); }

        void SetMaterial(const AssetHandle& mat, uint32_t index);

        void SetMaterialsToDefault();

    private:
        friend class AssetImporter;
        friend class Serializer;
        
        void SetMaterialCount(const uint32_t count) { m_DefaultMaterials.resize(count); };

    private:
        std::vector<Ref<Mesh>> m_Meshes;
        std::vector<AssetHandle> m_DefaultMaterials;
        uint32_t m_BufferSize = 0;

    };

}