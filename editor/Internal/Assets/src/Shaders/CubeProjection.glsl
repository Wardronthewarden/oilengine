#domain engine

#type vertex
#version 440 core

//unit cube VBO
layout(location = 0) in vec4 a_Position;  
layout(location = 1) in vec4 a_Color;  
layout(location = 2) in vec2 a_TexCoord;  

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_LocalPos;

void main(){
    //send screen quad over to fragment shader
    v_LocalPos = a_Position.xyz;
    gl_Position = u_Projection * u_View * vec4(a_Position.xyz, 1.0);
}


#type fragment
#version 440 core

layout(location = 0) out vec4 o_Color;

in vec3 v_LocalPos;

uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSpphericalMap(vec3 v){
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
//main function
void main(){
    vec2 uv = SampleSpphericalMap(normalize(v_LocalPos));
    vec3 color = texture(u_EquirectangularMap, uv).rgb;

    o_Color = vec4(color, 1.0);
    //o_Color = vec4(1.0, 0.0, 0.0, 1.0);
}