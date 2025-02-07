#type vertex

#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : require

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

layout(std430 , binding = 0) uniform CameraBuffer{ mat4 u_viewprojection;} ;
layout(std430, binding = 1) uniform ModelBuffer {mat4 u_model; };

layout(location = 0) out struct VS_OUT
{
    vec3 position;
    vec2 texcoord;
    vec3 normal;
} vs_out;

void main()
{
    vec4 worldPos = u_model * vec4(vPos, 1);
    vs_out.position = worldPos.xyz;
    vs_out.texcoord = vTexCoord;

    mat3 normal_matrix = transpose(inverse(mat3(u_model)));
    vs_out.normal = normal_matrix * vNormal;

    gl_Position = u_viewprojection * worldPos;
}

#type fragment

#version 460 core

layout(location = 0) in struct VS_OUT
{
    vec3 position;
    vec2 texcoord;
    vec3 normal;
} vs_in;


layout(binding = 0) uniform sampler2D u_Texture;
layout(location = 0) out vec4 fColor;
layout(location = 1) out vec4 fPosition;
layout(location = 2) out vec4 fNormal;

void main()
{
    vec4 color = texture(u_Texture, vs_in.texcoord);
    fColor = color;
    fPosition = vec4(vs_in.position,1);
    fNormal = vec4(vs_in.normal, 1);
}