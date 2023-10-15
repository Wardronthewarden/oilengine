#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;   
layout(location = 1) in vec2 a_TexCoord;   

uniform mat4 u_VPMat;
uniform mat4 u_Transform;

out vec2 v_TexCoord;

void main(){
    v_TexCoord = a_TexCoord;
    gl_Position = u_VPMat * u_Transform * vec4(a_Position, 1.0f);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 o_Color;    
in vec3 v_Position;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main(){
    o_Color = texture(u_Texture, v_TexCoord);
}