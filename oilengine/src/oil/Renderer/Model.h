#pragma once

#include "Mesh.h"
#include "Material.h"

namespace oil{

    //Forward declarations
    template<typename T>
    class Asset;

    class Model{
    public:
        Model();
        ~Model();

        const uint32_t GetBufferSize() { return m_BufferSize; }

        void AddMesh(const Ref<Mesh> mesh);

        const std::vector<Ref<Mesh>> GetMeshes() { return m_Meshes; };
        const std::vector<Asset<Material>> GetMaterials() { return m_Materials; };
        const Asset<Material> GetMaterial(uint32_t index);

        void SetMaterial(const Asset<Material>& mat, uint32_t index);

        void SetMaterialsToDefault();

    private:
        friend class AssetImporter;
        friend class Serializer;
        
        void SetMaterialCount(const uint32_t count) { m_Materials.resize(count); };

    private:
        std::vector<Ref<Mesh>> m_Meshes;
        std::vector<Asset<Material>> m_Materials;
        uint32_t m_BufferSize = 0;

    };

}