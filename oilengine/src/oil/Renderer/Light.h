#pragma once

#include "VertexArray.h"
#include "Buffer.h"
#include <vector>

#include <glm/glm.hpp>

namespace oil{
    struct LightInfoBase{
        glm::vec3 LightColor;
        float LightIntensity;

        LightInfoBase()
            : LightColor({1.0f, 1.0f, 1.0f}), LightIntensity(1.0f){}
    };

    class DirectionalLight{
    public:
        DirectionalLight();
        ~DirectionalLight();

        void SetLightDirection(const glm::vec3& direction) { m_LightDirection = direction; };
        void SetLightColor(const glm::vec3& color) { m_LightInfo.LightColor = color; };
        void SetLightIntensity(float intensity) { m_LightInfo.LightIntensity = intensity; };

        LightInfoBase GetLightInfo() { return m_LightInfo; };
        glm::vec3 GetLightDirection() { return m_LightDirection; };

    private:
        LightInfoBase m_LightInfo;
        glm::vec3 m_LightDirection;
    };

    class PointLight{
    public:
        PointLight();
        ~PointLight();

        void SetLightColor(const glm::vec3& color) { m_LightInfo.LightColor = color; };
        void SetLightIntensity(float intensity) { m_LightInfo.LightIntensity = intensity; };

        LightInfoBase GetLightInfo() { return m_LightInfo; };

    private:
        LightInfoBase m_LightInfo;
    };

    class SpotLight{
    public:
        SpotLight();
        ~SpotLight();

    private:
        LightInfoBase m_LightInfo;
    };
}