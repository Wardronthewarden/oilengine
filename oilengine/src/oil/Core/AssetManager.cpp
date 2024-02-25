#include "pch/oilpch.h"
#include "AssetManager.h"

namespace oil{
    void AssetManager::OnDragAsset(DragDropInfo asset)
    {
        m_DragDropInfo.contentType = asset.contentType;
        m_DragDropInfo.itemPath = asset.itemPath;
    }

    DragDropInfo AssetManager::GetDragDropInfo()
    {
        return m_DragDropInfo;
    }
}