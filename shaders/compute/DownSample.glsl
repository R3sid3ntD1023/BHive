#type compute
#version 460 core

#define LOCAL_SIZE 1
layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

layout(set = 1, binding = 0 ) uniform sampler2D uSrcTexture;
layout(set = 1, binding = 1, rgba32f) uniform image2D uOutput;
layout(push_constant) uniform DownsamplingSettings
{
    int uSrcMip;
}pc;

void main() {

    ivec2 dstCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dstSize = imageSize(uOutput);
  
    vec2 uv = (vec2(dstCoord) + 0.5) / vec2(dstSize);

    int mip = pc.uSrcMip;
    ivec2 srcSize = textureSize(uSrcTexture, 0);
    vec2 texel = 1.0 /  vec2(srcSize);

    vec3 a = textureLod(uSrcTexture, uv + texel * vec2(-2, 2), mip).rgb;
    vec3 b = textureLod(uSrcTexture, uv + texel * vec2(0,  2), mip).rgb;
    vec3 c = textureLod(uSrcTexture, uv + texel * vec2(2 , 2), mip).rgb;
                                 
    vec3 d = textureLod(uSrcTexture, uv + texel * vec2(-2, 0), mip).rgb;
    vec3 e = textureLod(uSrcTexture, uv + texel * vec2(0,  0), mip).rgb;
    vec3 f = textureLod(uSrcTexture, uv + texel * vec2(2,  0), mip).rgb;
                                
    vec3 g = textureLod(uSrcTexture, uv + texel * vec2(-2, -2), mip).rgb;
    vec3 h = textureLod(uSrcTexture, uv + texel * vec2(0,  -2), mip).rgb;
    vec3 i = textureLod(uSrcTexture, uv + texel * vec2(2,  -2), mip).rgb;
                                
    vec3 j = textureLod(uSrcTexture, uv + texel * vec2(-1, 1), mip).rgb;
    vec3 k = textureLod(uSrcTexture, uv + texel * vec2(1,  1), mip).rgb;
    vec3 l = textureLod(uSrcTexture, uv + texel * vec2(-1, -1), mip).rgb;
    vec3 m = textureLod(uSrcTexture, uv + texel * vec2(1,  -1), mip).rgb;

    vec3 sum = e*0.125;
    sum += (a+c+g+i)*0.03125;
    sum += (b+d+f+h)*0.0625;
    sum += (j+k+l+m)*0.125;
    sum = max(sum, 0.0001f);

    imageStore(uOutput, dstCoord, vec4(sum, 1.0));
}