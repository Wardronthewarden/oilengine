#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;   
layout(location = 1) in vec4 a_Color;   
layout(location = 2) in vec2 a_TexCoord;   
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;

uniform mat4 u_VPMat;

out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;
out flat int v_EntityID;

void main(){
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    v_EntityID = a_EntityID;
    gl_Position = u_VPMat * vec4(a_Position, 1.0f);
}


#type fragment
#version 440 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;
in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in flat int v_EntityID;

uniform sampler2D u_Textures[32];


void main(){
    //Todo: TilingFactor
    o_Color = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * v_Color;
    o_EntityID = v_EntityID;
}