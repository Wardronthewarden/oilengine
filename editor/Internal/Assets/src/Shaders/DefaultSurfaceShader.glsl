#domain surface
#model lit

#type vertex
#version 440 core

uniform mat4 u_VPMat;

layout(location = 0) in vec4 a_Position;  
layout(location = 1) in vec4 a_Color;  
layout(location = 2) in vec2 a_TexCoord;  
layout(location = 3) in int a_EntityID;

out vData{
    vec4 Position;
    vec4 Color;
    vec2 TexCoord;
    flat int EntityID;
}vertex;

void main(){
    gl_Position = u_VPMat * a_Position;
    vertex.Position = a_Position;
    vertex.Color = a_Color;
    vertex.TexCoord = a_TexCoord;
    vertex.EntityID = a_EntityID;
}

#type geometry
#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vData{
    vec4 Position;
    vec4 Color;
    vec2 TexCoord;
    flat int EntityID;
}vertices[];

out f_Data{
    vec4 Position;
    vec4 Color;
    vec3 Normal;
    vec2 TexCoord;
    flat int EntityID;
}frag;

void main(){

    vec3 normal = normalize(cross(vertices[1].Position.rgb - vertices[0].Position.rgb, vertices[2].Position.rgb - vertices[0].Position.rgb));
    int i;
    for (i = 0; i < gl_in.length(); i++){
        frag.Position = vertices[i].Position;
        frag.Color = vertices[i].Color;
        frag.Normal = normal * 0.5 + 0.5;
        frag.TexCoord = vertices[i].TexCoord;
        frag.EntityID = vertices[i].EntityID;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}


#type fragment
#version 440 core

uniform vec4 u_Color = vec4(1.0);
uniform sampler2D u_DiffuseMap;

layout(location = 0) out vec4 o_Color;
layout(location = 1) out vec4 o_Position;
layout(location = 2) out vec3 o_Normal;
layout(location = 3) out vec4 o_TexCoord;
layout(location = 4) out float o_Metallic;
layout(location = 5) out float o_Roughness;
layout(location = 6) out float o_AO;
layout(location = 7) out float o_EntityID;

in f_Data{
    vec4 Position;
    vec4 Color;
    vec3 Normal;
    vec2 TexCoord;
    flat int EntityID;
}frag;


void main(){


    //default outputs

    o_Color = frag.Color * u_Color * texture(u_DiffuseMap, frag.TexCoord);
    o_Position = frag.Position;
    o_Normal = frag.Normal;
    o_TexCoord = vec4(frag.TexCoord, 0.0, 1.0);
    o_Metallic = 0.0;
    o_Roughness = 0.8;
    o_AO = 1.0;
    
    o_EntityID = float(frag.EntityID);
}