#include <pch/oilpch.h>
#include "SubTexture2D.h"

namespace oil{
    SubTexture2D::SubTexture2D(const Ref<Texture2D> &texture, glm::vec2 &min, glm::vec2 &max)
     : m_Texture(texture)
    {
        m_TexCoords[0] = {min.x, min.y};
        m_TexCoords[1] = {max.x, min.y};
        m_TexCoords[2] = {max.x, max.y};
        m_TexCoords[3] = {min.x, max.y};
    }
    Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2 &spriteCoords, const glm::vec2 &spriteSize, const glm::vec2& cellDimensions)
    {

        glm::vec2 min = { ( spriteCoords.x * spriteSize.x) / texture->GetWidth(), (spriteCoords.y * spriteSize.y) / texture->GetHeight()};
        glm::vec2 max = { ( (spriteCoords.x + cellDimensions.x) * spriteSize.x) / texture->GetWidth(), ((spriteCoords.y + cellDimensions.y) * spriteSize.y) / texture->GetHeight()};

        return CreateRef<SubTexture2D>(texture, min, max);
    }
}