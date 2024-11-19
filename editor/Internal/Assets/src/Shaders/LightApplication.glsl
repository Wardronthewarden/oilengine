#domain engine

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

in vec2 v_TexCoord;

uniform sampler2D u_LightingMap;
uniform sampler2D u_SceneColor;

//main function
void main(){
    o_Color = texture(u_SceneColor, v_TexCoord) * texture(u_LightingMap, v_TexCoord);
}