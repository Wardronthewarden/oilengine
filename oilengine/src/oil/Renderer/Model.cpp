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
    AssetRef<Material> Model::GetMaterial(uint32_t index)
    {
        
        if (m_Materials.size() < index)
            return m_Materials[index];
        return AssetRef<Material>();
        
    }
    void Model::SetMaterial(const AssetRef<Material> &mat, uint32_t index)
    {
    
        OIL_CORE_ASSERT(index < m_Materials.size(), "Can't set material on Model. Index out of bounds!");
        m_Materials[index] = mat;
    
    }
    void Model::SetMaterialsToDefault()
    {
         
        for (auto material : m_Materials) material = AssetRef<Material>();
        
    }
    
    template <>
    ContentType AssetRef<Model>::GetType()
    {
        return ContentType::Model;
    }

}