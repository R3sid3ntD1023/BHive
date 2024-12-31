#type vertex
#version 460 core

layout(location = 0) in vec3 vPosition;


void main()
{
    gl_Position = vec4(vPosition, 1); 
}

#type geometry
#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

layout(location = 0) uniform mat4 u_projection;
layout(location = 1) uniform mat4 u_views[6];

layout(location = 0) out struct vertex_out
{
    vec3 position;
} vs_out;


void main()
{
	
    for(int f = 0; f < 6; f++)
    {
         gl_Layer = f;
        for(int i = 0; i < gl_in.length(); i++)
        {
            vec4 pos = gl_in[i].gl_Position;
            vs_out.position = pos.xyz;
            gl_Position = u_projection * u_views[f] * pos;
            
            
            EmitVertex();
        }
            
        EndPrimitive();
    }
}


#type fragment
#version 460 core

#define PI 3.14159265359

layout(location = 0) in struct vertex_output
{
	vec3 position;
} vs_in;

layout(binding = 0) uniform samplerCube environmentMap;

layout(location = 0) out vec4 fs_out;

void main()
{
    vec3 normal = normalize(vs_in.position);

    vec3 irradiance = vec3(0);
    vec3 up = vec3(0, 1, 0);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float samplesDelta = 0.025;
    float nrSamples = 0;
    for(float phi = 0; phi < 2.0 * PI; phi += samplesDelta)
    {
        for(float theta = 0; theta < 0.5 * PI; theta += samplesDelta)
        {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    fs_out = vec4(irradiance, 1);
}