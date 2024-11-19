#pragma once

#include "VertexArray.h"
#include "Buffer.h"
#include <vector>

#include <glm/glm.hpp>

namespace oil{

    struct DirecLightInfo{
        glm::vec4 LightColor;
        glm::vec3 LightDirection;
        float LightIntensity;

        DirecLightInfo()
            : LightColor({1.0f, 1.0f, 1.0f, 1.0f}), LightDirection({1.0f, 0.0f, 0.0f}), LightIntensity(1.0f){}
    };

    
    static BufferLayout DirecLightLayout({
        {ShaderDataType::Float4, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Float, "a_Intensity"},
        {ShaderDataType::Int, "a_EntityID"}
    });

    class DirectionalLight{
    public:
        DirectionalLight();
        ~DirectionalLight();

        void SetLightDirection(const glm::vec3& direction) { m_LightInfo.LightDirection = direction; };
        void SetLightColor(const glm::vec4& color) { m_LightInfo.LightColor = color; };
        void SetLightIntensity(float intensity) { m_LightInfo.LightIntensity = intensity; };

        DirecLightInfo GetLightInfo() { return m_LightInfo; };

    private:
        DirecLightInfo m_LightInfo;
    };

    struct PointLightInfo{
        glm::vec4 LightPosition;
        glm::vec4 LightColor;
        float LightIntensity;
        
        //Entity
        int EntityID; 

        PointLightInfo()
            : LightPosition({0.0f, 0.0f, 0.0f, 0.0f}), LightColor({1.0f, 1.0f, 1.0f, 1.0f}), LightIntensity(1.0f){}
    };

    
    static BufferLayout PointLightLayout({
        {ShaderDataType::Float4, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Float, "a_Intensity"},
        {ShaderDataType::Int, "a_EntityID"}
    });

    class PointLight{
    public:
        PointLight();
        ~PointLight() {}

        void SetLightColor(const glm::vec4& color) { m_LightInfo.LightColor = color; };
        void SetLightIntensity(float intensity) { m_LightInfo.LightIntensity = intensity; };

        PointLightInfo GetLightInfo() { return m_LightInfo; };

        operator bool() const{
            return (bool) m_LightInfo.LightIntensity;
        }

    private:
        PointLightInfo m_LightInfo;
    };
    
    struct SpotLightInfo{
        glm::vec4 LightColor;
        float LightIntensity;

        SpotLightInfo()
            : LightColor({1.0f, 1.0f, 1.0f, 1.0f}), LightIntensity(1.0f){}
    };

    
    static BufferLayout SpotLightLayout({
        {ShaderDataType::Float4, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Float, "a_Intensity"},
        {ShaderDataType::Int, "a_EntityID"}
    });


    class SpotLight{
    public:
        SpotLight();
        ~SpotLight();

    private:
        SpotLightInfo m_LightInfo;
    };
}