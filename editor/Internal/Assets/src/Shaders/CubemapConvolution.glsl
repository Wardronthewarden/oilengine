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

uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;

void main()
{		
    // the sample direction equals the hemisphere's orientation 
    vec3 normal = normalize(v_LocalPos);
  
    vec3 irradiance = vec3(0.0);
  
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up         = normalize(cross(normal, right));

    float sampleDelta = 0.025;
    float nrSamples = 0.0; 
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

            irradiance += texture(u_EnvironmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    o_Color = vec4(irradiance, 1.0);
}