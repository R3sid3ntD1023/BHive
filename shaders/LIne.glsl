#type vertex
#version 460  core

#extension GL_EXT_scalar_block_layout: require

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in int vEntityID;

// @semantic Camera
layout(std140, set = 0, binding = 0) uniform CameraBuffer
{
    mat4 uProjection;
    mat4 uView;
    vec2 uNearFar;
    vec4 uCameraPosition;
};

layout(location = 0) out struct VS_OUT
{
    vec4 color;
} vs_out;
        
void main()
{
    gl_Position = uProjection * uView * vec4(vPosition, 1.0);
    vs_out = VS_OUT(vColor);
}

#type fragment
#version 460 core

layout(location = 0) in struct VS_OUT
{
    vec4 color;
} vs_in;

layout(location = 0) out vec4 fColor;

void main()
{
    fColor = vs_in.color;
}
