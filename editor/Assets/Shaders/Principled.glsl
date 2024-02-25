#type vertex
#version 440 core

layout(location = 0) in vec4 a_Position;   
layout(location = 1) in vec4 a_Color;   
layout(location = 2) in vec2 a_TexCoord;   
layout(location = 3) in int a_EntityID;

uniform mat4 u_VPMat;

out vec2 v_TexCoord;
out vec4 v_Color;
out flat int v_EntityID;

void main(){
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_EntityID = a_EntityID;
    gl_Position = u_VPMat * a_Position;
}


#type fragment
#version 440 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat int v_EntityID;

uniform sampler2D u_Textures[32];


void main(){
    //Todo: TilingFactor
    o_Color = vec4(v_TexCoord, 0.0, 1.0) * v_Color;
    o_EntityID = v_EntityID;
}