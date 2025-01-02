#domain engine

#type vertex
#version 440 core

//unit cube VBO
layout(location = 0) in vec4 a_Position;  
layout(location = 1) in vec4 a_Color;  
layout(location = 2) in vec2 a_TexCoord;  

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_LocalPos;

void main(){
    //send screen quad over to fragment shader
    v_LocalPos = a_Position.xyz;
    gl_Position = u_Projection * u_View * vec4(a_Position.xyz, 1.0);
}


#type fragment
#version 440 core

layout(location = 0) out vec4 o_Color;

in vec3 v_LocalPos;

uniform samplerCube u_EnvironmentMap;
uniform float u_Roughness;
uniform float u_EnvironmentResolution;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}  

float distributionGGX(vec3 N , vec3 H, float roughness){
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    return a2 / (PI * pow(NdotH2 * (a2 - 1.0) + 1.0 , 2.0));
}

//main function
void main(){
    vec3 N = normalize(v_LocalPos);    
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0); 

    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, u_Roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);

        //Calculate mip level to sample
        float D   = distributionGGX(NdotH, u_Roughness);
        float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001; 

        float saTexel  = 4.0 * PI / (6.0 * u_EnvironmentResolution * u_EnvironmentResolution);
        float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

        float mipLevel = u_Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

        if(NdotL > 0.0)
        {
            prefilteredColor += textureLod(u_EnvironmentMap, L, mipLevel).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    o_Color = vec4(1.0, 1.0, 1.0, 1.0);
}