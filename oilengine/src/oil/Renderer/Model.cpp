#pragma once
#include "Model.h"

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
    void Model::SubmitForRendering()
    {
    }
}