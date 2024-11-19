#domain engine

#type vertex
#version 440 core

layout(location = 0) in vec2 a_Position;   
layout(location = 1) in vec2 a_TexCoords;   

out vec2 v_TexCoords;

void main(){
    v_TexCoords = a_TexCoords;
    gl_Position = vec4(a_Position.x, a_Position.y, 1.0, 1.0);
}



#type fragment
#version 440 core

layout(location = 0) out vec4 o_Lighting;

in vec2 v_TexCoords;

uniform samplerCube u_DiffuseIrradiance;
uniform samplerCube u_SpecularIrradiance;

uniform sampler2D u_Albedo;
uniform sampler2D u_Position;
uniform sampler2D u_Normal;
uniform sampler2D u_Texcoord;
uniform sampler2D u_Metallic;
uniform sampler2D u_Roughness;
uniform sampler2D u_AO;

uniform vec3 u_CamPos;

//function declarations
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


//main function
void main(){
    //setup
    gl_FragDepth = 1.0;

    vec3 albedo = texture(u_Albedo, v_TexCoords).rgb;
    float metallic = texture(u_Metallic, v_TexCoords).r;
    float roughness = texture(u_Roughness, v_TexCoords).r;
    float AO = texture(u_AO, v_TexCoords).r;

    vec3 normal = normalize(texture(u_Normal, v_TexCoords).rgb);

    vec3 worldPos = texture(u_Position, v_TexCoords).rgb;

    vec3 viewDir = normalize(u_CamPos - worldPos);


    vec3 F0 = vec3(0.4);
    F0 = mix(F0, albedo, metallic);

    vec3 kS = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(u_DiffuseIrradiance, normal).rgb;
    vec3 diffuse = irradiance * albedo;
    vec3 ambient = (kD * diffuse); //* AO;



    o_Lighting = vec4(ambient, 1.0);
    //o_Lighting = vec4(1.0);
}