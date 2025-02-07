#type vertex

#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : require

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;

layout(std430 , binding = 0) uniform CameraBuffer{ mat4 u_viewprojection;} ;
layout(std430, binding = 1) uniform ModelBuffer {mat4 u_model; };

layout(location = 0) out struct VS_OUT
{
    vec3 pos;
    vec2 texcoord;
} vs_out;

void main()
{
    gl_Position = u_viewprojection * u_model * vec4(vPos, 1);
    vs_out.pos = vPos;
    vs_out.texcoord = vTexCoord;
}

#type fragment

#version 460 core

layout(location = 0) in struct VS_OUT
{
    vec3 pos;
    vec2 texcoord;
} vs_in;


layout(binding = 0) uniform sampler2D u_Texture;
layout(location = 0) out vec4 fColor;
//layout(location = 1) out vec3 fPosition;

void main()
{
    vec4 color = texture(u_Texture, vs_in.texcoord);
    fColor = color;
    //fPosition = vs_in.pos;
}