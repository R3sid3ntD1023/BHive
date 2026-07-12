#type vertex
#version 460 core

#include <Fullscreen.vert>

#type fragment
#version 460 core

#include <Sampling.glsl>

layout (location = 0) in vec2 vUV;

layout(set = 1, binding = 0) uniform sampler2D uSrcTexture;

layout(push_constant) uniform BloomSettings
{
    float uFilterRadius;
} pc;

layout(location = 0) out vec4 oColor;

void main()
{
    ivec2 srcSize = textureSize(uSrcTexture, 0);
    vec2 texel = 1.0 / vec2(srcSize);
    float radius = max(pc.uFilterRadius, 0.001);

    vec3 upsample = BicubicSample(uSrcTexture, vUV, texel * radius);

    oColor =  vec4(upsample, 1.0);
}