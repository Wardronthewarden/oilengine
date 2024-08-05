#domain engine
#model none

#type vertex
#version 440 core

layout(location = 0) in vec2 a_Position;   
layout(location = 1) in vec2 a_TexCoord;   


uniform mat4 u_VPMat;

out vec2 v_TexCoord;

void main(){
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

uniform sampler2D u_Textures[32];



void main(){
    vec3 Albedo = texture(u_Textures[0], v_TexCoord).rgb;
    vec3 FragPos =  texture(u_Textures[1], v_TexCoord).rgb;
    vec3 Normal = texture(u_Textures[2], v_TexCoord).rgb * 2.0 - 1.0;
    float Spec = texture(u_Textures[0], v_TexCoord).a;
    int id = int(texture(u_Textures[4], v_TexCoord).r);

    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightPos = vec3(10.0, 10.0, 5.0);
    
    
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(u_CamPos - FragPos);

    //Attenuation
    float dist = length(lightPos - FragPos);
    float attenuation = 1.0/(1.0f, + 0.09f * dist + 0.032f * (dist * dist));

    //GI
    vec3 lighting = Albedo * 0.1;
    lighting *= attenuation;

    //Diffuse
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * lightColor * Albedo;

    lighting += diffuse * attenuation;

    //specular
    float specularStrength = 0.5;
    
    vec3 reflectDir = reflect(-lightDir, Normal);

    float specCalc = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * specCalc * lightColor * Spec;

    lighting += specular * attenuation;
    


    o_Color = vec4(lighting, 1.0);
    o_EntityID = id;
}