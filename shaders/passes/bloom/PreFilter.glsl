#type vertex
#version 460 core

#include <Fullscreen.vert>

#type fragment
#version 460 core

#include <Core.glsl>
#include <Constants.glsl>

layout(location = 0) in vec2 vUV;

layout(set = SET_MATERIAL, binding = 0) uniform sampler2D uSceneColor;  

layout(push_constant) uniform PushConstants
{
   float Threshold;
} pc;

layout(location = 0) out vec4 oColor;

void main()
{
    float threshold = clamp(pc.Threshold, 0.8f, 5.0f);
    vec3 color = texture(uSceneColor, vUV).rgb;
    vec3 bloom = SoftKneeThreshold(color, threshold);
    oColor = vec4(bloom, 1.0);
}