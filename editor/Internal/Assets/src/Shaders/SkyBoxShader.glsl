#domain engine
#model none

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
    //send screen quad over to fragment shader
    v_TexCoords = a_Position.xyz;
    gl_Position = u_VPMat * vec4(a_Position.xyz, 1.0);
}


#type fragment
#version 440 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec3 v_TexCoords;
in vec2 s_TexCoords;

uniform samplerCube u_SkyBoxTexture;
uniform sampler2D u_SceneDepth;

//main function
void main(){

    gl_FragDepth = texture(u_SceneDepth, s_TexCoords);
    o_Color = texture(u_SkyBoxTexture, v_TexCoords);
    o_EntityID = -1;

}