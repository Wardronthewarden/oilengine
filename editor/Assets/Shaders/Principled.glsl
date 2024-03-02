#type vertex
#version 440 core

uniform mat4 u_VPMat;

layout(location = 0) in vec4 a_Position;  
layout(location = 1) in vec4 a_Color;  
layout(location = 2) in vec2 a_TexCoord;  
layout(location = 3) in int a_EntityID;

out vec4 v_Position;
out vec4 v_Color;
out vec2 v_TexCoord;
out flat int v_EntityID;

void main(){
    gl_Position = u_VPMat * a_Position;
    v_Position = a_Position;
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_EntityID = a_EntityID;
}


#type fragment
#version 440 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out vec4 o_Position;
layout(location = 2) out vec4 o_TexCoord;
layout(location = 3) out int o_EntityID;

in vec4 v_Position;
in vec4 v_Color;
in vec2 v_TexCoord;
in flat int v_EntityID;

uniform sampler2D u_Textures[32];


void main(){
    //Todo: TilingFactor
    o_Color = vec4(1.0, 0.0, 0.0, 1.0);
    o_Position = v_Position;
    o_TexCoord = vec4(0.0, 0.0, 1.0, 1.0);
    o_EntityID = v_EntityID;
}