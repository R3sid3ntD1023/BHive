#type vertex
#version 460 core

#include <Fullscreen.vert>

#type fragment
#version 460 core

layout(location = 0) in vec2 vUV;

layout(set = SET_MATERIAL, binding = 0) uniform sampler2D Scene;

layout(location = 0) out vec4 fColor;

void main()
{
    vec4 color = texture(Scene, vUV);
    fColor = color;
}