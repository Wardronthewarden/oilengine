#include "Model.h"
#include "oil/storage/AssetManager.h"

namespace oil{
    Model::Model(){
    }
    Model::~Model(){
    }
    void Model::AddMesh(const Ref<Mesh> mesh)
    {
        m_Meshes.push_back(mesh);
        m_BufferSize += mesh->GetBufferSize();
    }
    AssetHandle Model::GetMaterial(uint32_t index)
    {
        
        if (m_DefaultMaterials.size() < index)
            return m_DefaultMaterials[index];
        return 0;
        
    }
    void Model::SetMaterial(const AssetHandle &mat, uint32_t index)
    {
    
        OIL_CORE_ASSERT(index < m_DefaultMaterials.size(), "Can't set material on Model. Index out of bounds!");
        m_DefaultMaterials[index] = mat;
    
    }
    void Model::SetMaterialsToDefault()
    {
         
        for (auto material : m_DefaultMaterials) material = AssetHandle();
        
    }
    
    template <>
    ContentType AssetRef<Model>::GetType()
    {
        return ContentType::Model;
    }

}