#type compute
#version 460 core

#include <Sampling.glsl>

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(set = 1, binding = 0) uniform sampler2D uSrcTexture;
layout(set = 1, binding = 1, rgba32f) uniform image2D uOutput;

layout(push_constant) uniform BloomSettings
{
    float uFilterRadius;
    int uSrcMip;
} pc;


void main()
{
    ivec2 dstCoord = ivec2(gl_WorkGroupID.xy * 16 + gl_LocalInvocationID.xy);

    ivec2 dstSize = imageSize(uOutput);

    if(dstCoord.x >= dstSize.x || dstCoord.y >= dstSize.y)
        return;

    vec2 uv = (vec2(dstCoord) + 0.5 )/ vec2(dstSize);

    int mip = pc.uSrcMip;
    ivec2 srcSize = textureSize(uSrcTexture, 0);
    vec2 texel = 1.0 / vec2(srcSize);
    float radius = max(pc.uFilterRadius, 0.001);

    vec3 upsample = BicubicSample(uSrcTexture, uv, texel * radius);

    imageStore(uOutput, dstCoord, vec4(upsample, 1.0));
}