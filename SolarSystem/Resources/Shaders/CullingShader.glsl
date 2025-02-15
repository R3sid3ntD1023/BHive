#type vertex

#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : require

layout(location = 0) in vec3 vPos;
layout(location = 8) in mat4 vInstanceMatrix;

layout(std430 , binding = 0) uniform ObjectBuffer{ mat4 u_projection; mat4 u_view; mat4 u_model;} ;

void main()
{
    vec4 worldPos = u_model * vInstanceMatrix * vec4(vPos, 1);
    gl_Position = u_projection * u_view * worldPos;
}

#type fragment

#version 460 core

layout(location =  0) uniform uint uIsCulled;
layout(location = 0) out vec4 fColor;


void main()
{
    vec3 color = mix(vec3(0, 1, 0), vec3(1, 0, 0), uIsCulled);
    fColor = vec4(color, 1);
}

