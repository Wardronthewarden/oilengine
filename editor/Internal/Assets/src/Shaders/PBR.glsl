#domain engine
#model none

#type vertex
#version 440 core

// set up screen quad
layout(location = 0) in vec2 a_Position;   
layout(location = 1) in vec2 a_TexCoord;   


uniform mat4 u_VPMat;

out vec2 v_TexCoord;

void main(){
    //send screen quad over to fragment shader
    v_TexCoord = a_TexCoord;
    gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
}


#type fragment
#version 440 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec2 v_Position;
in vec2 v_TexCoord;

uniform vec3 u_CamPos;

//maps
uniform sampler2D u_Albedo;
uniform sampler2D u_Position;
uniform sampler2D u_Normal;
uniform sampler2D u_Texcoord;
uniform sampler2D u_Metallic;
uniform sampler2D u_Roughness;
uniform sampler2D u_AO;
uniform sampler2D u_ID;

uniform sampler2D u_SceneDepth;

uniform vec3 u_LightPos;



const float PI = 3.14159265359;

//function declarations
vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float distributionGGX(vec3 N , vec3 H, float roughness){
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    return a2 / (PI * pow(NdotH2 * (a2 - 1.0) + 1.0 , 2.0));
}

float geometrySchlickGGX(float NdotV, float roughness){
    float k = pow(roughness + 1.0, 2.0) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    float ggx1 = geometrySchlickGGX(NdotV, roughness);
    float ggx2 = geometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

//main function
void main(){
    //variable setup
    //engine data
    int id = int(texture(u_ID, v_TexCoord).r);


    //pbr
    vec3 albedo = texture(u_Albedo, v_TexCoord).rgb;
    float metallic = texture(u_Metallic, v_TexCoord).r;
    float roughness = texture(u_Roughness, v_TexCoord).r;
    float AO = texture(u_AO, v_TexCoord).r;

    vec3 normal = normalize(texture(u_Normal, v_TexCoord).rgb);
    vec3 worldPos = texture(u_Position, v_TexCoord).rgb;

    vec3 viewDir = normalize(u_CamPos - worldPos);

    //temp
    vec3 lightPos = vec3(0.0, 0.0, 2.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    int noLights = 1;


    vec3 F0 = vec3(0.4);
    F0 = mix(F0, albedo, metallic);


    //render all lights
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < noLights; i++){
        vec3 lightDir = normalize(lightPos - worldPos);
        vec3 halfwayVector = normalize(lightDir + viewDir);

        float dist = length(lightPos - worldPos);
        float attenuation = 1/(dist * dist);

        vec3 radiance = lightColor * attenuation;

        

        vec3 F = fresnelSchlick(max(dot(halfwayVector, viewDir), 0.0), F0);
        float NDF = distributionGGX(normal, halfwayVector, roughness);
        float G = geometrySmith(normal, viewDir, lightDir, roughness);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic;


        float NdotL = max(dot(normal, lightDir), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    }

    vec3 ambient = vec3(0.03) * albedo; // * AO
    vec3 color = ambient + Lo;
    
    //gamma correction

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    o_Color = vec4(color, 1.0);
    o_EntityID = id;

}