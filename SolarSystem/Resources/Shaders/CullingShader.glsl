#type vertex

#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : require

layout(location = 0) in vec3 vPos;

layout(std430 , binding = 0) uniform ObjectBuffer{ mat4 u_projection; mat4 u_view; mat4 u_model;} ;
layout(std430, binding = 0) buffer InstanceBuffer{ mat4 matrices[];};

layout(location = 0) uniform bool uInstanced = false;

void main()
{
    mat4 instance = mat4(1);
    if(uInstanced)
        instance = matrices[gl_InstanceID];

    vec4 worldPos = instance * u_model * vec4(vPos, 1);
    gl_Position = u_projection * u_view * worldPos;
}

#type fragment

#version 460 core

layout(location =  1) uniform uint uIsCulled;
layout(location = 0) out vec4 fColor;


void main()
{
    vec3 color = mix(vec3(0, 1, 0), vec3(1, 0, 0), uIsCulled);
    fColor = vec4(color, 1);
}

