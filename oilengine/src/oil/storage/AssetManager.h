#pragma once


#ifdef OIL_EDITOR
    #include "EditorAssetManager.h"
    namespace oil{
        using AssetManager = EditorAssetManager;
    }
#else
    #include "RuntimeAssetManager.h"
    namespace oil{
        using AssetManager = RuntimeAssetManager;
    }
#endif

