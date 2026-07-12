#type vertex
#version 460 core

#include <Fullscreen.vert>

#type fragment
#version 460 core
#include <Core.glsl>

layout(location = 0) in vec2 vUV;

layout(set = 1, binding = 0) uniform sampler2D uSceneColor;  

layout(push_constant) uniform PushConstants
{
   float uThreshold;
} pc;

layout(location = 0) out vec4 oColor;

void main()
{
    float threshold = pc.uThreshold;
    vec3 color = texture(uSceneColor, vUV).rgb;
    vec3 bloom = SoftKeeThreshold(color, threshold);

    oColor = vec4(bloom, 1.0);
}