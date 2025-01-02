#domain engine

#type vertex
#version 440 core

// set up screen quad
layout(location = 0) in vec2 a_Position;   
layout(location = 1) in vec2 a_TexCoord;   

out vec2 v_TexCoord;

void main(){
    //send screen quad over to fragment shader
    gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
    v_TexCoord = a_TexCoord;
}


#type fragment
#version 440 core

in vec2 v_TexCoord;

uniform sampler2D u_InputTexture;
uniform int u_InputComponents;

layout(location = 0) out vec4 o_Color;

//main function
void main(){
    
    switch (u_InputComponents){
        case 1: o_Color = vec4(texture(u_InputTexture, v_TexCoord).r, 0.0, 0.0, 1.0);
            break;
        case 2: o_Color = vec4(texture(u_InputTexture, v_TexCoord).rg, 0.0, 1.0);
            break;
        case 3: o_Color = vec4(texture(u_InputTexture, v_TexCoord).rgb, 1.0);
            break;
        case 4: o_Color = texture(u_InputTexture, v_TexCoord);
            break;
        default: o_Color = vec4(1.0, 0.0, 1.0, 1.0);
    }
    

}