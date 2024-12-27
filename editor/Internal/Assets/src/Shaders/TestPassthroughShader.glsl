#domain engine

#type vertex
#version 440 core

// set up screen quad
layout(location = 0) in vec2 a_Position;   

void main(){
    //send screen quad over to fragment shader
    gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
}


#type fragment
#version 440 core

layout(location = 0) out vec4 o_Color;

//main function
void main(){
    
    o_Color = vec4(1.0, 0.0, 0.0, 1.0);

}