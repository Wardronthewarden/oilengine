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

uniform sampler2D u_Textures[32];


void main(){
    //Todo: TilingFactor
    o_Color = vec4(texture(u_Textures[1], v_TexCoord).rgb, 1.0);
    o_EntityID = 1;
}