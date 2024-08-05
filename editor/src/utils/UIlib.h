#pragma once

#include "imgui.h"
#include "oil/core/core.h"
#include "glm/glm.hpp"
#include "oil/Renderer/Texture.h"


namespace oil{

    namespace UI
    {
        //controls
        bool DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f, float columnWidth = 100.0f);
        bool DrawVec2Control(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f);
        bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
        bool DrawVec4Control(const std::string& label, glm::vec4& values, float resetValue = 0.0f, float columnWidth = 100.0f);

        //asset display in browser windows
        bool DrawAssetItemBrowser(Ref<Texture2D> thumbnail, std::string& name, bool nameEditMode, float thumbnailSize = 90.0f, float padding = 10.0f);

        //Asset display for list items
        void DrawAssetItemList(Ref<Texture2D> thumbnail, std::string& name,  float thumbnailSize, float padding, float widthLimit);

        //drag drop utils
        UUID AcceptAssetDrop(ContentType type);
    }//UI
    

}//oil