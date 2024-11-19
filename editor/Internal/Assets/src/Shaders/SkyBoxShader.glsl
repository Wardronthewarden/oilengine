#domain engine

#type vertex
#version 440 core

// set up screen quad
layout(location = 0) in vec4 a_Position;  
layout(location = 1) in vec4 a_Color;  
layout(location = 2) in vec2 a_TexCoord;  
layout(location = 3) in int a_EntityID;


uniform mat4 u_VPMat;

out vec3 v_TexCoords;
out vec2 s_TexCoords;


void main(){

    v_TexCoords = a_Position.xyz;
    vec4 clipPos = u_VPMat * vec4(a_Position.xyz, 1.0);

    gl_Position = clipPos.xyww;
}


#type fragment
#version 440 core

layout(location = 0) out vec4 o_Color;

in vec3 v_TexCoords;
in vec2 s_TexCoords;

uniform samplerCube u_SkyBoxTexture;

//main function
void main(){
    gl_FragDepth = 1.0;


    vec3 envColor = texture(u_SkyBoxTexture, v_TexCoords).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

    o_Color = vec4(envColor, 1.0);

}