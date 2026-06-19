#type compute
#version 460 core

#define LOCAL_SIZE 1
layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

layout(set = 1, binding = 0) uniform sampler2D uSrcTexture;
layout(set = 1, binding = 1, r11f_g11f_b10f) uniform image2D uOutput;

layout(push_constant) uniform BloomSettings
{
    float uFilterRadius;
} pc;


void main()
{
    ivec2 dstCoord = ivec2(gl_GlobalInvocationID.xy);

    vec2 uv = (vec2(dstCoord) + 0.5 )/ vec2(gl_NumWorkGroups.xy);
    vec2 texel = vec2(pc.uFilterRadius);

    vec3 upsample = vec3(0);
    upsample += texture(uSrcTexture, uv + texel * vec2(-1,  1)).rgb;
    upsample += texture(uSrcTexture, uv + texel * vec2(0,   1)).rgb;
    upsample += texture(uSrcTexture, uv + texel * vec2(1,   1)).rgb;
                                  
    upsample += texture(uSrcTexture, uv + texel * vec2(-1,  0)).rgb;
    upsample += texture(uSrcTexture, uv + texel * vec2(0,   0)).rgb;
    upsample += texture(uSrcTexture, uv + texel * vec2(1,   0)).rgb;
                                    
    upsample += texture(uSrcTexture, uv + texel * vec2(-1,  -1)).rgb;
    upsample += texture(uSrcTexture, uv + texel * vec2(0,   -1)).rgb;
    upsample += texture(uSrcTexture, uv + texel * vec2(1,   -1)).rgb;

  
    upsample *= (1.0 / 16.0);

    vec3 color = texture(uSrcTexture, uv ).rgb;
    imageStore(uOutput, dstCoord, vec4(color + upsample, 1));
}