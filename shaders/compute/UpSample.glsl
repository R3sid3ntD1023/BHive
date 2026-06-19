#type compute
#version 460 core

#define LOCAL_SIZE 1
layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

layout(set = 1, binding = 0) uniform sampler2D uSrcTexture;
layout(set = 1, binding = 1, rgba32f) uniform image2D uOutput;

layout(push_constant) uniform BloomSettings
{
    float uFilterRadius;
    int uSrcMip;
} pc;


void main()
{
    ivec2 dstCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dstSize = imageSize(uOutput);

    vec2 uv = (vec2(dstCoord) + 0.5 )/ vec2(dstSize);

    int mip = pc.uSrcMip;
    ivec2 srcSize = textureSize(uSrcTexture, 0);
    vec2 texel = 1.0 / vec2(srcSize);
    vec2 offset = pc.uFilterRadius * texel;
  
    vec3 upsample = vec3(0);
    upsample += textureLod(uSrcTexture, uv + vec2(-offset.x     ,offset.y), mip).rgb;
    upsample += textureLod(uSrcTexture, uv + vec2(0.0           ,offset.y), mip).rgb;
    upsample += textureLod(uSrcTexture, uv + vec2(offset.x      ,offset.y), mip).rgb;
                                  
    upsample += textureLod(uSrcTexture, uv + vec2(-offset.x     ,0.0), mip).rgb;
    upsample += textureLod(uSrcTexture, uv + vec2(0.0           ,0.0), mip).rgb;
    upsample += textureLod(uSrcTexture, uv + vec2(offset.x      ,0.0), mip).rgb;
                                    
    upsample += textureLod(uSrcTexture, uv + vec2(-offset.x     ,-offset.y), mip).rgb;
    upsample += textureLod(uSrcTexture, uv + vec2(0.0           ,-offset.y), mip).rgb;
    upsample += textureLod(uSrcTexture, uv + vec2(offset.x      ,-offset.y), mip).rgb;

  
    upsample *= (1.0 / 16.0);

    imageStore(uOutput, dstCoord, vec4(upsample, 1));
}